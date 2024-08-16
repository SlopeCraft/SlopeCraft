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
  is_grass = 13,
  is_foliage = 14,
  reproducible = 15,
  rare = 16
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

enum class VCL_biome_t : uint16_t {
  the_void = 0,
  plains = 1,
  sunflower_plains = 2,
  snowy_plains = 3,
  ice_spikes = 4,
  desert = 5,
  swamp = 6,
  mangrove_swamp = 7,
  forest = 8,
  flower_forest = 9,
  birch_forest = 10,
  dark_forest = 11,
  old_growth_birch_forest = 12,
  old_growth_pine_taiga = 13,
  old_growth_spruce_taiga = 14,
  taiga = 15,
  snowy_taiga = 16,
  savanna = 17,
  savanna_plateau = 18,
  windswept_hills = 19,
  windswept_gravelly_hills = 20,
  windswept_forest = 21,
  windswept_savanna = 22,
  jungle = 23,
  sparse_jungle = 24,
  bamboo_jungle = 25,
  badlands = 26,
  eroded_badlands = 27,
  wooded_badlands = 28,
  meadow = 29,
  grove = 30,
  snowy_slopes = 31,
  frozen_peaks = 32,
  jagged_peaks = 33,
  stony_peaks = 34,
  river = 35,
  frozen_river = 36,
  beach = 37,
  snowy_beach = 38,
  stony_shore = 39,
  warm_ocean = 40,
  lukewarm_ocean = 41,
  deep_lukewarm_ocean = 42,
  ocean = 43,
  deep_ocean = 44,
  cold_ocean = 45,
  deep_cold_ocean = 46,
  frozen_ocean = 47,
  deep_frozen_ocean = 48,
  mushroom_fields = 49,
  dripstone_caves = 50,
  lush_caves = 51,
  deep_dark = 52,
  nether_wastes = 53,
  warped_forest = 54,
  crimson_forest = 55,
  soul_sand_valley = 56,
  basalt_deltas = 57,
  the_end = 58,
  end_highlands = 59,
  end_midlands = 60,
  small_end_islands = 61,
  end_barrens = 62,
  cherry_grove = 63
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

struct VCL_string_deliver {
  char *data{nullptr};
  size_t size{0};
  size_t capacity{0};
};

struct VCL_read_only_buffer {
  const void *data{nullptr};
  size_t size{0};
};

class VCL_Kernel;
class VCL_resource_pack;
class VCL_block_state_list;
class VCL_block;
class VCL_model;

class VCL_GPU_Platform;
class VCL_GPU_Device;

class VCL_Kernel {
 public:
  VCL_Kernel() = default;
  virtual ~VCL_Kernel() = default;

  virtual void set_ui(void *uiptr,
                      void (*progressRangeSet)(void *, int, int, int),
                      void (*progressAdd)(void *, int)) noexcept = 0;

  virtual bool have_gpu_resource() const noexcept = 0;

  virtual bool set_gpu_resource(const VCL_GPU_Platform *,
                                const VCL_GPU_Device *) noexcept = 0;

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

  virtual const uint32_t *raw_image(
      int64_t *const rows, int64_t *const cols,
      bool *const is_row_major) const noexcept = 0;

  ////////////////////////////////////////////////////////////////////////
  virtual bool convert(::SCL_convertAlgo algo,
                       bool dither = false) noexcept = 0;
  virtual void converted_image(uint32_t *dest, int64_t *rows, int64_t *cols,
                               bool write_dest_row_major) const noexcept = 0;

  struct flag_diagram_option {
    const uint64_t lib_version{SC_VERSION_U64};

    // [row_start,row_end) * [o,col_count) will be written
    int64_t row_start;
    int64_t row_end;
    int32_t split_line_row_margin;  // 0 or negative number means no split lines
    int32_t split_line_col_margin;  // 0 or negative number means no split lines
    int png_compress_level{9};
    int png_compress_memory_level{8};
  };

  virtual void flag_diagram(uint32_t *image_u8c3_rowmajor,
                            const flag_diagram_option &, int layer_idx,
                            int64_t *rows_required_dest,
                            int64_t *cols_required_dest) const noexcept = 0;
  virtual bool export_flag_diagram(const char *png_filename,
                                   const flag_diagram_option &,
                                   int layer_idx) const noexcept = 0;

  ////////////////////////////////////////////////////////////////////////
  virtual bool build() noexcept = 0;
  virtual int64_t xyz_size(int64_t *x = nullptr, int64_t *y = nullptr,
                           int64_t *z = nullptr) const noexcept = 0;
  ////////////////////////////////////////////////////////////////////////
  virtual bool export_litematic(const char *localEncoding_filename,
                                const char *utf8_litename,
                                const char *utf8_regionname) const noexcept = 0;

