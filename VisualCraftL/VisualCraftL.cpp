/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "VisualCraftL.h"

#include <stddef.h>

#include <mutex>
#include <sstream>
#include <span>

#include "BlockStateList.h"
#include "ParseResourcePack.h"
#include "Resource_tree.h"
#include "TokiVC.h"
#include "VCL_internal.h"

VCL_EXPORT_FUN VCL_Kernel *VCL_create_kernel() {
  return static_cast<VCL_Kernel *>(new TokiVC);
}

VCL_EXPORT_FUN void VCL_destroy_kernel(VCL_Kernel *const ptr) {
  if (ptr != nullptr) {
    delete dynamic_cast<TokiVC *>(ptr);
  }
}

VCL_resource_pack *zip_folder_to_resource_pack(
    const zipped_folder &zf) noexcept {
  VCL_resource_pack *const rp = new VCL_resource_pack;

  if (!rp->add_colormaps(zf)) {
    delete rp;
    VCL_report(VCL_report_type_t::error,
               "Failed to add colormaps from resource pack.", true);
    return nullptr;
  }

  if (!rp->add_textures(zf)) {
    delete rp;
    VCL_report(VCL_report_type_t::error,
               "Failed to add textures from resource pack.", true);
    return nullptr;
  }

  if (!rp->add_block_states(zf)) {
    delete rp;
    VCL_report(VCL_report_type_t::error,
               "Failed to add block states from resource pack.", true);
    return nullptr;
  }

  if (!rp->add_block_models(zf)) {
    /*
    cout << "All textures are listed here : " << endl;
    for (const auto &pair : rp->get_textures()) {
      cout << pair.first << " : [" << pair.second.rows() << ", "
           << pair.second.cols() << "]\n";
    }
    cout << endl;
    */

    delete rp;
    VCL_report(VCL_report_type_t::error,
               "Failed to add block states from resource pack.", true);
    return nullptr;
  }

  VCL_report(VCL_report_type_t::warning, nullptr, true);

  return rp;
}

VCL_EXPORT_FUN VCL_resource_pack *VCL_create_resource_pack(
    const int zip_file_count, const char *const *const zip_file_names) {
  if (zip_file_count <= 0) {
    return nullptr;
  }
  auto parse_zip = [](const char *filename) noexcept {
    std::optional<zipped_folder> zf_opt = zipped_folder::from_zip(filename);

    if (not zf_opt) {
      std::string msg = fmt::format("Failed to parse {}\n", filename);
      VCL_report(VCL_report_type_t::error, msg.c_str(), true);
    }
    return zf_opt;
  };

  zipped_folder zf;
  {
    std::optional<zipped_folder> zf_opt = parse_zip(zip_file_names[0]);

    if (not zf_opt) {
      return nullptr;
    }
    zf = std::move(zf_opt.value());
  }

  for (int zfidx = 1; zfidx < zip_file_count; zfidx++) {
    std::optional<zipped_folder> zf_opt = parse_zip(zip_file_names[0]);
    if (not zf_opt) {
      return nullptr;
    }

    zf.merge_from_base(std::move(zf_opt.value()));
  }

  return zip_folder_to_resource_pack(zf);
}

[[nodiscard]] VCL_EXPORT_FUN VCL_resource_pack *
VCL_create_resource_pack_from_buffers(const size_t zip_count,
                                      const VCL_read_only_buffer *file_contents,
                                      const char *const *const zip_file_names) {
  if (zip_count <= 0) {
    return nullptr;
  }
  zipped_folder zf;
  {
    auto zf_opt = zipped_folder::from_zip(
        zip_file_names[0],
        {reinterpret_cast<const uint8_t *>(file_contents[0].data),
         file_contents[0].size});
    if (not zf_opt) {
      VCL_report(VCL_report_type_t::error,
                 fmt::format("Failed to parse {}\n", zip_file_names[0]).c_str(),
                 true);
      return nullptr;
    }
    zf = std::move(zf_opt.value());
  }
  for (size_t idx = 1; idx < zip_count; idx++) {
    auto zf_opt = zipped_folder::from_zip(
        zip_file_names[idx],
        {reinterpret_cast<const uint8_t *>(file_contents[idx].data),
         file_contents[idx].size});
    if (not zf_opt) {
      VCL_report(
          VCL_report_type_t::error,
          fmt::format("Failed to parse {}\n", zip_file_names[idx]).c_str(),
          true);
      return nullptr;
    }
    zf.merge_from_base(std::move(zf_opt.value()));
  }

  return zip_folder_to_resource_pack(zf);
}

VCL_EXPORT_FUN void VCL_destroy_resource_pack(VCL_resource_pack *const ptr) {
  if (ptr != nullptr) {
    delete ptr;
  }
}

[[nodiscard]] VCL_EXPORT_FUN VCL_block_state_list *VCL_create_block_state_list(
    const int file_count, const char *const *const json_file_names) {
  if (file_count <= 0 || json_file_names == nullptr) {
    return nullptr;
  }

  VCL_block_state_list *const bsl = new VCL_block_state_list;

  for (int i = 0; i < file_count; i++) {
    bsl->add(json_file_names[i]);
  }
  VCL_report(VCL_report_type_t::warning, nullptr, true);
  return bsl;
}

VCL_EXPORT_FUN void VCL_destroy_block_state_list(
    VCL_block_state_list *const ptr) {
  delete ptr;
}

