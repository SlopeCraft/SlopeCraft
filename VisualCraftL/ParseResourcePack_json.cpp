#include <utilities/Schem/bit_shrink.h>

#include <json.hpp>
#include <string>
#include <unordered_map>

#include "ParseResourcePack.h"

using namespace resource_json;

using njson = nlohmann::json;

bool resource_json::match_state_list(const state_list &sla,
                                     const state_list &slb) noexcept {
  if (sla.size() <= 0) return true;
  if (sla.size() != slb.size()) return false;
  int match_num = 0;
  for (const state &sa : sla) {
    for (const state &sb : slb) {
      if ((sa.key == sb.key) && (sa.value == sb.value)) {
        match_num++;
      }
    }
  }

  if (match_num < sla.size()) {
    return false;
  } else {
    return true;
  }
}

bool resource_json::match_criteria_list(const criteria_list_and &cl,
                                        const state_list &sl) noexcept {
  int match_num = 0;
  for (const criteria &c : cl) {
    std::string_view key = c.key;

    const char *value = nullptr;
    for (const state &s : sl) {
      if (s.key == key) {
        value = s.value.data();
        break;
      }
    }
    // if value is not set, it is not considered as match
    if (value == nullptr) {
      break;
    }

    if (c.match(value)) {
      match_num++;
    }
  }

  if (match_num < cl.size()) {
    return false;
  }
  return true;
}

model_pass_t block_states_variant::block_model_name(
    const state_list &sl) const noexcept {
  model_pass_t res;
  res.model_name = nullptr;
  for (const auto &pair : this->LUT) {
    if (match_state_list(pair.first, sl)) {
      res = model_pass_t(pair.second);
      return res;
    }
  }

  return res;
}

std::vector<model_pass_t> block_state_multipart::block_model_names(
    const state_list &sl) const noexcept {
  std::vector<model_pass_t> res;

  for (const multipart_pair &pair : this->pairs) {
    if (pair.match(sl)) res.emplace_back(model_pass_t(pair.apply_blockmodel));
  }

  return res;
}

bool parse_block_state_variant(const njson::object_t &obj,
                               block_states_variant *const dest_variant);

bool parse_block_state_multipart(const njson::object_t &obj,
                                 block_state_multipart *const dest_variant);

bool resource_json::parse_block_state(
    const char *const json_str_beg, const char *const json_str_end,
    block_states_variant *const dest_variant,
    block_state_multipart *const dest_multipart,
    bool *const is_dest_variant) noexcept {
  njson::object_t obj;
  try {
    obj = njson::parse(json_str_beg, json_str_end);
  } catch (...) {
    printf("\nnlohmann json failed to parse json string : ");
    for (const char *p = json_str_beg; p < json_str_end; p++) {
      printf("%c", *p);
    }
    printf("\n");
    return false;
  }

  const bool has_variant =
      obj.contains("variants") && obj.at("variants").is_object();
  const bool has_multipart =
      obj.contains("multipart") && obj.at("multipart").is_array();

  if (has_variant == has_multipart) {
    printf(
        "\nFunction parse_block_state failed to parse json : has_variant = "
        "%i, has_multipart = %i.",
        has_variant, has_multipart);
    return false;
  }

  if (has_variant) {
    if (is_dest_variant != nullptr) *is_dest_variant = true;
    return parse_block_state_variant(obj, dest_variant);
  }

  if (has_multipart) {
    // parsing multipart is not supported yet.
    if (is_dest_variant != nullptr) *is_dest_variant = false;
    return true;
    // return parse_block_state_multipart(obj, dest_multipart);
  }
  // unreachable
  return false;
}

bool parse_block_state_list(std::string_view str,
                            state_list *const sl) noexcept {
  sl->clear();
  if (str.size() <= 1) return true;
  int substr_start = 0;
  // int substr_end = -1;
  int eq_pos = -1;

  for (int cur = 0;; cur++) {
    if (str[cur] == '=') {
      eq_pos = cur;
      continue;
    }

    if (str[cur] == ',' || str[cur] == '\0') {
      // substr_end = cur;
      if (eq_pos <= 0) {
        printf(
            "\n Function parse_block_state_list failed to parse block state "
            "list : %s\n",
            str.data());
        return false;
      }

      state s;

      s.key = str.substr(substr_start, eq_pos - substr_start);
      s.value = str.substr(eq_pos + 1, cur - (eq_pos + 1));

      sl->emplace_back(s);

      substr_start = cur + 1;
      eq_pos = -1;

      // write in an element
    }

    if (str[cur] == '\0') {
      break;
    }
  }

  return true;
}

