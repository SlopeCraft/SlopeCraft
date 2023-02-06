#include "VisualCraftL.h"

#include <stddef.h>

#include "ParseResourcePack.h"
#include "Resource_tree.h"
#include "TokiVC.h"

#include "BlockStateList.h"
#include "VCL_internal.h"
#include <mutex>

#include <sstream>

VCL_EXPORT_FUN VCL_Kernel *VCL_create_kernel() {
  return static_cast<VCL_Kernel *>(new TokiVC);
}
VCL_EXPORT_FUN void VCL_destroy_kernel(VCL_Kernel *const ptr) {
  if (ptr != nullptr) {
    delete dynamic_cast<TokiVC *>(ptr);
  }
}

VCL_EXPORT_FUN VCL_resource_pack *
VCL_create_resource_pack(const int zip_file_count,
                         const char *const *const zip_file_names) {

  if (zip_file_count <= 0) {
    return nullptr;
  }

  bool ok = true;
  zipped_folder zf = zipped_folder::from_zip(zip_file_names[0], &ok);

  if (!ok) {

    std::string msg = fmt::format("Failed to parse {}\n", zip_file_names[0]);

    VCL_report(VCL_report_type_t::error, msg.c_str(), true);
    return nullptr;
  }

  for (int zfidx = 1; zfidx < zip_file_count; zfidx++) {

    zipped_folder z = zipped_folder::from_zip(zip_file_names[zfidx], &ok);

    if (!ok) {
      std::string msg =
          fmt::format("Failed to parse {}\n", zip_file_names[zfidx]);
      VCL_report(VCL_report_type_t::error, msg.c_str(), true);
      return nullptr;
    }

    zf.merge_from_base(std::move(z));
  }

  VCL_resource_pack *const rp = new VCL_resource_pack;

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

  // int success_count = 0;
  /*
  for (int zfidx = 0; zfidx < zip_file_count; zfidx++) {

    zipped_folder zf_2 = zipped_folder::from_zip(zip_file_names[zfidx]);

    if (!rp->add_textures(zf_2, !cover_from_first_to_end)) {
      cout << "Failed to add textures from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }

    if (!rp->add_block_states(zf_2, !cover_from_first_to_end)) {
      cout << "Failed to add block states from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }

    if (!rp->add_block_models(zf_2, !cover_from_first_to_end)) {
      cout << "Warning : Failed to add block models from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }

    success_count++;
  }*/

  /*
    if (success_count < zip_file_count) {
      cout << "Warning : " << zip_file_count - success_count
           << " resource pack(s) not parsed." << endl;
    }

    if (success_count <= 0) {
      delete rp;
      return nullptr;
    }

    */

  return rp;
}

VCL_EXPORT_FUN void VCL_destroy_resource_pack(VCL_resource_pack *const ptr) {
  if (ptr != nullptr) {
    delete ptr;
  }
}

[[nodiscard]] VCL_EXPORT_FUN VCL_block_state_list *
VCL_create_block_state_list(const int file_count,
                            const char *const *const json_file_names) {
  if (file_count <= 0 || json_file_names == nullptr) {
    return nullptr;
  }

  VCL_block_state_list *const bsl = new VCL_block_state_list;

  for (int i = 0; i < file_count; i++) {
    bsl->add(json_file_names[i]);
  }

  return bsl;
}

