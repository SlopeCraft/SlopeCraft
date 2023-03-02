#include "ParseResourcePack.h"
#include "VCL_internal.h"
#include <ColorManip/ColorManip.h>

bool VCL_resource_pack::copy(const VCL_resource_pack &src) noexcept {

  this->textures_original = src.textures_original;
  this->textures_override = src.textures_override;
  this->block_states = src.block_states;
  this->block_models = src.block_models;
  this->colormap_foliage = src.colormap_foliage;
  this->colormap_grass = src.colormap_grass;

  std::unordered_map<const block_model::EImgRowMajor_t *,
                     const block_model::EImgRowMajor_t *>
      update_textures;
  update_textures.reserve(this->textures_original.size() +
                          textures_override.size());

  for (const auto &pair_src : src.textures_original) {
    auto it_dst = this->textures_original.find(pair_src.first);

    update_textures.emplace(&pair_src.second, &it_dst->second);
  }

  for (const auto &pair_src : src.textures_override) {
    auto it_dst = this->textures_override.find(pair_src.first);

    update_textures.emplace(&pair_src.second, &it_dst->second);
  }

  update_textures.emplace(nullptr, nullptr);

  const bool ok = this->filter_model_textures(update_textures, true);

  if (!ok) {
    VCL_report(VCL_report_type_t::error, "Failed to copy resource pack.");
    return false;
  }
  return true;
}