model_store_t json_to_model(const njson &obj) noexcept {
  model_store_t res;

  res.model_name = obj.at("model");

  if (obj.contains("x") && obj.at("x").is_number()) {
    const int val = obj.at("x");
    res.x = block_model::int_to_face_rot(val);
  }

  if (obj.contains("y") && obj.at("y").is_number()) {
    const int val = obj.at("y");
    res.y = block_model::int_to_face_rot(val);
  }

  if (obj.contains("uvlock") && obj.at("uvlock").is_boolean()) {
    res.uvlock = obj.at("uvlock");
  }

  return res;
}

bool parse_block_state_variant(const njson::object_t &obj,
                               block_states_variant *const dest) {
  const njson &variants = obj.at("variants");

  dest->LUT.clear();
  dest->LUT.reserve(variants.size());

  // printf("size of variants = %i\n", (int)variants.size());

  for (auto pair : variants.items()) {
    if (!pair.value().is_structured()) {
      printf(
          "\nFunction parse_block_state_variant failed to parse json : "
          "value for key \"%s\" is not an object or array.\n",
          pair.key().data());
      return false;
    }

    if (pair.value().is_array() && pair.value().size() <= 0) {
      printf(
          "\nFunction parse_block_state_variant failed to parse json : "
          "value for key \"%s\" is an empty array.\n",
          pair.key().data());
      return false;
    }

    const njson &obj =
        (pair.value().is_object()) ? (pair.value()) : (pair.value().at(0));

    if ((!obj.contains("model")) || (!obj.at("model").is_string())) {
      printf(
          "\nFunction parse_block_state_variant failed to parse json : no "
          "valid value for key \"model\"\n");
      return false;
    }

    std::pair<state_list, model_store_t> p;
    if (!parse_block_state_list(pair.key(), &p.first)) {
      printf("Failed to parse block state list : %s\n", pair.key().data());
      return false;
    }

    p.second = json_to_model(obj);

    dest->LUT.emplace_back(p);
  }

  return true;
}

bool parse_block_state_multipart(const njson::object_t &,
                                 block_state_multipart *const) {
  printf(
      "\nFatal error : parsing blockstate json of multipart is not "
      "supported yet.\n");

  // exit(1);
  return false;
}

struct face_json_temp {
  std::string texture{""};
  std::array<int16_t, 4> uv{0, 0, 16, 16};
  block_model::face_idx cullface_face;
  bool have_cullface{false};
  bool is_hidden{true};  ///< note that by default, is_hidden is true.
};

struct element_json_temp {
  std::array<float, 3> from;
  std::array<float, 3> to;
  std::array<face_json_temp, 6> faces;
};

struct block_model_json_temp {
  std::string parent{""};
  std::map<std::string, std::string> textures;
  std::vector<element_json_temp> elements;
  bool is_inherited{false};
};

block_model::face_idx string_to_face_idx(std::string_view str,
                                         bool *const _ok) noexcept {
  block_model::face_idx res = block_model::face_idx::face_down;
  bool ok = false;
  if (str == "bottom") {
    res = block_model::face_idx::face_down;
    ok = true;
  }
  if (str == "down") {
    res = block_model::face_idx::face_down;
    ok = true;
  }
  if (str == "up") {
    res = block_model::face_idx::face_up;
    ok = true;
  }
  if (str == "north") {
    res = block_model::face_idx::face_north;
    ok = true;
  }
  if (str == "south") {
    res = block_model::face_idx::face_south;
    ok = true;
  }
  if (str == "east") {
    res = block_model::face_idx::face_east;
    ok = true;
  }
  if (str == "west") {
    res = block_model::face_idx::face_west;
    ok = true;
  }

  if (_ok != nullptr) {
    *_ok = ok;
  }
  return res;
}

