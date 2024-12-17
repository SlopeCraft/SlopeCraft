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

#ifndef KERNEL_H
#define KERNEL_H
#include <cstdint>
#include <cstddef>
#include <cstring>

#include "SlopeCraftL_export.h"

#ifdef SLOPECRAFTL_NOT_INSTALLED
#include <SC_version_buildtime.h>
#else
#include "SC_version_buildtime.h"
#endif  // SLOPECRAFTL_NOT_INSTALLED

// define enumerations first
#ifdef SCL_FIND_GlobalEnums_BY_PATH
#include "../utilities/SC_GlobalEnums.h"
#else
#include "SC_GlobalEnums.h"
#endif

enum class SCL_item_frame_variant : uint16_t {
  common = 0,
  glowing = 1,
};

namespace SlopeCraft {

// using step = ::SCL_step;
using mapTypes = ::SCL_mapTypes;
using compressSettings = ::SCL_compressSettings;
using convertAlgo = ::SCL_convertAlgo;
using glassBridgeSettings = ::SCL_glassBridgeSettings;
using gameVersion = ::SCL_gameVersion;
using workStatus = ::SCL_workStatus;
using errorFlag = ::SCL_errorFlag;

struct GA_converter_option {
  uint64_t caller_api_version{SC_VERSION_U64};
  size_t popSize{50};
  size_t maxGeneration{200};
  size_t maxFailTimes{50};
  double crossoverProb{0.8};
  double mutationProb{0.01};
};

/// struct to deliver string via ABI
struct string_deliver {
  string_deliver() = default;
  string_deliver(char *p, size_t cap) : data(p), capacity(cap) {}
  char *const data{nullptr};
  const size_t capacity{0};
  size_t size{0};
  bool is_complete{true};

  constexpr bool is_valid() const noexcept {
    return this->data != nullptr && this->capacity > 0;
  }

  template <class string_t>
  [[nodiscard]] static string_deliver from_string(string_t &s) noexcept {
    return string_deliver{s.data(), s.size()};
  }
};
/// Struct to wrap anything like ostream, safe to use in ABI
struct ostream_wrapper {
  void *handle{nullptr};

  using write_fun = void (*)(const void *data, size_t len_bytes, void *handle);
  write_fun callback_write_data{nullptr};

  inline void write(const void *data, size_t len_bytes) {
    this->callback_write_data(data, len_bytes, this->handle);
  }
  inline void write(const char *str) {
    this->write(reinterpret_cast<const void *>(str), strlen(str));
  }

  template <class ostream_t>
  [[nodiscard]] inline static ostream_wrapper wrap_std_ostream(
      ostream_t &os) noexcept {
    return ostream_wrapper{
        .handle = reinterpret_cast<void *>(&os),
        .callback_write_data = [](const void *data, size_t len_bytes,
                                  void *h) -> size_t {
          ostream_t &os = *reinterpret_cast<ostream_t *>(h);
          os.write(reinterpret_cast<const char *>(data), len_bytes);
          return len_bytes;
        },
    };
  }
};

class mc_block_interface {
 public:
  mc_block_interface() = default;
  virtual ~mc_block_interface() = default;

  /// id of this block
  virtual const char *getId() const noexcept = 0;
  /// first version
  virtual uint8_t getVersion() const noexcept = 0;
  /// id in 1.12
  virtual const char *getIdOld() const noexcept = 0;
  /// if this block needs a glass block under it
  virtual bool getNeedGlass() const noexcept = 0;
  /// if this block emits light
  virtual bool getDoGlow() const noexcept = 0;
  /// if this block can be stolen by enderman
  virtual bool getEndermanPickable() const noexcept = 0;
  /// if this block can be burnt
  virtual bool getBurnable() const noexcept = 0;

  virtual uint8_t getStackSize() const noexcept = 0;

  virtual const char *getNameZH() const noexcept = 0;
  virtual const char *getNameEN() const noexcept = 0;
  virtual const char *getImageFilename() const noexcept = 0;

