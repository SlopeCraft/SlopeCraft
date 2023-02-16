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

class VCL_Kernel;
class VCL_resource_pack;
class VCL_block_state_list;
class VCL_block;
class VCL_model;

class VCL_Kernel {
public:
  VCL_Kernel() = default;
  virtual ~VCL_Kernel() = default;

  virtual VCL_Kernel_step step() const noexcept = 0;

  virtual bool set_image(const int64_t rows, const int64_t cols,
                         const uint32_t *const img_argb32,
                         const bool is_row_major) noexcept = 0;

  virtual int64_t rows() const noexcept = 0;
  virtual int64_t cols() const noexcept = 0;

  virtual const uint32_t *
  raw_image(int64_t *const rows, int64_t *const cols,
            bool *const is_row_major) const noexcept = 0;
};

extern "C" {
// create and destroy kernel
[[nodiscard]] VCL_Kernel *VCL_EXPORT_FUN VCL_create_kernel();
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
  SCL_gameVersion version;
  VCL_face_t exposed_face;
  int32_t max_block_layers;
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
}

#endif // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H