  virtual bool export_structure(const char *localEncoding_TargetName,
                                bool is_air_structure_void) const noexcept = 0;

  virtual bool export_WESchem(
      const char *localEncoding_fileName, const int (&offset)[3] = {0, 0, 0},
      const int (&weOffset)[3] = {0, 0, 0}, const char *utf8_Name = "",
      const char *const *const utf8_requiredMods = nullptr,
      const int requiredModsCount = 0) const noexcept = 0;

  struct gpu_options {
    const uint64_t lib_version{SC_VERSION_U64};
    VCL_string_deliver *error_message{nullptr};
  };
  virtual bool set_gpu_resource(const VCL_GPU_Platform *,
                                const VCL_GPU_Device *,
                                const gpu_options &option) noexcept = 0;
  /*
/// export map into Structure files (*.NBT)
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
[[nodiscard]] VCL_EXPORT_FUN VCL_resource_pack *VCL_create_resource_pack(
    const int zip_file_count, const char *const *const zip_file_names);
[[nodiscard]] VCL_EXPORT_FUN VCL_resource_pack *
VCL_create_resource_pack_from_buffers(const size_t zip_count,
                                      const VCL_read_only_buffer *file_contents,
                                      const char *const *const zip_file_names);
VCL_EXPORT_FUN
void VCL_destroy_resource_pack(VCL_resource_pack *const ptr);

// create and destroy block state list
[[nodiscard]] VCL_EXPORT_FUN VCL_block_state_list *VCL_create_block_state_list(
    const int file_count, const char *const *const json_file_names);

VCL_EXPORT_FUN void VCL_destroy_block_state_list(
    VCL_block_state_list *const ptr);

struct VCL_set_resource_option {
  const uint64_t lib_version{SC_VERSION_U64};
  SCL_gameVersion version;
  int32_t max_block_layers;
  VCL_biome_t biome;
  VCL_face_t exposed_face;
  bool is_render_quality_fast;
};

VCL_EXPORT_FUN double VCL_estimate_color_num(
    size_t num_layers, size_t num_foreground, size_t num_background,
    size_t num_nontransparent_non_background);

// set resource for kernel
VCL_EXPORT_FUN bool VCL_set_resource_copy(
    const VCL_resource_pack *const rp, const VCL_block_state_list *const bsl,
    const VCL_set_resource_option &option);

VCL_EXPORT_FUN bool VCL_set_resource_move(
    VCL_resource_pack **rp_ptr, VCL_block_state_list **bsl_ptr,
    const VCL_set_resource_option &option);

VCL_EXPORT_FUN void VCL_discard_resource();

// functions to check the resource
VCL_EXPORT_FUN bool VCL_is_basic_colorset_ok();
VCL_EXPORT_FUN VCL_resource_pack *VCL_get_resource_pack();
VCL_EXPORT_FUN VCL_block_state_list *VCL_get_block_state_list();
VCL_EXPORT_FUN SCL_gameVersion VCL_get_game_version();
VCL_EXPORT_FUN VCL_face_t VCL_get_exposed_face();
VCL_EXPORT_FUN int VCL_get_max_block_layers();
VCL_EXPORT_FUN size_t VCL_num_basic_colors();
/**
  \returns the number of blocks of this color.
*/
VCL_EXPORT_FUN int VCL_get_basic_color_composition(
    size_t color_idx, const VCL_block **const blocks_dest = nullptr,
    uint32_t *const color_dest = nullptr);

// set allowed blocks for kernel
VCL_EXPORT_FUN bool VCL_set_allowed_blocks(
    const VCL_block *const *const blocks_allowed, size_t num_block_allowed);
VCL_EXPORT_FUN void VCL_discard_allowed_blocks();

VCL_EXPORT_FUN bool VCL_is_allowed_colorset_ok();

VCL_EXPORT_FUN int VCL_get_allowed_colors(uint32_t *dest, size_t dest_capacity);

VCL_EXPORT_FUN size_t VCL_get_allowed_color_id(
    uint16_t *const dest, size_t dest_capacity_in_elements);

VCL_EXPORT_FUN bool VCL_export_test_litematic(const char *filename);

// functions about resource pack
VCL_EXPORT_FUN void VCL_display_resource_pack(const VCL_resource_pack *,
                                              bool textures = true,
                                              bool blockstates = true,
                                              bool model = true);

VCL_EXPORT_FUN const uint32_t *VCL_get_colormap(const VCL_resource_pack *,
                                                bool is_foliage,
                                                int *rows = nullptr,
                                                int *cols = nullptr);

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

VCL_EXPORT_FUN const char *VCL_get_block_id(const VCL_block *,
                                            bool ignore_id_replace_list = true);
VCL_EXPORT_FUN const char *VCL_get_block_id_version(const VCL_block *,
                                                    SCL_gameVersion);
VCL_EXPORT_FUN const char *VCL_get_block_name(const VCL_block *, uint8_t is_ZH);

VCL_EXPORT_FUN VCL_block_class_t VCL_get_block_class(const VCL_block *);
VCL_EXPORT_FUN void VCL_set_block_class(VCL_block *, VCL_block_class_t cl);
VCL_EXPORT_FUN bool VCL_is_block_suitable_for_version(const VCL_block *,
                                                      SCL_gameVersion version);
VCL_EXPORT_FUN bool VCL_compare_block(const VCL_block *b1, const VCL_block *b2);

VCL_EXPORT_FUN VCL_block_class_t VCL_string_to_block_class(const char *str,
                                                           bool *ok = nullptr);

[[nodiscard]] VCL_EXPORT_FUN VCL_model *VCL_get_block_model(
    const VCL_block *block, const VCL_resource_pack *resource_pack);

[[nodiscard]] VCL_EXPORT_FUN VCL_model *VCL_get_block_model_by_name(
    const VCL_resource_pack *, const char *name);

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
  if other values, the function will return INT_MIN.
*/
VCL_EXPORT_FUN int VCL_version_component(int i);
VCL_EXPORT_FUN bool VCL_is_version_ok(
    uint64_t version_at_caller_s_build_time = SC_VERSION_U64);

VCL_EXPORT_FUN bool VCL_have_gpu_api();
VCL_EXPORT_FUN const char *VCL_get_GPU_api_name();

[[nodiscard]] VCL_EXPORT_FUN size_t VCL_platform_num();
VCL_EXPORT_FUN VCL_GPU_Platform *VCL_get_platform(size_t platform_idx,
                                                  int *errorcode = nullptr);
VCL_EXPORT_FUN void VCL_release_platform(VCL_GPU_Platform *);
VCL_EXPORT_FUN const char *VCL_get_platform_name(const VCL_GPU_Platform *);

VCL_EXPORT_FUN size_t VCL_get_device_num(const VCL_GPU_Platform *);
[[nodiscard]] VCL_EXPORT_FUN VCL_GPU_Device *VCL_get_device(
    const VCL_GPU_Platform *, size_t device_idx, int *errorcode = nullptr);
VCL_EXPORT_FUN void VCL_release_device(VCL_GPU_Device *);
VCL_EXPORT_FUN const char *VCL_get_device_name(const VCL_GPU_Device *);

struct VCL_biome_info {
  float temperature;
  float downfall;
};

VCL_EXPORT_FUN VCL_biome_info VCL_get_biome_info(VCL_biome_t);
VCL_EXPORT_FUN const char *VCL_biome_name(VCL_biome_t, uint8_t is_ZH);

VCL_EXPORT_FUN uint32_t VCL_locate_colormap(const VCL_resource_pack *,
                                            bool is_grass, VCL_biome_info info,
                                            int *row, int *col);

class VCL_preset;

VCL_EXPORT VCL_preset *VCL_create_preset();
VCL_EXPORT VCL_preset *VCL_load_preset(const char *filename,
                                       VCL_string_deliver *error);
VCL_EXPORT bool VCL_save_preset(const VCL_preset *, const char *filename,
                                VCL_string_deliver *error);
VCL_EXPORT void VCL_destroy_preset(VCL_preset *);
VCL_EXPORT bool VCL_preset_contains_id(const VCL_preset *, const char *id);
VCL_EXPORT void VCL_preset_emplace_id(VCL_preset *, const char *id);
VCL_EXPORT bool VCL_preset_contains_class(const VCL_preset *,
                                          VCL_block_class_t);
VCL_EXPORT void VCL_preset_emplace_class(VCL_preset *, VCL_block_class_t);
VCL_EXPORT size_t VCL_preset_num_ids(const VCL_preset *p);
VCL_EXPORT size_t VCL_preset_get_ids(const VCL_preset *p, const char **id_dest,
                                     size_t capacity);
VCL_EXPORT size_t VCL_preset_num_classes(const VCL_preset *p);
VCL_EXPORT size_t VCL_preset_get_classes(const VCL_preset *p,
                                         VCL_block_class_t *class_dest,
                                         size_t capacity);
VCL_EXPORT void VCL_preset_clear(VCL_preset *);
}

#endif  // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H