VCL_EXPORT_FUN void VCL_display_resource_pack(const VCL_resource_pack *rp,
                                              bool textures, bool blockstates,
                                              bool model) {
  if (rp == nullptr) {
    return;
  }
  if (blockstates) {
    std::stringstream ss;
    ss << "There are " << rp->get_block_states().size()
       << " block state files : \n";

    for (const auto &pair : rp->get_block_states()) {
      ss << pair.first << " : {";
      if (pair.second.index() == 1) {
        ss << "\n  multipart : [";

        for (const auto &mpp : std::get<1>(pair.second).pairs) {
          ss << "apply :[";
          for (const auto &md : mpp.apply_blockmodel) {
            ss << md.model_name << ',';
          }
          ss << ']';
          if (mpp.criteria_variant.index() == 2) {
            ss << ';';
            continue;
          }
          if (mpp.criteria_variant.index() == 0) {
            ss << " when : ";

            const auto &cr = std::get<0>(mpp.criteria_variant);
            ss << cr.key << " = [";
            for (const auto &val : cr.values) {
              ss << val << ',';
            }
            ss << "];";
            continue;
          }

          // or_list
          ss << "when_"
             << (std::get<1>(mpp.criteria_variant).is_or ? "or" : "and")
             << " : [";
          for (const auto &cla : std::get<1>(mpp.criteria_variant).components) {
            ss << '{';
            for (const auto &cr : cla) {
              ss << cr.key << " = [";
              for (const auto &val : cr.values) {
                ss << val << ',';
              }
              ss << ']';
            }
            ss << '}';
          }
          ss << "];";
        }

      } else
        for (const auto &i : std::get<0>(pair.second).LUT) {
          ss << "\n  [";
          for (const auto &j : i.first) {
            ss << j.key << '=' << j.value << ',';
          }

          if (!i.first.empty()) {
            ss << '\b';
          }

          ss << "] => " << i.second.model_name;
          ss << ", x=" << int(i.second.x) * 10;
          ss << ", y=" << int(i.second.y) * 10;
          ss << ", uvlock="
             << (const char *)(i.second.uvlock ? ("true") : ("false"));
        }
      ss << "}\n";
    }

    std::string msg(ss.str());

    VCL_report(VCL_report_type_t::information, msg.c_str(), true);
  }

  if (model) {
    std::stringstream ss;
    ss << "There are " << rp->get_models().size() << " models : \n";

    for (const auto &pair : rp->get_models()) {
      ss << pair.first << " : " << pair.second.elements.size() << " elements\n";
    }

    ss << std::endl;
    std::string msg(ss.str());

    VCL_report(VCL_report_type_t::information, msg.c_str(), true);
  }

  if (textures) {
    std::stringstream ss;
    ss << "There are " << rp->get_textures_original().size()
       << "original textures : \n";

    for (const auto &pair : rp->get_textures_original()) {
      ss << pair.first << " : [" << pair.second.rows() << ", "
         << pair.second.cols() << "]\n";
    }

    ss << "There are " << rp->get_textures_override().size()
       << " overrided textures : \n";

    for (const auto &pair : rp->get_textures_override()) {
      ss << pair.first << " : [" << pair.second.rows() << ", "
         << pair.second.cols() << "]\n";
    }

    ss << std::endl;
    std::string msg(ss.str());

    VCL_report(VCL_report_type_t::information, msg.c_str(), true);
  }
}

VCL_EXPORT_FUN const uint32_t *VCL_get_colormap(const VCL_resource_pack *rp,
                                                bool is_foliage, int *rows,
                                                int *cols) {
  if (rows != nullptr) {
    *rows = 256;
  }
  if (cols != nullptr) {
    *cols = 256;
  }

  return rp->get_colormap(is_foliage).data();
}

VCL_EXPORT_FUN
void VCL_display_block_state_list(const VCL_block_state_list *bsl) {
  if (bsl == nullptr) {
    return;
  }

  std::stringstream ss;
  ss << "Block state contains " << bsl->block_states().size() << " blocks : \n";

  for (const auto &pair : bsl->block_states()) {
    ss << pair.first << " : ";
    ss << "nameZH = \"" << pair.second.name_ZH << "\", nameEN = \""
       << pair.second.name_EN;
    ss << "\", transparent = "
       << (const char *)(pair.second.is_transparent() ? "true" : "false");
    ss << ", supported versions = [";

    for (SCL_gameVersion v = SCL_gameVersion::MC12; v <= max_version;
         v = SCL_gameVersion(int(v) + 1)) {
      if (pair.second.version_info.match(v)) {
        ss << int(v) << ',';
      }
    }

    ss << "]\n";
  }

  ss << std::endl;
  std::string msg;
  ss >> msg;

  VCL_report(VCL_report_type_t::information, msg.c_str(), true);
}

VCL_EXPORT_FUN double VCL_estimate_color_num(
    size_t num_layers, size_t num_foreground, size_t num_background,
    size_t num_nontransparent_non_background) {
  double a = num_background;
  double b = num_foreground;
  double num_stacked = 0;

  switch (num_foreground) {
    case 0:
      num_stacked = a;
      break;
    case 1:
      num_stacked = a + a * b;
      break;
    case 2:
      num_stacked = a + (num_layers + 1) * b * a;
      break;
    default:
      num_stacked = a + a * b * (std::pow(b - 1, num_layers + 1) - 1) / (b - 2);
      break;
  }

  return num_stacked + num_nontransparent_non_background;
}

VCL_EXPORT_FUN bool VCL_set_resource_copy(
    const VCL_resource_pack *const rp, const VCL_block_state_list *const bsl,
    const VCL_set_resource_option &option) {
  if (rp == nullptr || bsl == nullptr) {
    return false;
  }

  if (option.max_block_layers <= 0) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  TokiVC_internal::is_basic_color_set_ready = false;
  TokiVC::pack = *rp;
  TokiVC::bsl = *bsl;

  TokiVC::version = option.version;
  TokiVC::exposed_face = option.exposed_face;
  TokiVC::max_block_layers = option.max_block_layers;
  TokiVC::biome = option.biome;
  TokiVC::is_render_quality_fast = option.is_render_quality_fast;

  const bool ret = TokiVC::set_resource_no_lock();
  VCL_report(VCL_report_type_t::warning, nullptr, true);

  return ret;
}

VCL_EXPORT_FUN bool VCL_set_resource_move(
    VCL_resource_pack **rp_ptr, VCL_block_state_list **bsl_ptr,
    const VCL_set_resource_option &option) {
  if (rp_ptr == nullptr || bsl_ptr == nullptr) {
    return false;
  }

  if (*rp_ptr == nullptr || *bsl_ptr == nullptr) {
    return false;
  }
  if (option.max_block_layers <= 0) {
    return false;
  }

  bool ret = true;
  std::unique_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  TokiVC_internal::is_basic_color_set_ready = false;

  TokiVC::pack = std::move(**rp_ptr);
  VCL_destroy_resource_pack(*rp_ptr);
  *rp_ptr = nullptr;

  TokiVC::bsl = std::move(**bsl_ptr);
  VCL_destroy_block_state_list(*bsl_ptr);
  *bsl_ptr = nullptr;

  TokiVC::version = option.version;
  TokiVC::exposed_face = option.exposed_face;
  TokiVC::max_block_layers = option.max_block_layers;
  TokiVC::biome = option.biome;
  TokiVC::is_render_quality_fast = option.is_render_quality_fast;

  if (!TokiVC::set_resource_no_lock()) {
    ret = false;
  }

  VCL_report(VCL_report_type_t::warning, nullptr, true);

  return ret;
}