bool parse_single_model_json(const char *const json_beg,
                             const char *const json_end,
                             block_model_json_temp *const dest) {
  dest->textures.clear();
  dest->elements.clear();
  // disable exceptions, and ignore comments.
  njson obj = njson::parse(json_beg, json_end, nullptr, false, true);
  if (obj.is_null()) {
    // this may be unsafe but just keep it currently.
    printf("\nError : Failed to parse block model json : \n");
    for (const char *p = json_beg; p != json_end; p++) {
      printf("%c", *p);
    }
    printf("\n");
    return false;
  }

  if (obj.contains("parent") && obj.at("parent").is_string()) {
    std::string p_str = obj.at("parent");
    if (p_str.starts_with("minecraft:")) {
      dest->parent = p_str.substr(sizeof("minecraft:") / sizeof(char) - 1);
    } else {
      dest->parent = p_str;
    }
  }

  if (obj.contains("textures") && obj.at("textures").is_object()) {
    const njson &textures = obj.at("textures");
    // dest->textures.reserve(textures.size());
    for (auto temp : textures.items()) {
      if (!temp.value().is_string()) {
        continue;
      }

      auto it = dest->textures.emplace(temp.key(), temp.value());

      if (it.first->second.starts_with("block/")) {
        it.first->second = "minecraft:" + it.first->second;
      }
    }
  }
  // finished textures

  if (obj.contains("elements") && obj.at("elements").is_array()) {
    const njson::array_t &elearr = obj.at("elements");

    dest->elements.reserve(obj.size());
    for (const auto &e : elearr) {
      if (!e.is_object()) {
        return false;
      }

      element_json_temp ele;
      if (!e.contains("from") || !e.at("from").is_array()) {
        printf("Error : \"from\" doesn't exist, or is not an array.\n");
        return false;
      }
      // from
      {
        const njson::array_t &arr_from = e.at("from");
        if (arr_from.size() != 3 || !arr_from.front().is_number()) {
          printf(
              "Error : size of \"from\" is not 3, or is not consisted of "
              "numbers.\n");
          return false;
        }

        for (int idx = 0; idx < 3; idx++) {
          ele.from[idx] = arr_from[idx];
        }
      }
      if (!e.contains("to") || !e.at("to").is_array()) {
        printf("Error : \"to\" doesn't exist, or is not an array.\n");
        return false;
      }
      // to
      {
        const njson::array_t &arr_to = e.at("to");
        if (arr_to.size() != 3 || !arr_to.front().is_number()) {
          printf(
              "Error : size of \"to\" is not 3, or is not consisted of "
              "numbers.\n");
          return false;
        }

        for (int idx = 0; idx < 3; idx++) {
          ele.to[idx] = arr_to[idx];
        }
      }

      // faces
      {
        if (!e.contains("faces") || !e.at("faces").is_object()) {
          printf("Error : \"faces\" doesn't exist, or is not an object.\n");
          return false;
        }

        const njson &faces = e.at("faces");
        for (auto temp : faces.items()) {
          // if the face is not object, skip current face.
          if (!temp.value().is_object()) continue;
          face_json_temp f;
          block_model::face_idx fidx;
          {
            bool _ok;
            fidx = string_to_face_idx(temp.key(), &_ok);
            if (!_ok) {
              printf(
                  "\nError while parsing block model json : invalid key %s "
                  "doesn't refer to any face.\n",
                  temp.key().data());
              return false;
            }
          }

          const njson &curface = temp.value();

          if (!curface.contains("texture") ||
              !curface.at("texture").is_string()) {
            printf(
                "\nError while parsing block model json : face do not have "
                "texture.\n");
            return false;
          }

          f.texture = curface.at("texture");
          if (f.texture.starts_with("block/")) {
            f.texture = ("minecraft:") + f.texture;
          }
          // finished texture

          // cullface
          {
            std::string cullface_temp("");
            if (curface.contains("cullface") &&
                curface.at("cullface").is_string()) {
              cullface_temp = curface.at("cullface");
            }

            if (!cullface_temp.empty()) {
              bool ok;
              const block_model::face_idx cullface_fidx =
                  string_to_face_idx(cullface_temp, &ok);

              if (!ok) {
                printf("\nInvalid value for cullface : %s\n",
                       cullface_temp.data());
                return false;
              }

              f.cullface_face = cullface_fidx;
              f.have_cullface = true;
            }
          }
          // finished cullface

          // uv
          if (curface.contains("uv") && curface.at("uv").is_array()) {
            const njson::array_t &uvarr = curface.at("uv");

            if (uvarr.size() != 4 || !uvarr.front().is_number_integer()) {
              printf(
                  "\nInvalid value for uv array : the size must be 4, and "
                  "each value should be integer.\n");
              return false;
            }

            for (int idx = 0; idx < 4; idx++) {
              f.uv[idx] = uvarr[idx];
            }
          }
          // finished uv

          f.is_hidden = false;
          // finished is_hidden

          // write in this face
          ele.faces[int(fidx)] = f;
        }
      }
      // finished all faces

      dest->elements.emplace_back(ele);
    }
  }

  return true;
}

