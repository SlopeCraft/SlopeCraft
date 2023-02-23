#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H

#ifdef VISUALCRAFTL_BUILD
#include <utilities/SC_GlobalEnums.h>
#else
#include "SC_GlobalEnums.h"
#endif

#include <stddef.h>
#include <stdint.h>

#include "VisualCraftL_global.h"

enum class VCL_Kernel_step : int {
  VCL_wait_for_resource = 0,
  VCL_wait_for_allowed_list = 1,
  VCL_wait_for_image = 2,
  VCL_wait_for_conversion = 3,
  VCL_wait_for_build = 4,
  VCL_built = 5
};

enum class VCL_face_t : uint8_t {
  face_up = 0,
  face_down = 1,
  face_north = 2,
  face_south = 3,
  face_east = 4,
  face_west = 5
};

enum class VCL_block_attribute_t : uint8_t {
  face_up = int(VCL_face_t::face_up),
  face_down = int(VCL_face_t::face_down),
  face_north = int(VCL_face_t::face_north),
  face_south = int(VCL_face_t::face_south),
  face_east = int(VCL_face_t::face_east),
  face_west = int(VCL_face_t::face_west),
  transparency = 6,
  background = 7,
  burnable = 8,
  enderman_pickable = 9,
  is_glowing = 10,
  disabled = 11,
  is_air = 12,
};

enum class VCL_block_class_t : uint8_t {
  wood,
  planks,
  leaves,
  mushroom,
  slab,
  wool,
  concrete,
  terracotta,
  glazed_terracotta,
  concrete_powder,
  shulker_box,
  glass,
  redstone,
  stone,
  ore,
  clay,
  natural,
  crafted,
  desert,
  nether,
  the_end,
  ocean,
  creative_only,
  others
};

/**
 * Tells the direction of upper when VCL_face_t = face_up or face_down.
 *
enum class VCL_upper_direction_t : uint8_t {
  dir_north = 6,
  dir_south = 7,
  dir_east = 8,
  dir_west = 9
};
*/

class VCL_Kernel;
class VCL_resource_pack;
class VCL_block_state_list;
class VCL_block;
class VCL_model;

class VCL_Kernel {
public:
  VCL_Kernel() = default;
  virtual ~VCL_Kernel() = default;

  virtual void set_ui(void *uiptr,
                      void (*progressRangeSet)(void *, int, int, int),
                      void (*progressAdd)(void *, int)) noexcept = 0;

  virtual bool have_gpu_resource() const noexcept = 0;
  virtual bool set_gpu_resource(size_t platform_idx,
                                size_t device_idx) noexcept = 0;

  virtual bool prefer_gpu() const noexcept = 0;
  virtual void set_prefer_gpu(bool try_gpu) noexcept = 0;
  virtual void show_gpu_name() const noexcept = 0;
  virtual size_t get_gpu_name(char *string_buffer,
                              size_t buffer_capacity) const noexcept = 0;

  virtual VCL_Kernel_step step() const noexcept = 0;

  ////////////////////////////////////////////////////////////////////////
  virtual bool set_image(const int64_t rows, const int64_t cols,
                         const uint32_t *const img_argb32,
                         const bool is_row_major) noexcept = 0;

  virtual int64_t rows() const noexcept = 0;
  virtual int64_t cols() const noexcept = 0;
  // virtual int64_t layers() const noexcept = 0;

  virtual const uint32_t *
  raw_image(int64_t *const rows, int64_t *const cols,
            bool *const is_row_major) const noexcept = 0;

  ////////////////////////////////////////////////////////////////////////
  virtual bool convert(::SCL_convertAlgo algo,
                       bool dither = false) noexcept = 0;
  virtual void converted_image(uint32_t *dest, int64_t *rows, int64_t *cols,
                               bool write_dest_row_major) const noexcept = 0;