VCL_EXPORT_FUN void VCL_discard_resource() {
  std::unique_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  TokiVC_internal::is_basic_color_set_ready = false;
  TokiVC_internal::is_allowed_color_set_ready = false;
}

VCL_EXPORT_FUN int VCL_get_max_block_layers() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (!TokiVC_internal::is_basic_color_set_ready) {
    return 0;
  }

  return TokiVC::max_block_layers;
}

VCL_EXPORT_FUN bool VCL_is_basic_colorset_ok() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  return TokiVC_internal::is_basic_color_set_ready;
}

VCL_EXPORT_FUN VCL_resource_pack *VCL_get_resource_pack() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (!TokiVC_internal::is_basic_color_set_ready) {
    return nullptr;
  }

  return &TokiVC::pack;
}

VCL_EXPORT_FUN VCL_block_state_list *VCL_get_block_state_list() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (!TokiVC_internal::is_basic_color_set_ready) {
    return nullptr;
  }
  return &TokiVC::bsl;
}

VCL_EXPORT_FUN SCL_gameVersion VCL_get_game_version() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (!TokiVC_internal::is_basic_color_set_ready) {
    return SCL_gameVersion::ANCIENT;
  }
  return TokiVC::version;
}

VCL_EXPORT_FUN VCL_face_t VCL_get_exposed_face() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (!TokiVC_internal::is_basic_color_set_ready) {
    return {};
  }

  return TokiVC::exposed_face;
}

VCL_EXPORT_FUN size_t VCL_num_basic_colors() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  return TokiVC::LUT_bcitb().size();
}

VCL_EXPORT_FUN int VCL_get_basic_color_composition(
    size_t color_idx, const VCL_block **const blocks_dest,
    uint32_t *const color) {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (!TokiVC_internal::is_basic_color_set_ready) {
    return -1;
  }

  if (color_idx >= TokiVC::LUT_bcitb().size()) {
    return false;
  }

  if (color != nullptr) {
    const uint16_t color_id = TokiVC::colorset_basic.color_id(color_idx);
    *color = ARGB32(TokiVC::colorset_basic.RGB(color_id, 0) * 255,
                    TokiVC::colorset_basic.RGB(color_id, 1) * 255,
                    TokiVC::colorset_basic.RGB(color_id, 2) * 255);
  }

  const auto &variant = TokiVC::LUT_bcitb()[color_idx];
  const VCL_block *const *srcp = nullptr;
  size_t num_blocks = 0;
  if (variant.index() == 0) {
    srcp = &std::get<0>(variant);
    num_blocks = 1;
  } else {
    const auto &vec = std::get<1>(variant);
    srcp = vec.data();
    num_blocks = vec.size();
  }

  if (blocks_dest != nullptr) {
    for (size_t i = 0; i < num_blocks; i++) {
      blocks_dest[i] = srcp[i];
    }
  }

  return num_blocks;
}

VCL_EXPORT_FUN bool VCL_set_allowed_blocks(
    const VCL_block *const *const blocks_allowed, size_t num_block_allowed) {
  std::unique_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  return TokiVC::set_allowed_no_lock({blocks_allowed, num_block_allowed});
}
VCL_EXPORT_FUN void VCL_discard_allowed_blocks() {
  std::unique_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  TokiVC_internal::is_allowed_color_set_ready = false;
}

VCL_EXPORT_FUN bool VCL_is_allowed_colorset_ok() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (!TokiVC_internal::is_basic_color_set_ready) {
    return false;
  }
  return TokiVC_internal::is_allowed_color_set_ready;
}

VCL_EXPORT_FUN bool VCL_export_test_litematic(const char *filename) {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  return TokiVC::export_test_litematic_no_lock(filename);
}

VCL_EXPORT_FUN int VCL_get_allowed_colors(uint32_t *dest,
                                          size_t dest_capacity) {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  size_t num_written = 0;

  if (dest == nullptr || dest_capacity <= 0) {
    return TokiVC::colorset_allowed.color_count();
  }

  for (int idx = 0; idx < TokiVC::colorset_allowed.color_count(); idx++) {
    if (num_written >= dest_capacity) {
      break;
    }
    Eigen::Array3i ret = (TokiVC::colorset_allowed.rgb(idx) * 255).cast<int>();
    dest[num_written] = ARGB32(ret[0], ret[1], ret[2]);
    num_written++;
  }

  return TokiVC::colorset_allowed.color_count();
}

VCL_EXPORT_FUN size_t VCL_get_allowed_color_id(
    uint16_t *const dest, size_t dest_capacity_in_elements) {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (!TokiVC_internal::is_basic_color_set_ready ||
      !TokiVC_internal::is_allowed_color_set_ready) {
    return 0;
  }

  if (dest != nullptr) {
    for (size_t cidx = 0;
         cidx < std::min<size_t>(dest_capacity_in_elements,
                                 TokiVC::colorset_allowed.color_count());
         cidx++) {
      dest[cidx] = TokiVC::colorset_allowed.color_id(cidx);
    }
  }

  return TokiVC::colorset_allowed.color_count();
}

VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list(
    VCL_block_state_list *bsl, VCL_block **const const_VCL_ptr_arr,
    size_t arr_capcity) {
  if (bsl == nullptr) {
    return 0;
  }

  if (const_VCL_ptr_arr == nullptr || arr_capcity <= 0) {
    return bsl->block_states().size();
  }

  size_t widx = 0;

  for (auto &pair : bsl->block_states()) {
    if (widx >= arr_capcity) {
      break;
    }

    const_VCL_ptr_arr[widx] = &pair.second;
    widx++;
  }

  return bsl->block_states().size();
}

VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list_match(
    VCL_block_state_list *bsl, SCL_gameVersion version, VCL_face_t f,
    VCL_block **const array_of_const_VCL_block, size_t array_capcity) {
  if (bsl == nullptr) {
    return 0;
  }

  if (version == SCL_gameVersion::ANCIENT ||
      version == SCL_gameVersion::FUTURE) {
    // invalid version
    return 0;
  }

  size_t available_block_counter = 0;
  size_t write_counter = 0;

  const bool can_write =
      (array_of_const_VCL_block != nullptr) && (array_capcity > 0);

  for (auto &pair : bsl->block_states()) {
    if (pair.second.match(version, f)) {
      available_block_counter++;

      if (can_write && (write_counter < array_capcity)) {
        array_of_const_VCL_block[write_counter] = &pair.second;
        write_counter++;
      }
    }
  }

  return available_block_counter;
}

VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list_const(
    const VCL_block_state_list *bsl, const VCL_block **const const_VCL_ptr_arr,
    size_t arr_capcity) {
  if (bsl == nullptr) {
    return 0;
  }

  if (const_VCL_ptr_arr == nullptr || arr_capcity <= 0) {
    return bsl->block_states().size();
  }

  size_t widx = 0;

  for (const auto &pair : bsl->block_states()) {
    if (widx >= arr_capcity) {
      break;
    }

    const_VCL_ptr_arr[widx] = &pair.second;
    widx++;
  }

  return bsl->block_states().size();
}

VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list_match_const(
    const VCL_block_state_list *bsl, SCL_gameVersion version, VCL_face_t f,
    const VCL_block **const array_of_const_VCL_block, size_t array_capcity) {
  if (bsl == nullptr) {
    return 0;
  }

  if (version == SCL_gameVersion::ANCIENT ||
      version == SCL_gameVersion::FUTURE) {
    // invalid version
    return 0;
  }

  size_t available_block_counter = 0;
  size_t write_counter = 0;

  const bool can_write =
      (array_of_const_VCL_block not_eq nullptr) and (array_capcity > 0);

  for (const auto &pair : bsl->block_states()) {
    if (pair.second.match(version, f)) {
      available_block_counter++;

      if (can_write and (write_counter < array_capcity)) {
        array_of_const_VCL_block[write_counter] = &pair.second;
        write_counter++;
      }
    }
  }
  // no possible output
  return available_block_counter;
}

VCL_EXPORT_FUN bool VCL_is_block_enabled(const VCL_block *b) {
  if (b == nullptr) {
    return false;
  }

  return !b->is_disabled();
}

VCL_EXPORT_FUN void VCL_set_block_enabled(VCL_block *b, bool val) {
  if (b == nullptr) return;

  b->set_disabled(!val);
}

VCL_EXPORT_FUN const char *VCL_face_t_to_str(VCL_face_t f) {
  return face_idx_to_string(f);
}

VCL_EXPORT_FUN VCL_face_t VCL_str_to_face_t(const char *str, bool *ok) {
  auto res = string_to_face_idx(str);
  if (ok not_eq nullptr) {
    *ok = res.has_value();
  }
  return res.value_or(VCL_face_t::face_up);
}

VCL_EXPORT_FUN bool VCL_get_block_attribute(const VCL_block *b,
                                            VCL_block_attribute_t attribute) {
  return b->get_attribute(attribute);
}

VCL_EXPORT_FUN void VCL_set_block_attribute(VCL_block *b,
                                            VCL_block_attribute_t attribute,
                                            bool value) {
  b->set_attribute(attribute, value);
}

VCL_EXPORT_FUN const char *VCL_get_block_id(const VCL_block *b,
                                            bool ignore_id_replace_list) {
  if (b->full_id_ptr() == nullptr) {
    return nullptr;
  }

  if (ignore_id_replace_list) {
    return b->full_id_ptr()->c_str();
  } else {
    return b->id_for_schem(VCL_get_game_version()).c_str();
  }
}

VCL_EXPORT_FUN const char *VCL_get_block_id_version(const VCL_block *b,
                                                    SCL_gameVersion v) {
  return b->id_for_schem(v).c_str();
}

VCL_EXPORT_FUN const char *VCL_get_block_name(const VCL_block *b,
                                              uint8_t is_ZH) {
  if (is_ZH) {
    return b->name_ZH.c_str();
  }
  return b->name_EN.c_str();
}

VCL_EXPORT_FUN VCL_block_class_t VCL_get_block_class(const VCL_block *b) {
  return b->block_class;
}
VCL_EXPORT_FUN void VCL_set_block_class(VCL_block *b, VCL_block_class_t cl) {
  b->block_class = cl;
}

VCL_EXPORT_FUN bool VCL_is_block_suitable_for_version(const VCL_block *blk,
                                                      SCL_gameVersion version) {
  return blk->version_info.match(version);
}

VCL_EXPORT_FUN bool VCL_compare_block(const VCL_block *b1,
                                      const VCL_block *b2) {
  // if one of these blocks contains null, compare by index
  if ((b1 == nullptr) || (b2 == nullptr)) {
    return b1 < b2;
  }

  // compare by class
  if (b1->block_class != b2->block_class) {
    return b1->block_class < b2->block_class;
  }

  // compare by introduced version. Earlier versions are greater
  {
    const auto v1 = b1->version_info.introduced_version();
    const auto v2 = b2->version_info.introduced_version();

    if (v1 != v2) {
      return v1 < v2;
    }
  }

  return std::less<std::string_view>()(*b1->full_id_ptr(), *b2->full_id_ptr());
}

VCL_EXPORT_FUN VCL_block_class_t VCL_string_to_block_class(const char *str,
                                                           bool *ok) {
  return string_to_block_class(str, ok);
}

[[nodiscard]] VCL_EXPORT_FUN VCL_model *VCL_get_block_model(
    const VCL_block *block, const VCL_resource_pack *resource_pack) {
  if (block->full_id_ptr() == nullptr) {
    return nullptr;
  }

  auto model_variant = resource_pack->find_model(*block->full_id_ptr());

  if (model_variant.index() == 0 &&
      std::get<0>(model_variant).model_ptr == nullptr) {
    return nullptr;
  }

  VCL_model *ret = new VCL_model;
  ret->value = std::move(model_variant);

  VCL_report(VCL_report_type_t::warning, nullptr, true);
  return ret;
}

[[nodiscard]] VCL_EXPORT_FUN VCL_model *VCL_get_block_model_by_name(
    const VCL_resource_pack *rp, const char *name) {
  auto result = rp->find_model(name);

  if (result.index() == 0) {
    if (std::get<0>(result).model_ptr == nullptr) {
      return nullptr;
    }
  }

  VCL_model *ret = new VCL_model;
  ret->value = result;
  return ret;
}

VCL_EXPORT_FUN void VCL_destroy_block_model(VCL_model *md) { delete md; }