const char *dereference_texture_name(
    std::map<std::string, std::string>::iterator it,
    std::map<std::string, std::string> &text) noexcept {
  if (it == text.end()) {
    return nullptr;
  }

  if (!it->second.starts_with('#')) {
    return it->second.data();
  }

  // here it->second must be a # reference.

  auto next_it = text.find(it->second.data() + 1);
  const char *const ret = dereference_texture_name(next_it, text);

  if (ret != nullptr) {
    // found a non-reference value
    it->second = ret;
    return ret;
  } else {
    // it->second is the the farest reference and no further link
    return it->second.data();
  }
}

void dereference_texture_name(
    std::map<std::string, std::string> &text) noexcept {
  for (auto it = text.begin(); it != text.end(); ++it) {
    if (!it->second.starts_with('#')) continue;
    dereference_texture_name(it, text);
  }
}

void dereference_model(block_model_json_temp &model) {
  // dereference_texture_name(model.textures);

  for (auto &ele : model.elements) {
    for (auto &face : ele.faces) {
      if (face.is_hidden) continue;
      if (face.texture.starts_with('#')) {
        auto it = model.textures.find(face.texture.data() + 1);

        if (it == model.textures.end()) {
          continue;
        }
        face.texture = it->second;
      }
    }
    // finished current face
  }
  // finished current element
}

bool model_json_inherit_new(block_model_json_temp &child,
                            block_model_json_temp &parent,
                            const bool must_dereference_all) {
  if (child.parent.empty()) {
    printf("Error : child has no parent.\n");
    return false;
  }

  parent.is_inherited = true;

  // child.textures.reserve(child.textures.size() + parent.textures.size());
  // merge textures
  for (const auto &pt : parent.textures) {
    if (!child.textures.contains(pt.first)) {
      child.textures.emplace(pt.first, pt.second);
    }
  }

  dereference_texture_name(child.textures);

  // if child have a parent, and child doesn't define its own element, child
  // inherit parent's elements.
  if (child.elements.size() <= 0) {
    child.elements = parent.elements;
  }

  dereference_model(child);

  // parent
  child.parent = parent.parent;
  return true;
}

bool inherit_recrusively(std::string_view childname,
                         block_model_json_temp &child,
                         std::unordered_map<std::string, block_model_json_temp>
                             &temp_models) noexcept {
  if (child.parent.empty()) return true;

  // #warning This function is not finished yet. I hope to inherit from the
  // root, which measn to find the root and inherit from root to leaf

  // find parent till the root
  auto it = temp_models.find(child.parent);

  if (it == temp_models.end()) {
    printf(
        "\nError : Failed to inherit. Undefined reference to model %s, "
        "required by %s.\n",
        child.parent.data(), childname.data());
    return false;
  }

  if (!it->second.parent.empty()) {
    // find root
    const bool success =
        inherit_recrusively(it->first, it->second, temp_models);
    if (!success) {
      return false;
    }
  }
  /*
  printf("\ninhering : parent : %s, child : %s,\n", child.parent.data(),
         childname.data());
         */
  const bool success = model_json_inherit_new(child, it->second, false);

  // dereference_texture_name(child.textures);

  if (!success) {
    printf("\nError : Failed to inherit. Child : %s, parent : %s.\n",
           childname.data(), child.parent.data());
    return false;
  }

  return true;
}