  constexpr int imageRows() const noexcept { return 16; }
  constexpr int imageCols() const noexcept { return 16; }

  virtual void getImage(uint32_t *dest_row_major) const noexcept = 0;

  /// set block id
  virtual void setId(const char *) noexcept = 0;
  /// set first version
  virtual void setVersion(uint8_t) noexcept = 0;
  /// set id in 1.12
  virtual void setIdOld(const char *) noexcept = 0;
  /// set if this block needs a glass block under it
  virtual void setNeedGlass(bool) noexcept = 0;
  /// set if this block emits light
  virtual void setDoGlow(bool) noexcept = 0;
  /// set if this block can be stolen by enderman
  virtual void setEndermanPickable(bool) noexcept = 0;
  /// set if this block can be burnt
  virtual void setBurnable(bool) noexcept = 0;

  virtual void setStackSize(uint8_t) noexcept = 0;

  virtual void setNameZH(const char *) noexcept = 0;
  virtual void setNameEN(const char *) noexcept = 0;
  virtual void setImageFilename(const char *) noexcept = 0;

  virtual void setImage(const uint32_t *src, bool is_row_major) noexcept = 0;

  /// let *b equal to *this
  virtual void copyTo(mc_block_interface *b) const noexcept = 0;
  /// set this block to air
  virtual void clear() noexcept;

  virtual const char *idForVersion(SCL_gameVersion ver) const noexcept = 0;

  virtual bool getNeedStone(SCL_gameVersion v) const noexcept = 0;
  virtual void setNeedStone(SCL_gameVersion v, bool) noexcept = 0;
};

class block_list_interface {
 public:
  block_list_interface() = default;
  virtual ~block_list_interface() = default;
  [[nodiscard]] virtual size_t size() const noexcept = 0;
  [[nodiscard]] virtual size_t get_blocks(
      mc_block_interface **, uint8_t *,
      size_t capacity_in_elements) noexcept = 0;

  [[nodiscard]] virtual size_t get_blocks(
      const mc_block_interface **, uint8_t *,
      size_t capacity_in_elements) const noexcept = 0;

  [[nodiscard]] virtual bool contains(
      const mc_block_interface *) const noexcept = 0;
};

struct color_map_ptrs {
  const float *r_data;
  const float *g_data;
  const float *b_data;
  const uint8_t *map_data;
  int num_colors;
};

struct progress_callbacks {
  void *widget{nullptr};
  void (*cb_set_range)(void *, int, int, int){nullptr};
  void (*cb_add)(void *, int){nullptr};

  inline void set_range(int min, int max, int val) const {
    if (this->cb_set_range) {
      this->cb_set_range(this->widget, min, max, val);
    }
  }
  inline void add(int delta) const {
    if (this->cb_add) {
      this->cb_add(this->widget, delta);
    }
  }
};

struct ui_callbacks {
  void *wind{nullptr};
  void (*cb_keep_awake)(void *){nullptr};
  void (*cb_report_error)(void *, errorFlag, const char *){nullptr};
  void (*cb_report_working_status)(void *, workStatus){nullptr};

  inline void keep_awake() const {
    if (this->cb_keep_awake) {
      this->cb_keep_awake(this->wind);
    }
  }

  inline void report_error(errorFlag e, const char *msg) const {
    if (this->cb_report_error) {
      this->cb_report_error(this->wind, e, msg);
    }
  }
  inline void report_working_status(workStatus ws) const {
    if (this->cb_report_working_status) {
      this->cb_report_working_status(this->wind, ws);
    }
  }
};

struct convert_option {
  uint64_t caller_api_version{SC_VERSION_U64};
  SCL_convertAlgo algo{SCL_convertAlgo::RGB_Better};
  bool dither{false};
  GA_converter_option ai_cvter_opt{};
  progress_callbacks progress{};
  ui_callbacks ui{};
};

struct map_data_file_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  const char *folder_path{""};
  int begin_index{0};
  progress_callbacks progress{};
  ui_callbacks ui{};
};