VCL_EXPORT_FUN bool VCL_compute_projection_image(const VCL_model *md,
                                                 VCL_face_t face, int *rows,
                                                 int *cols,
                                                 uint32_t *img_buffer_argb32,
                                                 size_t buffer_capacity_bytes) {
  if (rows != nullptr) {
    *rows = 16;
  }
  if (cols != nullptr) {
    *cols = 16;
  }

  const block_model::model *mdptr = nullptr;

  if (md->value.index() == 0) {
    mdptr = std::get<0>(md->value).model_ptr;
    face = block_model::invrotate(face, std::get<0>(md->value).x_rot,
                                  std::get<0>(md->value).y_rot);
  } else {
    mdptr = &std::get<1>(md->value);
  }

  if (img_buffer_argb32 == nullptr ||
      buffer_capacity_bytes < 16 * 16 * sizeof(uint32_t)) {
    return false;
  }

  [[maybe_unused]] auto img = mdptr->projection_image(face);

  Eigen::Map<block_model::EImgRowMajor_t> map(img_buffer_argb32, 16, 16);

  map = img;

  VCL_report(VCL_report_type_t::warning, nullptr, true);
  return true;
}

VCL_EXPORT_FUN void VCL_display_model(const VCL_model *md) {
  if (md == nullptr) {
    VCL_report(VCL_report_type_t::information, "nullptr", true);
    return;
  }

  std::string msg;
  msg.reserve(1024);

  const block_model::model *mdp = nullptr;

  if (md->value.index() == 0) {
    msg.append("Is variant.\n");
    mdp = std::get<0>(md->value).model_ptr;
  } else {
    msg.append("Is multipart.\n");
    mdp = &std::get<1>(md->value);
  }

  msg.append("elements :[\n");
  for (const auto &ele : mdp->elements) {
    msg.append("  {\n");
    msg.append(fmt::format("    from : [{}, {}, {}]\n", ele._from[0],
                           ele._from[1], ele._from[2]));
    msg.append(fmt::format("    to : [{}, {}, {}]\n", ele._to[0], ele._to[1],
                           ele._to[2]));
    msg.append("  faces : [\n");
    for (const block_model::face_t &face : ele.faces) {
      msg.append("    {");
      msg.append(fmt::format(
          "uv_start=[{},{}],uv_end=[{},{}],rot={},is_hidden={},texture={}",
          face.uv_start[0], face.uv_start[1], face.uv_end[0], face.uv_end[1],
          int(face.rot) * 10, face.is_hidden, (const void *)face.texture));
      msg.append("}\n");
    }
    msg.append("  ]\n");
  }
  msg.append("]\n");

  VCL_report(VCL_report_type_t::information, msg.c_str(), true);
}

void default_report_callback(VCL_report_type_t type, const char *msg, bool) {
  if (msg == nullptr) {
    // flush here.

    return;
  }
  const char *type_msg = nullptr;

  switch (type) {
    case VCL_report_type_t::information:
      type_msg = "Information : ";
      break;
    case VCL_report_type_t::warning:
      type_msg = "Warning : ";
      break;
    case VCL_report_type_t::error:
      type_msg = "Error : ";
      break;
  }

  printf("\n%s%s\n", type_msg, msg);
}

VCL_EXPORT_FUN bool VCL_is_version_ok(uint64_t version_at_caller_s_build_time) {
  const uint64_t version_at_lib_build_time = SC_VERSION_U64;

  return version_at_caller_s_build_time <= version_at_lib_build_time;
}

VCL_report_callback_t VCL_report_fun = default_report_callback;

VCL_EXPORT_FUN VCL_report_callback_t VCL_get_report_callback() {
  return VCL_report_fun;
}

VCL_EXPORT_FUN void VCL_set_report_callback(VCL_report_callback_t cb) {
  VCL_report_fun = cb;
}

void VCL_report(VCL_report_type_t t, const char *msg, bool flush) noexcept {
  VCL_report_fun(t, msg, flush);
}

VCL_EXPORT_FUN bool VCL_have_gpu_api() { return ::gpu_wrapper::have_api; }

VCL_EXPORT_FUN const char *VCL_get_GPU_api_name() {
  return ::gpu_wrapper::api_name();
}

VCL_EXPORT_FUN size_t VCL_platform_num() { return gpu_wrapper::platform_num(); }

VCL_EXPORT_FUN VCL_GPU_Platform *VCL_get_platform(size_t platform_idx,
                                                  int *errorcode) {
  VCL_GPU_Platform *ret = new VCL_GPU_Platform;
  ret->pw = gpu_wrapper::platform_wrapper::create(platform_idx, errorcode);
  return ret;
}

VCL_EXPORT_FUN void VCL_release_platform(VCL_GPU_Platform *ptr) { delete ptr; }
VCL_EXPORT_FUN const char *VCL_get_platform_name(const VCL_GPU_Platform *ptr) {
  return ptr->pw->name_v();
}

VCL_EXPORT_FUN size_t VCL_get_device_num(const VCL_GPU_Platform *platp) {
  return platp->pw->num_devices_v();
}
VCL_EXPORT_FUN VCL_GPU_Device *VCL_get_device(const VCL_GPU_Platform *platp,
                                              size_t device_idx,
                                              int *errorcode) {
  VCL_GPU_Device *ret = new VCL_GPU_Device;
  ret->dw =
      gpu_wrapper::device_wrapper::create(platp->pw, device_idx, errorcode);

  return ret;
}

VCL_EXPORT_FUN void VCL_release_device(VCL_GPU_Device *dev) { delete dev; }

VCL_EXPORT_FUN const char *VCL_get_device_name(const VCL_GPU_Device *dev) {
  return dev->dw->name_v();
}

static_assert(std::is_trivially_copyable_v<VCL_biome_t> &&
              std::is_standard_layout_v<VCL_biome_t>);

