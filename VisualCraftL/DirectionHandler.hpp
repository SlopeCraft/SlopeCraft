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