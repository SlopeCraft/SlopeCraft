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

#ifndef WATERITEM_H
#define WATERITEM_H

#include <cstdint>
#include <utility>

struct rc_pos {
  int32_t row;
  int32_t col;

  [[nodiscard]] inline bool operator==(rc_pos b) const noexcept {
    return (this->row == b.row) && (this->col == b.col);
  }
};

template <>
struct std::hash<rc_pos> {
  size_t operator()(rc_pos pos) const noexcept {
    return std::hash<uint64_t>{}(reinterpret_cast<const uint64_t&>(pos));
  }
};

struct water_y_range {
  int high_y;
  int low_y;
};
[[nodiscard]] inline bool operator==(water_y_range a,
                                     water_y_range b) noexcept {
  return (a.high_y == b.high_y) && (a.low_y == b.low_y);
}

// constexpr water_y_range nullWater =
//     water_y_range{.high_y = INT_MIN, .low_y = INT_MIN};
constexpr int32_t WATER_COLUMN_SIZE[3] = {11, 6, 1};

#endif  // WATERITEM_H