VCL_EXPORT_FUN VCL_biome_info VCL_get_biome_info(VCL_biome_t biome) {
  switch (biome) {
    case VCL_biome_t::the_void:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::plains:
      return VCL_biome_info{0.8, 0.4};
    case VCL_biome_t::sunflower_plains:
      return VCL_biome_info{0.8, 0.4};
    case VCL_biome_t::snowy_plains:
      return VCL_biome_info{0, 0.5};
    case VCL_biome_t::ice_spikes:
      return VCL_biome_info{0, 0.5};
    case VCL_biome_t::desert:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::swamp:
      return VCL_biome_info{0.8, 0.9};
    case VCL_biome_t::mangrove_swamp:
      return VCL_biome_info{0.8, 0.9};
    case VCL_biome_t::forest:
      return VCL_biome_info{0.7, 0.4};
    case VCL_biome_t::flower_forest:
      return VCL_biome_info{0.7, 0.8};
    case VCL_biome_t::birch_forest:
      return VCL_biome_info{0.6, 0.6};
    case VCL_biome_t::dark_forest:
      return VCL_biome_info{0.7, 0.8};
    case VCL_biome_t::old_growth_birch_forest:
      return VCL_biome_info{0.6, 0.6};
    case VCL_biome_t::old_growth_pine_taiga:
      return VCL_biome_info{0.3, 0.8};
    case VCL_biome_t::old_growth_spruce_taiga:
      return VCL_biome_info{0.25, 0.8};
    case VCL_biome_t::taiga:
      return VCL_biome_info{0.25, 0.8};
    case VCL_biome_t::snowy_taiga:
      return VCL_biome_info{-0.5, 0.4};
    case VCL_biome_t::savanna:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::savanna_plateau:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::windswept_hills:
      return VCL_biome_info{0.2, 0.3};
    case VCL_biome_t::windswept_gravelly_hills:
      return VCL_biome_info{0.2, 0.3};
    case VCL_biome_t::windswept_forest:
      return VCL_biome_info{0.2, 0.3};
    case VCL_biome_t::windswept_savanna:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::jungle:
      return VCL_biome_info{0.95, 0.9};
    case VCL_biome_t::sparse_jungle:
      return VCL_biome_info{0.95, 0.8};
    case VCL_biome_t::bamboo_jungle:
      return VCL_biome_info{0.95, 0.9};
    case VCL_biome_t::badlands:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::eroded_badlands:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::wooded_badlands:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::meadow:
      return VCL_biome_info{0.5, 0.8};
    case VCL_biome_t::grove:
      return VCL_biome_info{-0.2, 0.8};
    case VCL_biome_t::snowy_slopes:
      return VCL_biome_info{-0.3, 0.9};
    case VCL_biome_t::frozen_peaks:
      return VCL_biome_info{-0.7, 0.9};
    case VCL_biome_t::jagged_peaks:
      return VCL_biome_info{-0.7, 0.9};
    case VCL_biome_t::stony_peaks:
      return VCL_biome_info{1, 0.3};
    case VCL_biome_t::river:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::frozen_river:
      return VCL_biome_info{0, 0.5};
    case VCL_biome_t::beach:
      return VCL_biome_info{0.8, 0.4};
    case VCL_biome_t::snowy_beach:
      return VCL_biome_info{0.05, 0.3};
    case VCL_biome_t::stony_shore:
      return VCL_biome_info{0.2, 0.3};
    case VCL_biome_t::warm_ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::lukewarm_ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::deep_lukewarm_ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::deep_ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::cold_ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::deep_cold_ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::frozen_ocean:
      return VCL_biome_info{0, 0.5};
    case VCL_biome_t::deep_frozen_ocean:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::mushroom_fields:
      return VCL_biome_info{0.9, 1};
    case VCL_biome_t::dripstone_caves:
      return VCL_biome_info{0.8, 0.4};
    case VCL_biome_t::lush_caves:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::deep_dark:
      return VCL_biome_info{0.8, 0.4};
    case VCL_biome_t::nether_wastes:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::warped_forest:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::crimson_forest:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::soul_sand_valley:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::basalt_deltas:
      return VCL_biome_info{2, 0};
    case VCL_biome_t::the_end:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::end_highlands:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::end_midlands:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::small_end_islands:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::end_barrens:
      return VCL_biome_info{0.5, 0.5};
    case VCL_biome_t::cherry_grove:
      return VCL_biome_info{0.5, 0.8};
  }
  return VCL_biome_info{NAN, NAN};
}

#include <magic_enum.hpp>