bool resource_pack::add_block_models(
    const zipped_folder &resourece_pack_root,
    const bool on_conflict_replace_old) noexcept {
  const std::unordered_map<std::string, zipped_file> *files = nullptr;
  // find assets/minecraft/models/block
  {
    const zipped_folder *temp = resourece_pack_root.subfolder("assets");
    if (temp == nullptr) return false;
    temp = temp->subfolder("minecraft");
    if (temp == nullptr) return false;
    temp = temp->subfolder("models");
    if (temp == nullptr) return false;
    temp = temp->subfolder("block");
    if (temp == nullptr) return false;

    files = &temp->files;
  }

  // the name of model is : block/<model-name>
  std::unordered_map<std::string, block_model_json_temp> temp_models;

  temp_models.reserve(files->size());

  std::array<char, 1024> buffer;

  for (const auto &file : *files) {
    if (!file.first.ends_with(".json")) continue;
    buffer.fill('\0');

    std::strcpy(buffer.data(), "block/");
    {
      const int end = file.first.find_last_of('.');
      char *const dest = buffer.data() + std::strlen(buffer.data());
      for (int idx = 0; idx < end; idx++) {
        dest[idx] = file.first[idx];
      }
    }

    block_model_json_temp bmjt;

    const bool ok = parse_single_model_json(
        (const char *)file.second.data(),
        (const char *)file.second.data() + file.second.file_size(), &bmjt);

    if (!ok) {
      printf("\nError : failed to parse assets/minecraft/models/block/%s.\n",
             file.first.data());
      return false;
    }

    temp_models.emplace(buffer.data(), bmjt);
  }
  // parsed all jsons
  /*
  printf("Loaded %i model jsons.\n", int(temp_models.size()));

  for (const auto &file : temp_models) {
    printf("%s, ", file.first.data());
  }
  printf("\n\n");

  */

  // inherit
  for (auto &model : temp_models) {
    const bool ok = inherit_recrusively(model.first, model.second, temp_models);
    if (!ok) {
      printf(
          "\nWarning : failed to inherit model %s. This model will be "
          "skipped.\n",
          model.first.data());
      model.second.parent = "INVALID";
      // #warning following line should be commented.
      // return false;
      continue;
    }

    dereference_texture_name(model.second.textures);
    dereference_model(model.second);
  }
  // remove invalid
  for (auto it = temp_models.begin(); it != temp_models.end();) {
    if (it->second.parent == "INVALID") {
      it = temp_models.erase(it);
      continue;
    }

    ++it;
  }

  // convert temp models to block_models
  this->block_models.reserve(this->block_models.size() + temp_models.size());
  for (auto &tmodel : temp_models) {
    if (this->block_models.contains(tmodel.first) && !on_conflict_replace_old) {
      continue;
    }

    block_model::model md;
    bool skip_this_model = false;

    md.elements.reserve(tmodel.second.elements.size());
    for (auto &tele : tmodel.second.elements) {
      if (skip_this_model) break;
      block_model::element ele;

      // ele._from = tele.from;
      for (int idx = 0; idx < 3; idx++) {
        ele._from[idx] = tele.from[idx];
        ele._to[idx] = tele.to[idx];
      }

      for (uint8_t faceidx = 0; faceidx < 6; faceidx++) {
        if (skip_this_model) break;
        auto &tface = tele.faces[faceidx];
        ele.faces[faceidx].is_hidden = tface.is_hidden;
        if (tface.is_hidden) {
          continue;
        }
        ele.faces[faceidx].uv_start[0] = tface.uv[0];
        ele.faces[faceidx].uv_start[1] = tface.uv[1];
        ele.faces[faceidx].uv_end[0] = tface.uv[2];
        ele.faces[faceidx].uv_end[1] = tface.uv[2];

        auto it = this->textures.find(tface.texture);

        if (it == this->textures.end()) {
          if (tface.texture.starts_with('#') && tmodel.second.is_inherited) {
            skip_this_model = true;
            continue;
          }

          printf(
              "\nError : Undefined reference to texture %s, required by "
              "model %s but no such image.\n",
              tface.texture.data(), tmodel.first.data());
          printf("The texture is : \n");
          for (const auto &pair : tmodel.second.textures) {
            printf("{%s, %s}\n", pair.first.data(), pair.second.data());
          }
          return false;

          // if managed to find, go on
        }
        ele.faces[faceidx].texture = &it->second;
      }
      // finished all faces

      md.elements.emplace_back(ele);
    }
    // finished current model
    this->block_models.emplace(tmodel.first, md);
  }

  return true;
}

bool resource_pack::add_block_states(
    const zipped_folder &resourece_pack_root,
    const bool on_conflict_replace_old) noexcept {
  const std::unordered_map<std::string, zipped_file> *files = nullptr;
  {
    const zipped_folder *temp = resourece_pack_root.subfolder("assets");
    if (temp == nullptr) {
      return false;
    }
    temp = temp->subfolder("minecraft");
    if (temp == nullptr) {
      return false;
    }
    temp = temp->subfolder("blockstates");
    if (temp == nullptr) {
      return false;
    }
    files = &temp->files;
  }

  this->block_states.reserve(this->block_states.size() + files->size());

  for (const auto &file : *files) {
    if (this->block_states.contains(file.first) && !on_conflict_replace_old) {
      continue;
    }

    block_states_variant bsv;
    bool is_dest_variant;

    const bool success = parse_block_state(
        (const char *)file.second.data(),
        (const char *)file.second.data() + file.second.file_size(), &bsv,
        nullptr, &is_dest_variant);

    if (!success) {
      printf(
          "\nWarning : Failed to parse block state json file "
          "assets/minecraft/blockstates/%s. This will be "
          "skipped but may cause further warnings.\n",
          file.first.data());
      continue;
    }

    if (!is_dest_variant) {
      continue;
    }

    const int substrlen = file.first.find_last_of('.');
    this->block_states.emplace(file.first.substr(0, substrlen), bsv);
  }

  return true;
}