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

#ifndef SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H
#define SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <optional>

#include <ColorManip/ColorManip.h>
#include <Eigen/Dense>

#include "Resource_tree.h"
#include "VisualCraftL.h"

/*
#if __cplusplus < 202002L
#warning Requires C++23
// This is used to disable stupid error from clangd.
namespace std {
void unreachable() { __builtin_trap(); }
} // namespace std
#endif

*/

// struct VCL_resource_pack;

/// resize image
Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
resize_image_nearest(const Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic,
                                        Eigen::RowMajor> &src,
                     int rows, int cols) noexcept;

Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
resize_image_nearest(
    const decltype(Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic,
                                Eigen::RowMajor>()
                       .block(0, 0, 1, 1)) src_block,
    int rows, int cols) noexcept;

Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
process_dynamic_texture(const Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic,
                                           Eigen::RowMajor> &src) noexcept;

bool parse_png(
    const void *const data, const int64_t length,
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> *img);

namespace block_model {
constexpr int x_idx = 0;
constexpr int y_idx = 1;
constexpr int z_idx = 2;
constexpr int EW_idx = x_idx;
constexpr int NS_idx = z_idx;
constexpr int UD_idx = y_idx;

using EImgRowMajor_t =
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

enum class face_rot : uint8_t {
  face_rot_0 = 0,
  face_rot_90 = 9,
  face_rot_180 = 18,
  face_rot_270 = 27
};

constexpr bool inline is_0_90_180_270(int val) noexcept {
  switch (val) {
    case 0:
    case 90:
    case 180:
    case 270:
      return true;
  }
  return false;
}

inline face_rot int_to_face_rot(int a) {
  switch (a) {
    case 0:
      return face_rot::face_rot_0;
    case 90:
      return face_rot::face_rot_90;
    case 180:
      return face_rot::face_rot_180;
    case 270:
      return face_rot::face_rot_270;

    default:
      printf(
          "\nFunction int_to_face_rot failed to convert int to face_rot : "
          "invalid value : %i\n",
          a);
      abort();
      return face_rot::face_rot_0;
  }
}

using face_idx = ::VCL_face_t;

/// assocate xyz with NWSE and up/down
constexpr face_idx face_x_pos = face_idx::face_east;
constexpr face_idx face_x_neg = face_idx::face_west;
constexpr face_idx face_z_pos = face_idx::face_south;
constexpr face_idx face_z_neg = face_idx::face_north;
constexpr face_idx face_y_pos = face_idx::face_up;
constexpr face_idx face_y_neg = face_idx::face_down;

constexpr inline face_idx inverse_face(const face_idx fi) noexcept {
  switch (fi) {
    case face_idx::face_down:
      return face_idx::face_up;
    case face_idx::face_up:
      return face_idx::face_down;

    case face_idx::face_north:
      return face_idx::face_south;
    case face_idx::face_south:
      return face_idx::face_north;

    case face_idx::face_east:
      return face_idx::face_west;
    case face_idx::face_west:
      return face_idx::face_east;
  }
  // std::unreachable();

  return face_idx::face_up;
}

constexpr inline bool is_parallel(const face_idx fiA,
                                  const face_idx fiB) noexcept {
  if (fiA == fiB || fiA == inverse_face(fiB)) {
    return true;
  }
  return false;
}

constexpr inline face_idx rotate_x(face_idx original, face_rot x_rot) noexcept {
  if (original == face_x_neg || original == face_x_pos) {
    return original;
  }

  switch (x_rot) {
    case face_rot::face_rot_0:
      return original;

    case face_rot::face_rot_180:
      return inverse_face(original);

    case face_rot::face_rot_90: {
      switch (original) {
        case face_y_pos:
          return face_z_neg;
        case face_z_neg:
          return face_y_neg;
        case face_y_neg:
          return face_z_pos;
        case face_z_pos:
          return face_y_pos;
        default:
          // std::unreachable();
          return {};
      }
    }

    case face_rot::face_rot_270: {
      return inverse_face(rotate_x(original, face_rot::face_rot_90));
    }
  }
  // std::unreachable();
  return {};
}

constexpr inline face_idx invrotate_x(face_idx rotated,
                                      face_rot x_rot) noexcept {
  switch (x_rot) {
    case face_rot::face_rot_0:
      return rotated;
    case face_rot::face_rot_180:
      return inverse_face(rotated);
    case face_rot::face_rot_90:
      return rotate_x(rotated, face_rot::face_rot_270);
    case face_rot::face_rot_270:
      return rotate_x(rotated, face_rot::face_rot_90);
  }
  // std::unreachable();
  return {};
}

constexpr inline face_idx rotate_y(face_idx original, face_rot y_rot) noexcept {
  if (original == face_y_neg || original == face_y_pos) {
    return original;
  }

  switch (y_rot) {
    case face_rot::face_rot_0:
      return original;
    case face_rot::face_rot_180:
      return inverse_face(original);
    case face_rot::face_rot_90: {
      switch (original) {
        case face_z_pos:
          return face_x_neg;
        case face_x_neg:
          return face_z_neg;
        case face_z_neg:
          return face_x_pos;
        case face_x_pos:
          return face_z_pos;
        default:
          // std::unreachable();
          return {};
      }
    }

    case face_rot::face_rot_270:
      return inverse_face(rotate_y(original, face_rot::face_rot_90));
  }

  // std::unreachable();
  return {};
}

constexpr inline face_idx invrotate_y(face_idx rotated,
                                      face_rot y_rot) noexcept {
  switch (y_rot) {
    case face_rot::face_rot_0:
      return rotated;
    case face_rot::face_rot_180:
      return inverse_face(rotated);
    case face_rot::face_rot_90:
      return rotate_y(rotated, face_rot::face_rot_270);
    case face_rot::face_rot_270:
      return rotate_y(rotated, face_rot::face_rot_90);
  }
  // std::unreachable();
  return {};
}

constexpr inline face_idx rotate(face_idx original, face_rot x_rot,
                                 face_rot y_rot) noexcept {
  return rotate_y(rotate_x(original, x_rot), y_rot);
}

constexpr inline face_idx invrotate(face_idx original, face_rot x_rot,
                                    face_rot y_rot) noexcept {
  return invrotate_x(invrotate_y(original, y_rot), x_rot);
}

Eigen::Array3f rotate_x(const Eigen::Array3f &pos, face_rot x_rot) noexcept;
Eigen::Array3f rotate_y(const Eigen::Array3f &pos, face_rot y_rot) noexcept;
inline Eigen::Array3f rotate(const Eigen::Array3f &pos, face_rot x_rot,
                             face_rot y_rot) noexcept {
  return rotate_y(rotate_x(pos, x_rot), y_rot);
}

class ray_t {
 public:
  ray_t() = delete;
  explicit ray_t(const face_idx f);
  Eigen::Array3f abc;
  Eigen::Array3f x0y0z0;
};

/// Ax+By+Cz+D=0
class plane_t {
 public:
  plane_t() = delete;
  explicit plane_t(const Eigen::Array3f &normVec, const Eigen::Array3f &point)
      : ABC(normVec), D(-(normVec * point).sum()) {}

