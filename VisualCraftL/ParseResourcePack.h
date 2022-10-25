#ifndef SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H
#define SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H

#include "Resource_tree.h"
#include <Eigen/Dense>

#include <ColorManip/ColorManip.h>

#include <unordered_map>

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

bool parse_png(
    const void *const data, const int64_t length,
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> *img);

std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                             Eigen::Dynamic, Eigen::RowMajor>>
folder_to_images(const zipped_folder &src, bool *const error = nullptr,
                 std::string *const error_string = nullptr) noexcept;

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
  face_rot_0,
  face_rot_90,
  face_rot_180,
  face_rot_270
};

enum class face_idx : uint8_t {
  face_up,
  face_down,
  face_north,
  face_south,
  face_east,
  face_west
};

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

  return face_idx::face_up;
}

constexpr inline bool is_parallel(const face_idx fiA,
                                  const face_idx fiB) noexcept {
  if (fiA == fiB || fiA == inverse_face(fiB)) {
    return true;
  }
  return false;
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
  std::array<int16_t, 2> uv_start{0, 0};
  /// It is not pixel index, but [0,1]*16 stored in integer
  std::array<int16_t, 2> uv_end{16, 16};
  face_rot rot{face_rot::face_rot_0};
  bool is_hidden{false};
};

struct intersect_point {
  float distance;
  Eigen::Array3f coordinate;
  const face_t *face_ptr;
  std::array<int16_t, 2> uv;

  inline ARGB color() const noexcept {
    return face_ptr->texture->operator()(uv[1], uv[0]);
  }
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

  inline float x_range_abs() const noexcept { return std::abs(x_range()); }
  inline float y_range_abs() const noexcept { return std::abs(y_range()); }
  inline float z_range_abs() const noexcept { return std::abs(z_range()); }

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
      if (!f.is_hidden)
        result++;
    }
    return result;
  }

  inline bool is_not_outside(const Eigen::Array3f &point) const noexcept {
    return (point >= this->xyz_minpos()).all() &&
           (point <= this->xyz_maxpos()).all();
  }

  void
  intersect_points(const face_idx f, const ray_t &ray,
                   std::vector<intersect_point> *const dest) const noexcept;
};

/// a block model
class model {
public:
  std::vector<element> elements;

  inline void
  get_faces(std::vector<const face_t *> *const dest) const noexcept {
    if (dest == nullptr) {
      return;
    }

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
};

} // namespace block_model

#endif // SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H