const char *VCL_biome_name_ZH(VCL_biome_t b) noexcept {
  switch (b) {
    case VCL_biome_t::the_void:
      return "虚空";
    case VCL_biome_t::plains:
      return "平原";
    case VCL_biome_t::sunflower_plains:
      return "向日葵平原";
    case VCL_biome_t::snowy_plains:
      return "雪原";
    case VCL_biome_t::ice_spikes:
      return "冰刺之地";
    case VCL_biome_t::desert:
      return "沙漠";
    case VCL_biome_t::swamp:
      return "湿地";
    case VCL_biome_t::mangrove_swamp:
      return "红树林湿地";
    case VCL_biome_t::forest:
      return "森林";
    case VCL_biome_t::flower_forest:
      return "繁华森林";
    case VCL_biome_t::birch_forest:
      return "白桦森林";
    case VCL_biome_t::dark_forest:
      return "黑森林";
    case VCL_biome_t::old_growth_birch_forest:
      return "原始桦木森林";
    case VCL_biome_t::old_growth_pine_taiga:
      return "原始松木针叶林";
    case VCL_biome_t::old_growth_spruce_taiga:
      return "原始云杉针叶林";
    case VCL_biome_t::taiga:
      return "针叶林";
    case VCL_biome_t::snowy_taiga:
      return "积雪针叶林";
    case VCL_biome_t::savanna:
      return "热带草原";
    case VCL_biome_t::savanna_plateau:
      return "热带高原";
    case VCL_biome_t::windswept_hills:
      return "风袭丘陵";
    case VCL_biome_t::windswept_gravelly_hills:
      return "风袭沙砾丘陵";
    case VCL_biome_t::windswept_forest:
      return "风袭森林";
    case VCL_biome_t::windswept_savanna:
      return "风袭热带草原";
    case VCL_biome_t::jungle:
      return "丛林";
    case VCL_biome_t::sparse_jungle:
      return "稀疏丛林";
    case VCL_biome_t::bamboo_jungle:
      return "竹林";
    case VCL_biome_t::badlands:
      return "恶地 (粘土山)";
    case VCL_biome_t::eroded_badlands:
      return "风蚀恶地";
    case VCL_biome_t::wooded_badlands:
      return "疏林恶地";
    case VCL_biome_t::meadow:
      return "草甸";
    case VCL_biome_t::grove:
      return "雪林";
    case VCL_biome_t::snowy_slopes:
      return "积雪山坡";
    case VCL_biome_t::frozen_peaks:
      return "冰封山峰";
    case VCL_biome_t::jagged_peaks:
      return "尖峭山峰";
    case VCL_biome_t::stony_peaks:
      return "裸岩山峰";
    case VCL_biome_t::river:
      return "河流";
    case VCL_biome_t::frozen_river:
      return "冻河";
    case VCL_biome_t::beach:
      return "沙滩";
    case VCL_biome_t::snowy_beach:
      return "积雪沙滩";
    case VCL_biome_t::stony_shore:
      return "石岸";
    case VCL_biome_t::warm_ocean:
      return "暖水海洋";
    case VCL_biome_t::lukewarm_ocean:
      return "温水海洋";
    case VCL_biome_t::deep_lukewarm_ocean:
      return "温水深海";
    case VCL_biome_t::ocean:
      return "海洋";
    case VCL_biome_t::deep_ocean:
      return "深海";
    case VCL_biome_t::cold_ocean:
      return "冷水海洋";
    case VCL_biome_t::deep_cold_ocean:
      return "冷水深海";
    case VCL_biome_t::frozen_ocean:
      return "冻洋";
    case VCL_biome_t::deep_frozen_ocean:
      return "冰冻深海";
    case VCL_biome_t::mushroom_fields:
      return "蘑菇岛";
    case VCL_biome_t::dripstone_caves:
      return "溶洞";
    case VCL_biome_t::lush_caves:
      return "繁茂洞穴";
    case VCL_biome_t::deep_dark:
      return "Deep ♂ Dark ♂";
    case VCL_biome_t::nether_wastes:
      return "下界荒地";
    case VCL_biome_t::warped_forest:
      return "扭曲森林";
    case VCL_biome_t::crimson_forest:
      return "绯红森林";
    case VCL_biome_t::soul_sand_valley:
      return "灵魂沙峡谷";
    case VCL_biome_t::basalt_deltas:
      return "玄武岩三角洲";
    case VCL_biome_t::the_end:
      return "末地";
    case VCL_biome_t::end_highlands:
      return "末地高原";
    case VCL_biome_t::end_midlands:
      return "末地内陆";
    case VCL_biome_t::small_end_islands:
      return "末地小型岛屿";
    case VCL_biome_t::end_barrens:
      return "末地荒地";
    case VCL_biome_t::cherry_grove:
      return "樱花树林";
  }
  return "Unamed";
}

const char *VCL_biome_name_EN(VCL_biome_t b) noexcept {
  switch (b) {
    case VCL_biome_t::the_void:
      return "The Void";
    case VCL_biome_t::plains:
      return "Plains";
    case VCL_biome_t::sunflower_plains:
      return "Sunflower Plains";
    case VCL_biome_t::snowy_plains:
      return "Snowy Plains";
    case VCL_biome_t::ice_spikes:
      return "Ice Spikes";
    case VCL_biome_t::desert:
      return "Desert";
    case VCL_biome_t::swamp:
      return "Swamp";
    case VCL_biome_t::mangrove_swamp:
      return "Mangrove Swamp";
    case VCL_biome_t::forest:
      return "Forest";
    case VCL_biome_t::flower_forest:
      return "Flower Forest";
    case VCL_biome_t::birch_forest:
      return "Birch Forest";
    case VCL_biome_t::dark_forest:
      return "Dark Forest";
    case VCL_biome_t::old_growth_birch_forest:
      return "Old Growth Birch Forest";
    case VCL_biome_t::old_growth_pine_taiga:
      return "Old Growth Pine Taiga";
    case VCL_biome_t::old_growth_spruce_taiga:
      return "Old Growth Spruce Taiga";
    case VCL_biome_t::taiga:
      return "Taiga";
    case VCL_biome_t::snowy_taiga:
      return "Snowy Taiga";
    case VCL_biome_t::savanna:
      return "Savanna";
    case VCL_biome_t::savanna_plateau:
      return "Savanna Plateau";
    case VCL_biome_t::windswept_hills:
      return "Windswept Hills";
    case VCL_biome_t::windswept_gravelly_hills:
      return "Windswept Gravelly Hills";
    case VCL_biome_t::windswept_forest:
      return "Windswept Forest";
    case VCL_biome_t::windswept_savanna:
      return "Windswept Savanna";
    case VCL_biome_t::jungle:
      return "Jungle";
    case VCL_biome_t::sparse_jungle:
      return "Sparse Jungle";
    case VCL_biome_t::bamboo_jungle:
      return "Bamboo Jungle";
    case VCL_biome_t::badlands:
      return "Badlands";
    case VCL_biome_t::eroded_badlands:
      return "Eroded Badlands";
    case VCL_biome_t::wooded_badlands:
      return "Wooded Badlands";
    case VCL_biome_t::meadow:
      return "Meadow";
    case VCL_biome_t::grove:
      return "Grove";
    case VCL_biome_t::snowy_slopes:
      return "Snowy Slopes";
    case VCL_biome_t::frozen_peaks:
      return "Frozen Peaks";
    case VCL_biome_t::jagged_peaks:
      return "Jagged Peaks";
    case VCL_biome_t::stony_peaks:
      return "Stony Peaks";
    case VCL_biome_t::river:
      return "River";
    case VCL_biome_t::frozen_river:
      return "Frozen River";
    case VCL_biome_t::beach:
      return "Beach";
    case VCL_biome_t::snowy_beach:
      return "Snowy Beach";
    case VCL_biome_t::stony_shore:
      return "Stony Shore";
    case VCL_biome_t::warm_ocean:
      return "Warm Ocean";
    case VCL_biome_t::lukewarm_ocean:
      return "Lukewarm Ocean";
    case VCL_biome_t::deep_lukewarm_ocean:
      return "Deep Lukewarm Ocean";
    case VCL_biome_t::ocean:
      return "Ocean";
    case VCL_biome_t::deep_ocean:
      return "Deep Ocean";
    case VCL_biome_t::cold_ocean:
      return "Cold Ocean";
    case VCL_biome_t::deep_cold_ocean:
      return "Deep Cold Ocean";
    case VCL_biome_t::frozen_ocean:
      return "Frozen Ocean";
    case VCL_biome_t::deep_frozen_ocean:
      return "Deep Frozen Ocean";
    case VCL_biome_t::mushroom_fields:
      return "Mushroom Fields";
    case VCL_biome_t::dripstone_caves:
      return "Dripstone Caves";
    case VCL_biome_t::lush_caves:
      return "Lush Caves";
    case VCL_biome_t::deep_dark:
      return "Deep Dark";
    case VCL_biome_t::nether_wastes:
      return "Nether Wastes";
    case VCL_biome_t::warped_forest:
      return "Warped Forest";
    case VCL_biome_t::crimson_forest:
      return "Crimson Forest";
    case VCL_biome_t::soul_sand_valley:
      return "Soul Sand Valley";
    case VCL_biome_t::basalt_deltas:
      return "Basalt Deltas";
    case VCL_biome_t::the_end:
      return "The End";
    case VCL_biome_t::end_highlands:
      return "End Highlands";
    case VCL_biome_t::end_midlands:
      return "End Midlands";
    case VCL_biome_t::small_end_islands:
      return "Small End Islands";
    case VCL_biome_t::end_barrens:
      return "End Barrens";
    case VCL_biome_t::cherry_grove:
      return "Cherry Grove";
  }

  return "Unamed";
}