  Eigen::Array3f ABC;
  float D;
};

class face_t {
 public:
  const EImgRowMajor_t *texture{nullptr};
  /// It is not pixel index, but [0,1]*16 stored in integer
  std::array<float, 2> uv_start{0, 0};
  /// It is not pixel index, but [0,1]*16 stored in integer
  std::array<float, 2> uv_end{16, 16};
  face_rot rot{face_rot::face_rot_0};
  bool is_hidden{false};

  inline bool is_uv_whole_texture() const noexcept {
    return (uv_start[0] <= 0 && uv_start[1] <= 0) &&
           (uv_end[0] >= 16 && uv_end[1] >= 16);
  }
};

inline ARGB color_at_relative_idx(const EImgRowMajor_t &img, const float r_f,
                                  const float c_f) noexcept {
  const int r_i = std::min<int>(std::max(int(std::floor(r_f * img.rows())), 0),
                                img.rows() - 1);
  const int c_i = std::min<int>(std::max(int(std::floor(c_f * img.cols())), 0),
                                img.cols() - 1);
  /*
  printf("\ncolor_at_relative_idx : r_f = %f, c_f = %f, r_i = %i, c_i = %i",
         r_f, c_f, r_i, c_i);
         */

  return img(r_i, c_i);
}

struct intersect_point {
  float distance;
  std::array<float, 2> uv;
  // Eigen::Array3f coordinate;
  const face_t *face_ptr;