VCL_EXPORT_FUN void
VCL_destroy_block_state_list(VCL_block_state_list *const ptr) {
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
          if (mpp.criteria.index() == 2) {
            ss << ';';
            continue;
          }
          if (mpp.criteria.index() == 0) {
            ss << " when : ";

            const auto &cr = std::get<0>(mpp.criteria);
            ss << cr.key << " = [";
            for (const auto &val : cr.values) {
              ss << val << ',';
            }
            ss << "];";
            continue;
          }

          // or_list
          ss << "when_or : [";
          for (const auto &cla : std::get<1>(mpp.criteria)) {
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

    std::string msg;
    ss >> msg;

    VCL_report(VCL_report_type_t::information, msg.c_str(), true);
  }

  if (model) {

    std::stringstream ss;
    ss << "There are " << rp->get_models().size() << " models : \n";

    for (const auto &pair : rp->get_models()) {
      ss << pair.first << " : " << pair.second.elements.size() << " elements\n";
    }

    ss << std::endl;
    std::string msg;
    ss >> msg;

    VCL_report(VCL_report_type_t::information, msg.c_str(), true);
  }

  if (textures) {
    std::stringstream ss;
    ss << "There are " << rp->get_textures().size() << " textures : \n";

    for (const auto &pair : rp->get_textures()) {
      ss << pair.first << " : [" << pair.second.rows() << ", "
         << pair.second.cols() << "]\n";
    }

    ss << std::endl;
    std::string msg;
    ss >> msg;

    VCL_report(VCL_report_type_t::information, msg.c_str(), true);
  }
}
VCL_EXPORT_FUN void
VCL_display_block_state_list(const VCL_block_state_list *bsl) {
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

VCL_EXPORT_FUN bool VCL_set_resource_and_version_copy(
    const VCL_resource_pack *const rp, const VCL_block_state_list *const bsl,
    SCL_gameVersion version, VCL_face_t face, int __max_block_layers) {

  if (rp == nullptr || bsl == nullptr) {
    return false;
  }

  if (__max_block_layers <= 0) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  TokiVC_internal::is_basic_color_set_ready = false;
  TokiVC::pack = *rp;
  TokiVC::bsl = *bsl;

  TokiVC::version = version;
  TokiVC::exposed_face = face;
  TokiVC::max_block_layers = __max_block_layers;

  const bool ret = TokiVC::update_color_set_no_lock();

  return ret;
}

VCL_EXPORT_FUN bool VCL_set_resource_and_version_move(
    VCL_resource_pack **rp_ptr, VCL_block_state_list **bsl_ptr,
    SCL_gameVersion version, VCL_face_t face, int __max_block_layers) {
  if (rp_ptr == nullptr || bsl_ptr == nullptr) {
    return false;
  }

  if (*rp_ptr == nullptr || *bsl_ptr == nullptr) {
    return false;
  }
  if (__max_block_layers <= 0) {
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

  TokiVC::version = version;
  TokiVC::exposed_face = face;
  TokiVC::max_block_layers = __max_block_layers;

  if (!TokiVC::update_color_set_no_lock()) {
    ret = false;
  }

  return ret;
}

VCL_EXPORT_FUN int VCL_get_max_block_layers() {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (!TokiVC_internal::is_basic_color_set_ready) {
    return 0;
  }

  return TokiVC::max_block_layers;
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
      (array_of_const_VCL_block != nullptr) && (array_capcity > 0);

  for (const auto &pair : bsl->block_states()) {
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

VCL_EXPORT_FUN bool VCL_is_block_enabled(const VCL_block *b) {
  if (b == nullptr) {
    return false;
  }

  return !b->is_disabled();
}

VCL_EXPORT_FUN void VCL_set_block_enabled(VCL_block *b, bool val) {
  if (b == nullptr)
    return;

  b->set_disabled(!val);
}

VCL_EXPORT_FUN const char *VCL_face_t_to_str(VCL_face_t f) {
  return face_idx_to_string(f);
}

VCL_EXPORT_FUN VCL_face_t VCL_str_to_face_t(const char *str, bool *ok) {
  return string_to_face_idx(str, ok);
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

VCL_EXPORT_FUN const char *VCL_get_block_id(const VCL_block *b) {
  if (b->full_id_ptr() == nullptr) {
    return nullptr;
  }
  return b->full_id_ptr()->c_str();
}

VCL_EXPORT_FUN VCL_block_class_t VCL_get_block_class(const VCL_block *b) {
  return b->block_class;
}
VCL_EXPORT_FUN void VCL_set_block_class(VCL_block *b, VCL_block_class_t cl) {
  b->block_class = cl;
}

VCL_EXPORT_FUN VCL_block_class_t VCL_string_to_block_class(const char *str,
                                                           bool *ok) {
  return string_to_block_class(str, ok);
}

[[nodiscard]] VCL_EXPORT_FUN VCL_model *
VCL_get_block_model(const VCL_block *block,
                    const VCL_resource_pack *resource_pack,
                    VCL_face_t face_exposed, VCL_face_t *face_invrotated) {
  if (block->full_id_ptr() == nullptr) {
    return nullptr;
  }
  VCL_face_t temp_fi;
  auto model_variant =
      resource_pack->find_model(*block->full_id_ptr(), face_exposed, &temp_fi);

  if (model_variant.index() == 0 && std::get<0>(model_variant) == nullptr) {
    return nullptr;
  }

  VCL_model *ret = new VCL_model;
  ret->value = std::move(model_variant);

  if (face_invrotated != nullptr) {
    *face_invrotated = temp_fi;
  }
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
    mdptr = std::get<0>(md->value);
  } else {
    mdptr = &std::get<1>(md->value);
  }

  if (img_buffer_argb32 == nullptr ||
      buffer_capacity_bytes < 16 * 16 * sizeof(uint32_t)) {
    return false;
  }

  auto img = mdptr->projection_image(face);

  Eigen::Map<block_model::EImgRowMajor_t> map(img_buffer_argb32, 16, 16);

  map = img;
  return true;
}

void default_report_callback(VCL_report_type_t type, const char *msg, bool) {
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
  }

  printf("\n%s%s\n", type_msg, msg);
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