bool VCL_resource_pack::filter_model_textures(
    const std::unordered_map<const block_model::EImgRowMajor_t *,
                             const block_model::EImgRowMajor_t *> &filter,
    bool is_missing_error) noexcept {

  for (auto &pair : this->block_models) {
    for (auto &ele : pair.second.elements) {
      for (auto &face : ele.faces) {
        auto it = filter.find(face.texture);
        if (it != filter.end()) {
          face.texture = it->second;
        } else {
          if (is_missing_error) {
            std::string msg =
                fmt::format("Failed to filter image pointer {}. Missing in the "
                            "filter and is_missing_error is set to true.",
                            (const void *)face.texture);
            VCL_report(VCL_report_type_t::error, msg.c_str());
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool parse_single_state_expression(const char *const beg, const char *const end,
                                   resource_json::state *state) noexcept {
  const char *ptr_eq = nullptr;

  for (const char *cur = beg; cur < end; cur++) {
    if (*cur == '=') {
      if (ptr_eq != nullptr) {
        // multiple '=' in one block state
        return false;
      }

      ptr_eq = cur;
      break;
    }
  }

  if (ptr_eq == nullptr) {
    std::string msg = "state expression dosen\'t contain '=' : \"";
    msg.append(beg, end);
    msg.append("\"\n");
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  state->key = std::string(beg, ptr_eq);
  state->value = std::string(ptr_eq + 1, end);

  return true;
}

bool process_block_state_list_in_id(
    const char *const beg, const char *const end,
    resource_json::state_list *states) noexcept {
  if (beg == end) {
    return true;
  }
  const char *cur_statement_beg = beg;
  for (const char *cur = beg;; cur++) {
    if (*cur == ',' || cur == end) {
      resource_json::state s;

      if (!parse_single_state_expression(cur_statement_beg, cur, &s)) {
        return false;
      }

      states->emplace_back(std::move(s));

      cur_statement_beg = cur + 1;
    }

    if (cur >= end)
      break;
  }

  return true;
}

bool resource_json::process_full_id(std::string_view full_id,
                                    std::string *namespace_name,
                                    std::string *pure_id,
                                    state_list *states) noexcept {
  if (namespace_name != nullptr)
    *namespace_name = "";
  if (pure_id != nullptr)
    pure_id->clear();
  if (states != nullptr)
    states->clear();

  if (full_id.find_first_of(':') != full_id.find_last_of(':')) {
    return false;
  }
  if (full_id.find_first_of('[') != full_id.find_last_of('[')) {
    return false;
  }
  if (full_id.find_first_of(']') != full_id.find_last_of(']')) {
    return false;
  }

  const size_t idx_colon = full_id.find_first_of(':');

  const size_t idx_left_bracket = full_id.find_first_of('[');

  const size_t idx_right_bracket = full_id.find_last_of(']');

  const bool have_left_bracket = (idx_left_bracket != full_id.npos);
  const bool have_right_bracket = (idx_right_bracket != full_id.npos);

  if (have_left_bracket != have_right_bracket)
    return false;

  if (idx_colon != full_id.npos && idx_colon > idx_left_bracket)
    return false;

  if (have_left_bracket && (idx_left_bracket >= idx_right_bracket)) {
    return false;
  }

  // get namespace name
  if (idx_colon != full_id.npos && namespace_name != nullptr) {
    *namespace_name = full_id.substr(idx_colon);
  }

  // get pure id
  const size_t pure_id_start_idx =
      (idx_colon == full_id.npos) ? (0) : (idx_colon + 1);
  const size_t pure_id_end_idx =
      (have_left_bracket) ? (idx_left_bracket) : (full_id.length());

  if (pure_id) {
    *pure_id =
        full_id.substr(pure_id_start_idx, pure_id_end_idx - pure_id_start_idx);
  }

  if (!have_left_bracket || states == nullptr) {
    return true;
  }

  if (!process_block_state_list_in_id(full_id.data() + idx_left_bracket + 1,
                                      full_id.data() + idx_right_bracket,
                                      states)) {
    return false;
  }

  return true;
}

std::variant<const block_model::model *, block_model::model>
VCL_resource_pack::find_model(const std::string &block_state_str,
                              VCL_face_t face_exposed,
                              VCL_face_t *face_invrotated,
                              buffer_t &buffer) const noexcept {

  if (!resource_json::process_full_id(block_state_str, nullptr, &buffer.pure_id,
                                      &buffer.state_list)) {
    std::string msg =
        fmt::format("invalid full block id that can not be parsed to a list "
                    "of block states : \"{}\"",
                    block_state_str.c_str());

    VCL_report(VCL_report_type_t::error, msg.c_str());
    return nullptr;
  }

  constexpr bool display_statelist_here = false;
  if (display_statelist_here) {
    std::string msg = "statelist = [";

    for (const auto &i : buffer.state_list) {
      std::string temp = fmt::format("{}={},", i.key.c_str(), i.value.c_str());
      msg.append(temp);
    }
    msg.append("]\n");
    VCL_report(VCL_report_type_t::information, msg.c_str());
  }

  auto it_state = this->block_states.find(buffer.pure_id);
  /*
   if (it_state == this->block_states.end()) {
     it_state = this->block_states.find("block/" + buffer.pure_id);
   }
   */

  if (it_state == this->block_states.end()) {
    if (buffer.pure_id == "air") {
      return block_model::model{};
    }
    std::string msg =
        fmt::format("Undefined reference to block state whose pure block id "
                    "is : \"{}\"  and full block id is : \"{}\"\n",
                    buffer.pure_id, block_state_str);
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return nullptr;
  }

  if (it_state->second.index() == 0) {
    resource_json::model_pass_t model =
        std::get<resource_json::block_states_variant>(it_state->second)
            .block_model_name(buffer.state_list);

    face_exposed = block_model::invrotate_y(face_exposed, model.y);
    face_exposed = block_model::invrotate_x(face_exposed, model.x);

    *face_invrotated = face_exposed;

    if (model.model_name == nullptr) {
      std::string msg =
          fmt::format("No block model for full id : \"{}\", this is usually "
                      "because block states mismatch.",
                      block_state_str);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return nullptr;
    }
    {
      std::string sv(model.model_name);
      const size_t idx_of_colon = sv.find_first_of(':');
      if (idx_of_colon != sv.npos) {
        buffer.pure_id = sv.substr(idx_of_colon + 1);
      } else {
        buffer.pure_id = sv;
      }
    }

    auto it_model = this->block_models.find(buffer.pure_id);
    if (it_model == this->block_models.end()) {
      it_model = this->block_models.find("block/" + buffer.pure_id);
    }

    if (it_model == this->block_models.end()) {
      std::string msg = fmt::format(
          "Failed to find block model for full id : \"{}\". Detail : "
          "undefined reference to model named \"{}\".\n",
          block_state_str.c_str(), model.model_name);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return nullptr;
    }

    return &it_model->second;
  }
  // Here the block must be a multipart
  const auto &multipart =
      std::get<resource_json::block_state_multipart>(it_state->second);

  const auto models = multipart.block_model_names(buffer.state_list);
  for (const auto &md : models) {

    if constexpr (false) {
      std::string msg =
          fmt::format("x_rot = {}, y_rot = {}", int(md.x), int(md.y));
      VCL_report(VCL_report_type_t::information, msg.c_str());
    }

    if (md.model_name == nullptr) {
      std::string msg =
          fmt::format("File = {}, line = {}\n", __FILE__, __LINE__);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return nullptr;
    }
  }

  if (models.size() <= 0) {
    std::string msg = fmt::format("File = {}, line = {}\n", __FILE__, __LINE__);
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return nullptr;
  }

  block_model::model md;

  for (size_t mdidx = 0; mdidx < models.size(); mdidx++) {
    {
      std::string_view sv(models[mdidx].model_name);
      const size_t idx_of_colon = sv.find_first_of(':');
      if (idx_of_colon != sv.npos) {
        buffer.pure_id = sv.substr(idx_of_colon + 1);
      } else {
        buffer.pure_id = sv;
      }
    }

    auto it_model = this->block_models.find(buffer.pure_id);
    if (it_model == this->block_models.end()) {
      it_model = this->block_models.find("block/" + buffer.pure_id);
    }
    if (it_model == this->block_models.end()) {
      std::string msg = fmt::format(
          "Failed to find block model for full id : \"{}\". Detail : "
          "undefined reference to model named \"{}\".\n",
          block_state_str.c_str(), models[mdidx].model_name);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return nullptr;
    }
    if constexpr (false) {
      std::string msg =
          fmt::format("merging back model : {}", models[mdidx].model_name);
      VCL_report(VCL_report_type_t::information, msg.c_str());
    }

    md.merge_back(it_model->second, models[mdidx].x, models[mdidx].y);
  }

  return md;
  // auto jt=this->block_models.find(it->second.)
}

bool VCL_resource_pack::compute_projection(
    const std::string &block_state_str, VCL_face_t face_exposed,
    block_model::EImgRowMajor_t *const img, buffer_t &buffer) const noexcept {

  std::variant<const block_model::model *, block_model::model> ret =
      this->find_model(block_state_str, face_exposed, &face_exposed, buffer);

  if (ret.index() == 0) {
    auto model = std::get<0>(ret);
    if (model == nullptr) {
      std::string msg =
          fmt::format("failed to find a block model for full id :\"{}\", "
                      "function find_model returned nullptr\n",
                      block_state_str.c_str());
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    model->projection_image(face_exposed, img);
    return true;
  }

  block_model::model &md = std::get<1>(ret);

  md.projection_image(face_exposed, img);
  return true;
}

bool VCL_resource_pack::override_texture(
    std::string_view path_in_original, uint32_t standard_color,
    bool replace_transparent_with_black) noexcept {
  if (this->textures_override.contains(path_in_original.data())) {
    return true;
  }

  auto it = this->textures_original.find(path_in_original.data());
  if (it == this->textures_original.end()) {
    std::string msg = fmt::format(
        "Failed to override texture \"{0}\" with given color {1:#x}, the "
        "original texture \"{0}\" doesn't exist.",
        path_in_original, standard_color);
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  const auto &img_original = it->second;

  if (img_original.rows() != 16 || img_original.cols() != 16) {
    std::string msg =
        fmt::format("Failed to override texture \"{0}\" with given "
                    "color {1:#x}, the image size of "
                    "original texture \"{0}\" is {2} * {3} instead of 16 * 16",
                    path_in_original, standard_color, img_original.rows(),
                    img_original.cols());
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  standard_color |= 0xFF'00'00'00;
  float Hs, Ss, Vs;
  RGB2HSV(getR(standard_color) / 255.0f, getG(standard_color) / 255.0f,
          getB(standard_color) / 255.0f, Hs, Ss, Vs);

  Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> img(16,
                                                                          16);
  memset(img.data(), 0xFF, img.size() * sizeof(uint32_t));
  // 0.902473997028232	-0.0371008915304607

  // the ratio to adjust brightness of pixels are determined by the brightness
  // of standard color
  const float k = 0.902473997028232 * Vs - 0.0371008915304607;

  for (int idx = 0; idx < 16 * 16; idx++) {
    const uint32_t ARGB_orignal = img_original(idx);
    if (getA(ARGB_orignal) == 0) {
      if (replace_transparent_with_black) {
        img(idx) = 0xFF'00'00'00;
      } else {
        img(idx) = ARGB_orignal;
      }
    }

    int min_RGB = std::min(std::min(getR(ARGB_orignal), getG(ARGB_orignal)),
                           getB(ARGB_orignal));
    int max_RGB = std::max(std::max(getR(ARGB_orignal), getG(ARGB_orignal)),
                           getB(ARGB_orignal));

    // this pixel is not to be colored.
    if (max_RGB - min_RGB >= 5) {
      img(idx) = ARGB_orignal;
      continue;
    }

    float Vp = max_RGB / 255.0f;

    Vp = std::clamp(Vp * k, 0.0f, 1.0f);

    img(idx) = HSV2ARGB(Hs, Ss, Vp);
  }

  this->textures_override.emplace(path_in_original.data(), std::move(img));
  return true;
}

uint32_t VCL_resource_pack::standard_color(VCL_biome_info info,
                                           bool is_foliage) const noexcept {
  assert(info.downfall == info.downfall);
  assert(info.temperature == info.temperature);
  const float t_adj = std::clamp(info.temperature, 0.0f, 1.0f);
  const float w_adj = std::clamp(info.downfall, 0.0f, 1.0f) * t_adj;

  int x = 255 * (1.0f - t_adj);
  x = std::clamp(x, 0, 255);

  int y = 255 * (1.0f - w_adj);
  y = std::clamp(y, 0, 255 - x);

  const int r = 255 - y;
  const int c = x;

  if (is_foliage) {
    return this->colormap_foliage(r, c);
  } else {
    return this->colormap_grass(r, c);
  }
}

uint32_t VCL_resource_pack::standard_color(
    VCL_biome_t biome, std::string_view texture_name) const noexcept {
  const bool is_foliage = (texture_name.find("leaves") != texture_name.npos) ||
                          (texture_name.find("vine") != texture_name.npos);
  const uint32_t default_result =
      this->standard_color(VCL_get_biome_info(biome), is_foliage);

  assert(default_result != 0);
  assert(getA(default_result) != 0);

  if (texture_name.find("spruce") != texture_name.npos) {
    return 0xFF'61'99'61;
  }

  if (texture_name.find("birch") != texture_name.npos) {
    return 0xFF'80'a7'55;
  }

  if (biome == VCL_biome_t::swamp || biome == VCL_biome_t::mangrove_swamp) {
    if (!is_foliage) {
      return 0xFF'4c'76'3c;
    }

    if (texture_name.find("mangrove") != texture_name.npos) {
      return 0xFF'8d'b1'27;
    }

    return 0xFF'6a'70'39;
  }

  if (biome == VCL_biome_t::dark_forest) {
    if (!is_foliage) {
      return ((default_result & 0xFF'fe'fe'fe) + 0xFF'28'34'0a) / 2;
    }
  }

  if (biome == VCL_biome_t::badlands || biome == VCL_biome_t::eroded_badlands ||
      biome == VCL_biome_t::wooded_badlands) {
    if (is_foliage) {
      return 0xFF'9e'81'4d;
    } else {
      return 0xFF'90'81'4d;
    }
  }

  if (biome == VCL_biome_t::cherry_grove) {
    return 0xFF'90'81'4d;
  }

  return default_result;
}

#include "textures_need_to_override.h"

bool VCL_resource_pack::override_textures(
    VCL_biome_t biome, bool replace_transparent_with_black) noexcept {
  const std::string_view *grass_ids = nullptr;
  size_t grass_num = 0;
  const std::string_view *foliage_ids = nullptr;
  size_t foliage_num = 0;
  if (this->is_MC12) {
    grass_ids = VCL_12_grass_texture_names;
    grass_num = VCL_12_grass_texture_name_size;
    foliage_ids = VCL_12_foliage_texture_names;
    foliage_num = VCL_12_foliage_texture_name_size;
  } else {
    grass_ids = VCL_latest_grass_texture_names;
    grass_num = VCL_latest_grass_texture_name_size;
    foliage_ids = VCL_latest_foliage_texture_names;
    foliage_num = VCL_latest_foliage_texture_name_size;
  }

  for (size_t gid = 0; gid < grass_num; gid++) {
    const bool ok = this->override_texture(
        grass_ids[gid], this->standard_color(biome, grass_ids[gid]),
        replace_transparent_with_black);
    if (!ok) {
      return false;
    }
  }

  for (size_t fid = 0; fid < foliage_num; fid++) {

    const bool ok = this->override_texture(
        foliage_ids[fid], this->standard_color(biome, foliage_ids[fid]),
        replace_transparent_with_black);
    if (!ok) {
      return false;
    }
  }

  return this->update_block_model_textures();
}

bool VCL_resource_pack::update_block_model_textures() noexcept {
  std::unordered_map<const block_model::EImgRowMajor_t *,
                     const block_model::EImgRowMajor_t *>
      updater;
  updater.reserve(this->textures_override.size());

  for (auto &pair : this->textures_override) {
    auto it = this->textures_original.find(pair.first);
    if (it == this->textures_original.end()) {
      std::string msg =
          fmt::format("Internal logical error when invoking function "
                      "\"VCL_resource_pack::update_block_model_textures\" : "
                      "texture \"{}\" is overrided, but failed to find the "
                      "image with same id in this->texture_original.",
                      pair.first);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    updater.emplace(&it->second, &pair.second);
  }
  this->filter_model_textures(updater, false);
  return true;
}