  ///(u,v) in range[0,1]. corresponding to (c,r)

  ARGB color() const noexcept;
};

/// A 3d box to be displayed
class element {
 public:
  Eigen::Array3f _from;
  Eigen::Array3f _to;
  std::array<face_t, 6> faces;

  inline face_t &face(face_idx fi) noexcept { return faces[uint8_t(fi)]; }

  inline const face_t &face(face_idx fi) const noexcept {
    return faces[uint8_t(fi)];
  }

  plane_t plane(face_idx fi) const noexcept;

  inline float x_range() const noexcept { return _to[0] - _from[0]; }
  inline float y_range() const noexcept { return _to[1] - _from[1]; }
  inline float z_range() const noexcept { return _to[2] - _from[2]; }

  inline float x_range_abs() const noexcept {
    return std::abs(this->x_range());
  }
  inline float y_range_abs() const noexcept {
    return std::abs(this->y_range());
  }
  inline float z_range_abs() const noexcept {
    return std::abs(this->z_range());
  }

  inline Eigen::Array3f xyz_minpos() const noexcept {
    return this->_from.min(this->_to);
  }

  inline Eigen::Array3f xyz_maxpos() const noexcept {
    return this->_from.max(this->_to);
  }

  inline float volume() const noexcept {
    return std::abs(x_range() * y_range() * z_range());
  }

  inline int shown_face_num() const noexcept {
    int result = 0;
    for (const auto &f : faces) {
      if (!f.is_hidden) result++;
    }
    return result;
  }

  inline bool is_not_outside(const Eigen::Array3f &point) const noexcept {
    return (point >= this->xyz_minpos()).all() &&
           (point <= this->xyz_maxpos()).all();
  }

  void intersect_points(
      const face_idx f, const ray_t &ray,
      std::vector<intersect_point> *const dest) const noexcept;

  element rotate(face_rot x_rot, face_rot y_rot) const noexcept;
};

/// a block model
class model {
 public:
  std::vector<element> elements;

  inline void get_faces(
      std::vector<const face_t *> *const dest) const noexcept {
    dest->reserve(elements.size() * 6);
    dest->clear();

    for (const element &ele : elements) {
      for (const face_t &f : ele.faces) {
        if (!f.is_hidden) {
          dest->emplace_back(&f);
        }
      }
    }
  }

  EImgRowMajor_t projection_image(face_idx fidx) const noexcept;

  void projection_image(face_idx idx,
                        EImgRowMajor_t *const dest) const noexcept;

  void merge_back(const model &md, face_rot x_rot, face_rot y_rot) noexcept;
};

}  // namespace block_model

std::optional<std::array<uint8_t, 3>> compute_mean_color(
    const block_model::EImgRowMajor_t &img) noexcept;

bool compose_image_background_half_transparent(
    block_model::EImgRowMajor_t &frontend_and_dest,
    const block_model::EImgRowMajor_t &backend) noexcept;

std::array<uint8_t, 3> compose_image_and_mean(
    const block_model::EImgRowMajor_t &front,
    const block_model::EImgRowMajor_t &back, bool *const ok = nullptr) noexcept;

namespace resource_json {

struct state {
  state() = default;
  explicit state(const char *k, const char *v) {
    this->key = k;
    this->value = v;
  }
  std::string key;
  std::string value;
};

struct model_store_t {
  std::string model_name;
  block_model::face_rot x{block_model::face_rot::face_rot_0};
  block_model::face_rot y{block_model::face_rot::face_rot_0};
  bool uvlock{false};
};

struct model_pass_t {
  model_pass_t() = default;
  explicit model_pass_t(const model_store_t &src) {
    this->model_name = src.model_name.data();
    this->x = src.x;
    this->y = src.y;
    this->uvlock = src.uvlock;
  }

