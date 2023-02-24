#ifndef SLOPECRAFT_VISUALCRAFTL_DIRECTION_HANDLER_H
#define SLOPECRAFT_VISUALCRAFTL_DIRECTION_HANDLER_H

#include "VisualCraftL.h"
#include <ranges>

template <typename int_t = int32_t> class dir_handler {
private:
  std::array<int_t, 3> size_xyz{0, 0, 0};
  VCL_face_t map_face;

public:
  dir_handler() = delete;
  dir_handler(VCL_face_t __face, int_t rows, int_t cols, int_t depth)
      : map_face(__face) {
    switch (this->map_face) {
    case VCL_face_t::face_north:
    case VCL_face_t::face_south:
      this->size_xyz = {cols, rows, depth};
      return;
    case VCL_face_t::face_east:
    case VCL_face_t::face_west:
      this->size_xyz = {depth, rows, cols};
      return;
    case VCL_face_t::face_up:
    case VCL_face_t::face_down:
      this->size_xyz = {cols, depth, rows};
      return;
    }
    abort();
  }

  inline const auto &range_xyz() const noexcept { return this->size_xyz; }
  inline auto face() const noexcept { return this->map_face; }

  // VCL_upper_direction_t direction;

  static constexpr size_t idx_x = 0;
  static constexpr size_t idx_y = 1;
  static constexpr size_t idx_z = 2;

  std::array<int_t, 3> coordinate_of(int_t r, int_t c,
                                     int_t depth) const noexcept {
    /*
switch (this->map_face) {
case VCL_face_t::face_up: {
}
case VCL_face_t::face_down: {
}
default:
return this->coord_when_vertical(r, c, depth);
}
*/
    return this->coord_when_no_rot(r, c, depth);
  }

private:
  std::array<int_t, 3> coord_when_no_rot(int_t r, int_t c,
                                         int_t depth) const noexcept {
    switch (this->map_face) {
    case VCL_face_t::face_east:
      return {this->size_xyz[idx_x] - depth - 1, this->size_xyz[idx_y] - r - 1,
              this->size_xyz[idx_z] - c - 1};

    case VCL_face_t::face_west:
      return {depth, this->size_xyz[idx_y] - r - 1, c};

    case VCL_face_t::face_north:
      return {this->size_xyz[idx_x] - c - 1, this->size_xyz[idx_y] - r - 1,
              depth};

    case VCL_face_t::face_south:
      return {c, this->size_xyz[idx_y] - r - 1,
              this->size_xyz[idx_z] - depth - 1};

    case VCL_face_t::face_up:
      return {c, this->size_xyz[idx_y] - depth - 1, r};

    case VCL_face_t::face_down:
      return {c, depth, this->size_xyz[idx_z] - r - 1};
    }
    abort();
    return {};
  }
};

#endif // SLOPECRAFT_VISUALCRAFTL_DIRECTION_HANDLER_H