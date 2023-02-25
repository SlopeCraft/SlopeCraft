#include "VisualCraftL.h"

#include <stddef.h>

#include <mutex>
#include <sstream>

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

  VCL_report(VCL_report_type_t::warning, nullptr, true);

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
  VCL_report(VCL_report_type_t::warning, nullptr, true);
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
          ss << "when_or : [";
          for (const auto &cla : std::get<1>(mpp.criteria_variant)) {
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

VCL_EXPORT_FUN bool
VCL_set_resource_copy(const VCL_resource_pack *const rp,
                      const VCL_block_state_list *const bsl,
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

  const bool ret = TokiVC::set_resource_no_lock();
  VCL_report(VCL_report_type_t::warning, nullptr, true);

  return ret;
}

VCL_EXPORT_FUN bool
VCL_set_resource_move(VCL_resource_pack **rp_ptr,
                      VCL_block_state_list **bsl_ptr,
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

  if (!TokiVC::set_resource_no_lock()) {
    ret = false;
  }

  VCL_report(VCL_report_type_t::warning, nullptr, true);

  return ret;
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

VCL_EXPORT_FUN bool
VCL_set_allowed_blocks(const VCL_block *const *const blocks_allowed,
                       size_t num_block_allowed) {
  std::unique_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  return TokiVC::set_allowed_no_lock(blocks_allowed, num_block_allowed);
}

VCL_EXPORT_FUN bool VCL_is_allowed_colorset_ok() {

  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (!TokiVC_internal::is_basic_color_set_ready) {
    return false;
  }
  return TokiVC_internal::is_allowed_color_set_ready;
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
  VCL_report(VCL_report_type_t::warning, nullptr, true);
  return ret;
}

[[nodiscard]] VCL_EXPORT_FUN VCL_model *
VCL_get_block_model_by_name(const VCL_resource_pack *rp, const char *name) {
  auto it = rp->get_models().find(name);

  if (it == rp->get_models().end()) {
    return nullptr;
  }

  VCL_model *ret = new VCL_model;
  ret->value = &it->second;
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
    mdp = std::get<0>(md->value);
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

class VCL_GPU_Platform {
public:
  cl::Platform plat;
  std::string name;
  cl_int err;
  std::vector<cl::Device> devices;

  VCL_GPU_Platform() = delete;
  VCL_GPU_Platform(size_t platid) {
    cl_platform_id plats[1024];
    cl_uint num_plats;
    this->err = clGetPlatformIDs(1024, plats, &num_plats);
    if (this->err != CL_SUCCESS) {
      return;
    }
    if (platid >= num_plats) {
      this->err = 1;
      return;
    }

    this->plat = cl::Platform(plats[platid]);

    this->name = this->plat.getInfo<CL_PLATFORM_NAME>(&this->err);
    if (this->err != CL_SUCCESS) {
      return;
    }

    this->err = this->plat.getDevices(CL_DEVICE_TYPE_ALL, &this->devices);
    if (this->err != CL_SUCCESS) {
      return;
    }
  }
};

class VCL_GPU_Device {
public:
  cl::Device device;
  std::string name;
};

VCL_EXPORT_FUN size_t VCL_platform_num() { return ocl_warpper::platform_num(); }
VCL_EXPORT_FUN VCL_GPU_Platform *VCL_get_platform(size_t platform_idx) {
  VCL_GPU_Platform *ret = new VCL_GPU_Platform(platform_idx);
  if (ret->err != CL_SUCCESS) {
    delete ret;
    return nullptr;
  }
  return ret;
}

VCL_EXPORT_FUN void VCL_release_platform(VCL_GPU_Platform *ptr) { delete ptr; }
VCL_EXPORT_FUN const char *VCL_get_platform_name(const VCL_GPU_Platform *ptr) {
  return ptr->name.c_str();
}

VCL_EXPORT_FUN size_t VCL_get_device_num(const VCL_GPU_Platform *platp) {
  return platp->devices.size();
}
VCL_EXPORT_FUN VCL_GPU_Device *VCL_get_device(const VCL_GPU_Platform *platp,
                                              size_t device_idx) {
  if (device_idx >= platp->devices.size()) {
    return nullptr;
  }

  VCL_GPU_Device *ret = new VCL_GPU_Device{platp->devices[device_idx], ""};
  ret->name = ret->device.getInfo<CL_DEVICE_NAME>();
  return ret;
}

VCL_EXPORT_FUN void VCL_release_device(VCL_GPU_Device *dev) { delete dev; }

VCL_EXPORT_FUN const char *VCL_get_device_name(const VCL_GPU_Device *dev) {
  return dev->name.c_str();
}