VCL_EXPORT_FUN const char *VCL_biome_name(VCL_biome_t biome, uint8_t is_ZH) {
  return (is_ZH) ? VCL_biome_name_ZH(biome) : VCL_biome_name_EN(biome);
}
VCL_EXPORT_FUN uint32_t VCL_locate_colormap(const VCL_resource_pack *rp,
                                            bool is_grass, VCL_biome_info info,
                                            int *row, int *col) {
  const auto rc = rp->locate_color_rc(info);

  if (row != nullptr) {
    *row = rc[0];
  }
  if (col != nullptr) {
    *col = rc[1];
  }

  return rp->standard_color(info, !is_grass);
}

class VCL_preset {
 public:
  std::unordered_set<std::string> ids;
  std::unordered_set<VCL_block_class_t> classes;
};

void write_to_string_deliver(std::string_view sv,
                             VCL_string_deliver *strp) noexcept {
  if (strp == nullptr) {
    return;
  }
  if (strp->data == nullptr || strp->capacity <= 0) {
    return;
  }

  const size_t written_bytes = std::min(sv.size(), strp->capacity - 1);
  memcpy(strp->data, sv.data(), written_bytes);
  strp->size = written_bytes;
  strp->data[strp->capacity - 1] = '\0';
}

#include <json.hpp>
#include <fstream>
using njson = nlohmann::json;

VCL_EXPORT VCL_preset *VCL_create_preset() { return new VCL_preset{}; }
VCL_EXPORT VCL_preset *VCL_load_preset(const char *filename,
                                       VCL_string_deliver *error) {
  write_to_string_deliver("", error);
  njson block_ids;
  njson block_classes;
  try {
    std::ifstream ifs{filename};
    auto nj = njson::parse(ifs, nullptr, true, true);
    block_ids = std::move(nj.at("block_ids"));
    block_classes = std::move(nj.at("block_classes"));
  } catch (std::exception &e) {
    auto err = fmt::format("Exception occurred when parsing {}, detail: {}",
                           filename, e.what());
    write_to_string_deliver(err, error);
    return nullptr;
  }

  VCL_preset *p = new VCL_preset{};
  p->ids.reserve(block_ids.size());

  for (auto &pair : block_ids.items()) {
    p->ids.emplace(std::move(pair.key()));
  }
  for (auto &pair : block_classes.items()) {
    auto cls = magic_enum::enum_cast<VCL_block_class_t>(pair.key());
    if (!cls.has_value()) {
      auto err = fmt::format(
          "Invalid block class \"{}\" can not be converted to "
          "VCL_block_class_t",
          pair.key());
      write_to_string_deliver(err, error);
      delete p;
      return nullptr;
    }
    p->classes.emplace(cls.value());
  }
  return p;
}

VCL_EXPORT bool VCL_save_preset(const VCL_preset *p, const char *filename,
                                VCL_string_deliver *error) {
  write_to_string_deliver("", error);
  njson nj;
  {
    njson classes, ids;
    for (const auto &id : p->ids) {
      ids.emplace(id, njson::object_t{});
    }
    for (const auto cls : p->classes) {
      classes.emplace(magic_enum::enum_name(cls), njson::object_t{});
    }
    nj.emplace("block_ids", std::move(ids));
    nj.emplace("block_classes", std::move(classes));
  }

  std::ofstream ofs{filename};
  if (!ofs) {
    write_to_string_deliver(fmt::format("Failed to open/create {}", filename),
                            error);
    return false;
  }
  ofs << nj.dump(2);
  ofs.close();
  return true;
}

VCL_EXPORT void VCL_destroy_preset(VCL_preset *p) { delete p; }

VCL_EXPORT bool VCL_preset_contains_id(const VCL_preset *p, const char *id) {
  return p->ids.contains(id);
}

VCL_EXPORT void VCL_preset_emplace_id(VCL_preset *p, const char *id) {
  p->ids.emplace(id);
}

VCL_EXPORT bool VCL_preset_contains_class(const VCL_preset *p,
                                          VCL_block_class_t cls) {
  return p->classes.contains(cls);
}

VCL_EXPORT void VCL_preset_emplace_class(VCL_preset *p, VCL_block_class_t cls) {
  p->classes.emplace(cls);
}

VCL_EXPORT void VCL_preset_clear(VCL_preset *p) {
  p->ids.clear();
  p->classes.clear();
}

VCL_EXPORT size_t VCL_preset_num_ids(const VCL_preset *p) {
  return p->ids.size();
}

VCL_EXPORT size_t VCL_preset_get_ids(const VCL_preset *p, const char **id_dest,
                                     size_t capacity) {
  size_t written_num{0};

  for (const auto &pair : p->ids) {
    if (capacity <= written_num) {
      break;
    }

    id_dest[written_num] = pair.c_str();
    written_num++;
  }
  return written_num;
}

VCL_EXPORT size_t VCL_preset_num_classes(const VCL_preset *p) {
  return p->classes.size();
}

VCL_EXPORT size_t VCL_preset_get_classes(const VCL_preset *p,
                                         VCL_block_class_t *class_dest,
                                         size_t capacity) {
  size_t written_num{0};
  for (const auto cls : p->classes) {
    if (capacity <= written_num) {
      break;
    }

    class_dest[written_num] = cls;
    written_num++;
  }
  return written_num;
}