struct map_data_file_give_command_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  ostream_wrapper *destination{};
  int begin_index{0};
  uint8_t stack_count{1};  /// <- Stack count of filed_map
  bool set_name_as_index{true};
  bool after_1_12{false};
  bool after_1_20_5{false};
};

struct assembled_maps_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  SCL_item_frame_variant frame_variant{SCL_item_frame_variant::common};
  SCL_map_facing map_facing{SCL_map_facing::wall_north};
  SCL_gameVersion mc_version{SCL_gameVersion::MC19};
  bool after_1_20_5{false};
  bool fixed_frame{true};
  bool invisible_frame{true};
  int begin_index{0};
};

struct build_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  uint16_t max_allowed_height{256};
  uint16_t bridge_interval{3};
  compressSettings compress_method{::SCL_compressSettings::noCompress};
  glassBridgeSettings glass_method{::SCL_glassBridgeSettings::noBridge};
  bool fire_proof{false};
  bool enderman_proof{false};

  // added in v5.1.0
  bool connect_mushrooms{false};
  // added in v5.3
  ui_callbacks ui;
  progress_callbacks main_progressbar;
  progress_callbacks sub_progressbar;
};

struct litematic_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  const char *litename_utf8 = "by SlopeCraft";
  const char *region_name_utf8 = "by SlopeCraft";
  ui_callbacks ui;
  progress_callbacks progressbar;
};
struct vanilla_structure_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  bool is_air_structure_void{true};
  ui_callbacks ui;
  progress_callbacks progressbar;
};
struct WE_schem_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  int offset[3] = {0, 0, 0};
  int we_offset[3] = {0, 0, 0};
  const char *const *required_mods_name_utf8{nullptr};
  int num_required_mods{0};
  ui_callbacks ui;
  progress_callbacks progressbar;
};

struct flag_diagram_options {
  uint64_t caller_api_version{SC_VERSION_U64};

  // 0 or negative number means no split lines
  int32_t split_line_row_margin{0};
  // 0 or negative number means no split lines
  int32_t split_line_col_margin{0};
  int png_compress_level{9};
  int png_compress_memory_level{8};

  ui_callbacks ui{};
  progress_callbacks progressbar{};
};

struct test_blocklist_options {
  uint64_t caller_api_version{SC_VERSION_U64};
  const mc_block_interface *const *block_ptrs{nullptr};
  const uint8_t *basecolors{nullptr};
  size_t block_count{0};
  string_deliver *err{nullptr};
};

struct const_image_reference {
  const uint32_t *data{nullptr};
  size_t rows{0};
  size_t cols{0};
};

class color_table;
class converted_image;
class structure_3D;

class color_table {
 public:
  virtual ~color_table() = default;
  [[nodiscard]] virtual color_map_ptrs colors() const noexcept = 0;
  [[nodiscard]] virtual ::SCL_mapTypes map_type() const noexcept = 0;

  [[nodiscard]] inline bool is_vanilla() const noexcept {
    return this->map_type() != SCL_mapTypes::FileOnly;
  }

  [[nodiscard]] inline bool is_flat() const noexcept {
    return this->map_type() == SCL_mapTypes::Flat;
  }

  [[nodiscard]] virtual ::SCL_gameVersion mc_version() const noexcept = 0;
  [[nodiscard]] virtual size_t num_blocks() const noexcept = 0;
  virtual void visit_blocks(void (*)(const mc_block_interface *,
                                     void *custom_data),
                            void *custom_data) const = 0;

  template <class visitor>
  void visit_blocks(visitor v) const {
    this->visit_blocks(
        [](const mc_block_interface *b, void *custom_data) {
          visitor *fun = reinterpret_cast<visitor *>(custom_data);
          (*fun)(b);
        },
        &v);
  }

  [[nodiscard]] virtual converted_image *convert_image(
      const_image_reference original_img,
      const convert_option &option) const noexcept = 0;