  const char *model_name;
  block_model::face_rot x{block_model::face_rot::face_rot_0};
  block_model::face_rot y{block_model::face_rot::face_rot_0};
  bool uvlock{false};
};

class state_list : public std::vector<state> {
 public:
  size_t num_1() const noexcept;
  bool euqals(const state_list &another) const noexcept;
  bool contains(const state_list &another) const noexcept;

  // bool contains_auto(const state_list &another) noexcept;
};

bool process_full_id(std::string_view full_id, std::string *namespace_name,
                     std::string *pure_id, state_list *states) noexcept;

// bool state_list_equals(const state_list &sla, const state_list &slb)
// noexcept;

class block_states_variant {
 public:
  model_pass_t block_model_name(const state_list &sl) const noexcept;

  std::vector<std::pair<state_list, model_store_t>> LUT;

  void sort() noexcept;
};

struct criteria {
 public:
  std::string key;
  std::vector<std::string> values;

  inline bool match(const state &state) const noexcept {
    if (state.key != this->key) return false;
    return this->match(state.value);
  }

  inline bool match(std::string_view value) const noexcept {
    for (const std::string &v : this->values) {
      if (v == value) return true;
    }
    return false;
  }
};

class criteria_list_and : public std::vector<criteria> {
 public:
  bool match(const state_list &sl) const noexcept;
};

struct criteria_list_or_and {
  std::vector<criteria_list_and> components;
  bool is_or{true};
};

struct criteria_all_pass {};

/// @return true if sl matches every criteria in cl
[[deprecated]] bool match_criteria_list(const criteria_list_and &cl,
                                        const state_list &sl) noexcept;

class multipart_pair {
 public:
  std::variant<criteria, criteria_list_or_and, criteria_all_pass>
      criteria_variant;
  std::vector<model_store_t> apply_blockmodel;
  /*
  criteria when;
  std::vector<criteria_list_and> when_or;
  */

  bool match(const state_list &sl) const noexcept;
};

class block_state_multipart {
 public:
  std::vector<multipart_pair> pairs;

  std::vector<model_pass_t> block_model_names(
      const state_list &sl) const noexcept;
};

bool parse_block_state(
    const char *const json_str_beg, const char *const end,
    std::variant<block_states_variant, block_state_multipart> *,
    bool *const is_dest_variant = nullptr) noexcept;

}  // namespace resource_json

std::optional<block_model::face_idx> string_to_face_idx(
    std::string_view str) noexcept;
const char *face_idx_to_string(block_model::face_idx) noexcept;

struct model_with_rotation {
  const block_model::model *model_ptr{nullptr};
  block_model::face_rot x_rot{block_model::face_rot::face_rot_0};
  block_model::face_rot y_rot{block_model::face_rot::face_rot_0};
};

class VCL_model {
 public:
  std::variant<model_with_rotation, block_model::model> value;
};

class VCL_resource_pack;
using resource_pack = VCL_resource_pack;
/**
 * \note Name of texture = <namespacename>:blocks/<pngfilename without prefix>
 */
class VCL_resource_pack {
 public:
  VCL_resource_pack() = default;
  VCL_resource_pack(const VCL_resource_pack &src) = delete;
  VCL_resource_pack(VCL_resource_pack &&) = default;

  VCL_resource_pack &operator=(const VCL_resource_pack &src) noexcept {
    if (!this->copy(src)) {
      abort();
    }

    return *this;
  }

  VCL_resource_pack &operator=(VCL_resource_pack &&src) noexcept {
    this->block_models = std::move(src.block_models);
    this->textures_original = std::move(src.textures_original);
    this->textures_override = std::move(src.textures_override);
    this->block_states = std::move(src.block_states);
    this->colormap_foliage = std::move(src.colormap_foliage);
    this->colormap_grass = std::move(src.colormap_grass);

    this->is_MC12 = src.is_MC12;

    return *this;
  }

  using namespace_name_t = std::string;

  inline void set_is_MC12(bool val) noexcept { this->is_MC12 = val; }