  ////////////////////////////////////////////////////////////////////////
  virtual bool build() noexcept = 0;
  virtual int64_t xyz_size(int64_t *x = nullptr, int64_t *y = nullptr,
                           int64_t *z = nullptr) const noexcept = 0;
  ////////////////////////////////////////////////////////////////////////
  virtual bool export_litematic(const char *localEncoding_filename,
                                const char *utf8_litename,
                                const char *utf8_regionname) const noexcept = 0;
  /**

  virtual void exportAsLitematic(
      const char *localEncoding_TargetName, const char *utf8_LiteName,
      const char *utf8_RegionName, char *localEncoding_returnVal) const = 0;
  /// export map into Structure files (*.NBT)
  virtual void exportAsStructure(const char *localEncoding_TargetName,
                                 char *localEncoding_FileName) const = 0;
  virtual void exportAsWESchem(
      const char *localEncoding_fileName, const int(&offset)[3] = {0, 0, 0},
      const int(&weOffset)[3] = {0, 0, 0}, const char *utf8_Name = "",
      const char *const *const utf8_requiredMods = nullptr,
      const int requiredModsCount = 0, char *localEncoding_returnVal = nullptr)
      const = 0;
  */
};

extern "C" {
// create and destroy kernel
[[nodiscard]] VCL_EXPORT_FUN VCL_Kernel *VCL_create_kernel();
VCL_EXPORT_FUN void VCL_destroy_kernel(VCL_Kernel *const ptr);

// create and destroy resource pack
[[nodiscard]] VCL_EXPORT_FUN VCL_resource_pack *
VCL_create_resource_pack(const int zip_file_count,
                         const char *const *const zip_file_names);
VCL_EXPORT_FUN void VCL_destroy_resource_pack(VCL_resource_pack *const ptr);

// create and destroy block state list
[[nodiscard]] VCL_EXPORT_FUN VCL_block_state_list *
VCL_create_block_state_list(const int file_count,
                            const char *const *const json_file_names);

VCL_EXPORT_FUN void
VCL_destroy_block_state_list(VCL_block_state_list *const ptr);

struct VCL_set_resource_option {
  const uint64_t lib_version{SC_VERSION_U64};
  SCL_gameVersion version;
  int32_t max_block_layers;
  VCL_face_t exposed_face;
};

// set resource for kernel
VCL_EXPORT_FUN bool
VCL_set_resource_copy(const VCL_resource_pack *const rp,
                      const VCL_block_state_list *const bsl,
                      const VCL_set_resource_option &option);

VCL_EXPORT_FUN bool
VCL_set_resource_move(VCL_resource_pack **rp_ptr,
                      VCL_block_state_list **bsl_ptr,
                      const VCL_set_resource_option &option);

// functions to check the resource
VCL_EXPORT_FUN bool VCL_is_basic_colorset_ok();
VCL_EXPORT_FUN VCL_resource_pack *VCL_get_resource_pack();
VCL_EXPORT_FUN VCL_block_state_list *VCL_get_block_state_list();
VCL_EXPORT_FUN SCL_gameVersion VCL_get_game_version();
VCL_EXPORT_FUN VCL_face_t VCL_get_exposed_face();
VCL_EXPORT_FUN int VCL_get_max_block_layers();

// set allowed blocks for kernel
VCL_EXPORT_FUN bool
VCL_set_allowed_blocks(const VCL_block *const *const blocks_allowed,
                       size_t num_block_allowed);

VCL_EXPORT_FUN bool VCL_is_allowed_colorset_ok();

VCL_EXPORT_FUN int VCL_get_allowed_colors(uint32_t *dest, size_t dest_capacity);

// functions about resource pack
VCL_EXPORT_FUN void VCL_display_resource_pack(const VCL_resource_pack *,
                                              bool textures = true,
                                              bool blockstates = true,
                                              bool model = true);

// functions about block state list
VCL_EXPORT_FUN void VCL_display_block_state_list(const VCL_block_state_list *);

/**
 *\return Number of blocks in this statelist, regardless of array_capacity
 */
VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list(
    VCL_block_state_list *, VCL_block **const array_of_const_VCL_block,
    size_t array_capcity);

/**
 *\return Number of blocks in this statelist that fits the version and face,
 *regardless of array_capacity
 */
VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list_match(
    VCL_block_state_list *, SCL_gameVersion v, VCL_face_t f,
    VCL_block **const array_of_const_VCL_block, size_t array_capcity);

/**
 *\return Number of blocks in this statelist, regardless of array_capacity
 */
VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list_const(
    const VCL_block_state_list *,
    const VCL_block **const array_of_const_VCL_block, size_t array_capcity);

/**
 *\return Number of blocks in this statelist that fits the version and list,
 *regardless of array_capacity
 */
VCL_EXPORT_FUN size_t VCL_get_blocks_from_block_state_list_match_const(
    const VCL_block_state_list *, SCL_gameVersion v, VCL_face_t f,
    const VCL_block **const array_of_const_VCL_block, size_t array_capcity);

// functions about VCL_block
VCL_EXPORT_FUN bool VCL_is_block_enabled(const VCL_block *);
VCL_EXPORT_FUN void VCL_set_block_enabled(VCL_block *, bool val);

VCL_EXPORT_FUN const char *VCL_face_t_to_str(VCL_face_t);
VCL_EXPORT_FUN VCL_face_t VCL_str_to_face_t(const char *str,
                                            bool *ok = nullptr);

VCL_EXPORT_FUN bool VCL_get_block_attribute(const VCL_block *,
                                            VCL_block_attribute_t attribute);
VCL_EXPORT_FUN void VCL_set_block_attribute(VCL_block *,
                                            VCL_block_attribute_t attribute,
                                            bool value);

VCL_EXPORT_FUN const char *VCL_get_block_id(const VCL_block *);

VCL_EXPORT_FUN VCL_block_class_t VCL_get_block_class(const VCL_block *);
VCL_EXPORT_FUN void VCL_set_block_class(VCL_block *, VCL_block_class_t cl);

VCL_EXPORT_FUN VCL_block_class_t VCL_string_to_block_class(const char *str,
                                                           bool *ok = nullptr);

[[nodiscard]] VCL_EXPORT_FUN VCL_model *
VCL_get_block_model(const VCL_block *block,
                    const VCL_resource_pack *resource_pack,
                    VCL_face_t face_exposed, VCL_face_t *face_invrotated);

[[nodiscard]] VCL_EXPORT_FUN VCL_model *
VCL_get_block_model_by_name(const VCL_resource_pack *, const char *name);

VCL_EXPORT_FUN void VCL_destroy_block_model(VCL_model *);

/// \return Whether the image is returned. rows and cols will always be
/// returned.
VCL_EXPORT_FUN bool VCL_compute_projection_image(const VCL_model *,
                                                 VCL_face_t face, int *rows,
                                                 int *cols,
                                                 uint32_t *img_buffer_argb32,
                                                 size_t buffer_capacity_bytes);

VCL_EXPORT_FUN void VCL_display_model(const VCL_model *);

enum class VCL_report_type_t : int { information, warning, error };

using VCL_report_callback_t = void (*)(VCL_report_type_t, const char *,
                                       bool flush);

VCL_EXPORT_FUN VCL_report_callback_t VCL_get_report_callback();
VCL_EXPORT_FUN void VCL_set_report_callback(VCL_report_callback_t);

VCL_EXPORT_FUN const char *VCL_version_string();

/**
i =
  0 -> major version
  1 -> minor version
  2 -> patch version
  3 -> tweak version
  if other values, the function will call abort().
*/
VCL_EXPORT_FUN int VCL_version_component(int i);

class VCL_GPU_Platform;
class VCL_GPU_Device;

[[nodiscard]] VCL_EXPORT_FUN size_t VCL_platform_num();
VCL_EXPORT_FUN VCL_GPU_Platform *VCL_get_platform(size_t platform_idx);
VCL_EXPORT_FUN void VCL_release_platform(VCL_GPU_Platform *);
VCL_EXPORT_FUN const char *VCL_get_platform_name(const VCL_GPU_Platform *);

VCL_EXPORT_FUN size_t VCL_get_device_num(const VCL_GPU_Platform *);
[[nodiscard]] VCL_EXPORT_FUN VCL_GPU_Device *
VCL_get_device(const VCL_GPU_Platform *, size_t device_idx);
VCL_EXPORT_FUN void VCL_release_device(VCL_GPU_Device *);
VCL_EXPORT_FUN const char *VCL_get_device_name(const VCL_GPU_Device *);
}

#endif // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H