  [[nodiscard]] virtual bool has_convert_cache(
      const_image_reference original_img, const convert_option &option,
      const char *cache_dir) const noexcept = 0;
  [[nodiscard]] virtual bool save_convert_cache(
      const_image_reference original_img, const convert_option &option,
      const converted_image &, const char *cache_dir,
      string_deliver *error) const noexcept = 0;
  [[nodiscard]] virtual converted_image *load_convert_cache(
      const_image_reference original_img, const convert_option &option,
      const char *cache_dir, string_deliver *error) const noexcept = 0;

  [[nodiscard]] virtual structure_3D *build(
      const converted_image &, const build_options &) const noexcept = 0;
  // Once you cache a structure_3D, it can be released to save memory
  [[nodiscard]] virtual bool save_build_cache(
      const converted_image &, const build_options &, const structure_3D &,
      const char *cache_root_dir, string_deliver *error) const noexcept = 0;
  [[nodiscard]] virtual bool has_build_cache(
      const converted_image &, const build_options &,
      const char *cache_root_dir) const noexcept = 0;
  [[nodiscard]] virtual structure_3D *load_build_cache(
      const converted_image &, const build_options &,
      const char *cache_root_dir, string_deliver *error) const noexcept = 0;

  virtual void stat_blocks(const structure_3D &,
                           size_t buffer[64]) const noexcept = 0;

  [[nodiscard]] virtual bool generate_test_schematic(
      const char *filename,
      const test_blocklist_options &option) const noexcept = 0;
};

class converted_image {
 public:
  virtual ~converted_image() = default;
  [[nodiscard]] virtual size_t rows() const noexcept = 0;
  [[nodiscard]] virtual size_t cols() const noexcept = 0;
  [[nodiscard]] inline size_t height() const noexcept { return this->rows(); }
  [[nodiscard]] inline size_t width() const noexcept { return this->cols(); }
  [[nodiscard]] inline size_t size() const noexcept {
    return this->rows() * this->cols();
  }

  [[nodiscard]] inline size_t size_in_bytes() const noexcept {
    return sizeof(uint32_t) * this->rows() * this->cols();
  }

  virtual void get_original_image(uint32_t *buffer) const noexcept = 0;
  //  virtual void get_dithered_image(uint32_t *buffer) const noexcept = 0;
  virtual void get_converted_image(uint32_t *buffer) const noexcept = 0;

  virtual void get_compressed_image(const structure_3D &structure,
                                    uint32_t *buffer) const noexcept = 0;

  [[nodiscard]] virtual bool export_map_data(
      const map_data_file_options &option) const noexcept = 0;

  [[nodiscard]] virtual bool is_converted_from(
      const color_table &) const noexcept = 0;

  [[nodiscard]] virtual bool get_map_command(
      const map_data_file_give_command_options &option) const = 0;

  [[nodiscard]] virtual bool export_assembled_maps_litematic(
      const char *filename, const assembled_maps_options &,
      const litematic_options &) const noexcept = 0;
  [[nodiscard]] virtual bool export_assembled_maps_vanilla_structure(
      const char *filename, const assembled_maps_options &,
      const vanilla_structure_options &) const noexcept = 0;
};

class structure_3D {
 public:
  virtual ~structure_3D() = default;
  [[nodiscard]] virtual size_t shape_x() const noexcept = 0;
  [[nodiscard]] virtual size_t shape_y() const noexcept = 0;
  [[nodiscard]] virtual size_t shape_z() const noexcept = 0;
  [[nodiscard]] virtual size_t palette_length() const noexcept = 0;
  virtual void get_palette(const char **buffer_block_id) const noexcept = 0;

