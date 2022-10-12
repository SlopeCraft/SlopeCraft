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

using EImgRowMajor_t =
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

enum class face_rot : uint8_t {
  face_rot_0,
  face_rot_90,
  face_rot_180,
  face_rot_270
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

enum class face_idx : uint8_t {
  face_up,
  face_down,
  face_north,
  face_south,
  face_east,
  face_west
};

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
}

constexpr inline bool is_parallel(const face_idx fiA,
                                  const face_idx fiB) noexcept {
  if (fiA == fiB || fiA == inverse_face(fiB)) {
    return true;
  }
  return false;
}

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
};

class model {
public:
  std::vector<element> elements;

  EImgRowMajor_t projection_image(face_idx fidx) const noexcept;
};

} // namespace block_model

#endif // SLOPECRAFT_VISUALCRAFTL_PARSERESOURCEPACK_H