  bool add_colormaps(const zipped_folder &resource_pack_root) noexcept;

  bool add_textures(const zipped_folder &resourece_pack_root,
                    const bool on_conflict_replace_old = true) noexcept;

  bool add_block_models(const zipped_folder &resource_pack_root,
                        const bool on_conflict_replace_old = true) noexcept;

  bool add_block_states(const zipped_folder &resourece_pack_root,
                        const bool on_conflict_replace_old = true) noexcept;

  const Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> *
  find_texture(std::string_view path, bool override_only) const noexcept;

  bool override_required_textures(VCL_biome_t biome,
                                  bool replace_transparent_with_black,
                                  const VCL_block *const *const blkpp,
                                  size_t num_blkp) noexcept;

  bool override_texture(std::string_view path_in_original,
                        uint32_t standard_color,
                        bool replace_transparent_with_black) noexcept;

  std::array<int, 2> locate_color_rc(VCL_biome_info) const noexcept;

  uint32_t standard_color(VCL_biome_info info, bool is_foliage) const noexcept;
  uint32_t standard_color(VCL_biome_t biome,
                          std::string_view texture_name) const noexcept;

  inline auto &get_textures_original() const noexcept {
    return this->textures_original;
  }
  inline auto &get_textures_override() const noexcept {
    return this->textures_override;
  }

  inline auto &get_models() const noexcept { return this->block_models; }
  inline auto &get_block_states() const noexcept { return this->block_states; }

  inline void clear_textures() noexcept {
    this->textures_original.clear();
    this->textures_override.clear();
  }
  inline void clear_models() noexcept { this->block_models.clear(); }
  inline void clear_block_states() noexcept { this->block_states.clear(); }

  inline void clear_texture_override() noexcept {
    this->textures_override.clear();
  }

  struct buffer_t {
    std::string pure_id;
    // std::vector<std::pair<std::string, std::string>> traits;
    resource_json::state_list state_list;
  };

  std::variant<model_with_rotation, block_model::model> find_model(
      const std::string &block_state_str) const noexcept {
    buffer_t b;
    return this->find_model(block_state_str, b);
  }

  std::variant<model_with_rotation, block_model::model> find_model(
      const std::string &block_state_str, buffer_t &) const noexcept;

  bool compute_projection(const std::string &block_state_str,
                          VCL_face_t face_exposed,
                          block_model::EImgRowMajor_t *const img,
                          buffer_t &) const noexcept;

  inline const auto &get_colormap(bool is_foliage) const noexcept {
    return (is_foliage) ? (this->colormap_foliage) : (this->colormap_grass);
  }

 private:
  std::unordered_map<std::string, block_model::model> block_models;
  std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                               Eigen::Dynamic, Eigen::RowMajor>>
      textures_original;
  std::unordered_map<std::string,
                     std::variant<resource_json::block_states_variant,
                                  resource_json::block_state_multipart>>
      block_states;
  block_model::EImgRowMajor_t colormap_grass;
  block_model::EImgRowMajor_t colormap_foliage;

  std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                               Eigen::Dynamic, Eigen::RowMajor>>
      textures_override;

  bool is_MC12{false};

  inline const char *texture_prefix_s() const noexcept {
    if (is_MC12)
      return "block";
    else {
      return "blocks";
    }
  }

  inline const char *texture_prefix_s_slash() const noexcept {
    if (is_MC12)
      return "block/";
    else {
      return "blocks/";
    }
  }

  bool add_textures_direct(
      const std::unordered_map<std::string, zipped_file> &pngs,
      std::string_view namespace_name,
      const bool on_conflict_replace_old) noexcept;
  bool add_colormap(const zipped_folder &resourece_pack_root,
                    std::string_view filename,
                    block_model::EImgRowMajor_t &img) noexcept;

  bool update_block_model_textures() noexcept;

  bool filter_model_textures(
      const std::unordered_map<const block_model::EImgRowMajor_t *,
                               const block_model::EImgRowMajor_t *> &filter,
      bool is_missing_error) noexcept;

  bool copy(const VCL_resource_pack &src) noexcept;
};

#endif  // SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H