  [[nodiscard]] virtual bool export_litematica(
      const char *filename, const litematic_options &option) const noexcept = 0;
  [[nodiscard]] virtual bool export_vanilla_structure(
      const char *filename,
      const vanilla_structure_options &option) const noexcept = 0;
  [[nodiscard]] virtual bool export_WE_schem(
      const char *filename, const WE_schem_options &option) const noexcept = 0;
  [[nodiscard]] virtual bool export_flat_diagram(
      const char *filename, const color_table &table,
      const flag_diagram_options &option) const noexcept = 0;

  [[nodiscard]] virtual uint64_t block_count() const noexcept = 0;
};

}  // namespace SlopeCraft

// these functions are
extern "C" {
namespace SlopeCraft {

[[nodiscard]] SCL_EXPORT const float *SCL_get_rgb_basic_colorset_source();

[[nodiscard]] SCL_EXPORT mc_block_interface *SCL_create_block();
SCL_EXPORT void SCL_destroy_block(mc_block_interface *);

struct color_table_create_info {
  ::SCL_mapTypes map_type;
  ::SCL_gameVersion mc_version;
  bool basecolor_allow_LUT[64];
  const mc_block_interface *blocks[64];
  ui_callbacks ui;
};

[[nodiscard]] SCL_EXPORT color_table *SCL_create_color_table(
    const color_table_create_info &);
SCL_EXPORT void SCL_destroy_color_table(color_table *);

SCL_EXPORT void SCL_destroy_converted_image(converted_image *);
SCL_EXPORT void SCL_destroy_structure_3D(structure_3D *);

struct block_list_create_info {
  uint64_t caller_api_version{SC_VERSION_U64};
  string_deliver *warnings{nullptr};
  string_deliver *error{nullptr};
};

[[nodiscard]] SCL_EXPORT block_list_interface *SCL_create_block_list(
    const char *zip_filename, const block_list_create_info &option);
[[nodiscard]] SCL_EXPORT block_list_interface *
SCL_create_block_list_from_buffer(const void *buffer, size_t buffer_bytes,
                                  const block_list_create_info &option);
SCL_EXPORT void SCL_destroy_block_list(block_list_interface *);

SCL_EXPORT void SCL_preprocessImage(
    uint32_t *ARGB32ptr, const uint64_t imageSize,
    const SCL_PureTpPixelSt = SCL_PureTpPixelSt::ReplaceWithBackGround,
    const SCL_HalfTpPixelSt = SCL_HalfTpPixelSt::ComposeWithBackGround,
    uint32_t backGround = 0xFFFFFFFF);

SCL_EXPORT bool SCL_haveTransparentPixel(const uint32_t *ARGB32,
                                         const uint64_t imageSize);

SCL_EXPORT SCL_gameVersion SCL_maxAvailableVersion();

SCL_EXPORT const char *SCL_getSCLVersion();

SCL_EXPORT const float *SCL_getBasicColorMapPtrs();
SCL_EXPORT uint8_t SCL_maxBaseColor();
SCL_EXPORT SCL_gameVersion SCL_basecolor_version(uint8_t basecolor);
SCL_EXPORT void SCL_get_base_color_ARGB32(uint32_t dest[64]);

// SCL_EXPORT SCL_gameVersion SCL_basecolor_version(uint8_t basecolor);

// SCL_EXPORT uint64_t SCL_mcVersion2VersionNumber(::SCL_gameVersion);

class deleter {
 public:
  //  void operator()(Kernel *k) const noexcept { SCL_destroyKernel(k); }
  void operator()(mc_block_interface *b) const noexcept {
    SCL_destroy_block(b);
  }
  void operator()(block_list_interface *b) const noexcept {
    SCL_destroy_block_list(b);
  }
  void operator()(color_table *c) const noexcept { SCL_destroy_color_table(c); }
  void operator()(converted_image *c) const noexcept {
    SCL_destroy_converted_image(c);
  }
  void operator()(structure_3D *s) const noexcept {
    SCL_destroy_structure_3D(s);
  }
};
}  //  namespace SlopeCraft

}  //  extern "C"

// SCL_EXPORT void SCL_test();

#endif  // KERNEL_H
