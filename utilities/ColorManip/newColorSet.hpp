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

#ifndef SCL_NEWCOLORSET_HPP
#define SCL_NEWCOLORSET_HPP

#include "colorset_maptical.hpp"
#include "colorset_optical.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <type_traits>
#include <span>
#include <SC_GlobalEnums.h>

// using Eigen::Dynamic;

namespace internal {

struct hash_temp {
  std::array<std::array<const float *, 3>, 4> color_ptrs;
  const void *color_id_ptr;
  int color_count;
  bool is_maptical;
};

// std::vector<uint8_t> hash_of_colorset(const hash_temp &) noexcept;

}  // namespace internal

template <bool is_basic, bool is_not_optical>
class colorset_new : public std::conditional_t<
                         is_not_optical,
                         std::conditional_t<is_basic, colorset_maptical_basic,
                                            colorset_maptical_allowed>,
                         std::conditional_t<is_basic, colorset_optical_basic,
                                            colorset_optical_allowed>> {
 public:
  template <typename = void>
  colorset_new(const float *const src) : colorset_maptical_basic(src) {
    static_assert(is_basic,
                  "This initialization requires colorset type to be basic.");
    static_assert(is_not_optical,
                  "This initialization requires colorset type to be maptical.");
  }

  colorset_new() = default;

  inline float color_value(const SCL_convertAlgo algo, const int r,
                           const int c) const noexcept {
    switch (algo) {
      case SCL_convertAlgo::gaCvter:
      case SCL_convertAlgo::RGB:
      case SCL_convertAlgo::RGB_Better:
        return this->RGB(r, c);

      case SCL_convertAlgo::HSV:
        return this->HSV(r, c);
      case SCL_convertAlgo::Lab94:
      case SCL_convertAlgo::Lab00:
        return this->Lab(r, c);
      case SCL_convertAlgo::XYZ:
        return this->XYZ(r, c);
    }
    return NAN;
  }

  std::span<const float> rgb_data_span(int ch) const noexcept {
    return {this->rgb_data(ch), (size_t)this->color_count()};
  }

  std::span<const float> hsv_data_span(int ch) const noexcept {
    return {this->hsv_data(ch), (size_t)this->color_count()};
  }

  std::span<const float> lab_data_span(int ch) const noexcept {
    return {this->lab_data(ch), (size_t)this->color_count()};
  }

  std::span<const float> xyz_data_span(int ch) const noexcept {
    return {this->xyz_data(ch), (size_t)this->color_count()};
  }

 private:
  template <typename = void>
  internal::hash_temp hash_temp() const noexcept {
    internal::hash_temp temp;
    temp.color_count = this->color_count();
    temp.color_ptrs[0] = {this->rgb_data(0), this->rgb_data(1),
                          this->rgb_data(2)};
    temp.color_ptrs[1] = {this->hsv_data(0), this->hsv_data(1),
                          this->hsv_data(2)};
    temp.color_ptrs[2] = {this->lab_data(0), this->lab_data(1),
                          this->lab_data(2)};
    temp.color_ptrs[3] = {this->xyz_data(0), this->xyz_data(1),
                          this->xyz_data(2)};

    temp.color_id_ptr = this->map_data();
    temp.is_maptical = is_not_optical;
    return temp;
  }
  /*
  template <typename = void>
  std::vector<uint8_t> hash() const noexcept {
    return internal::hash_of_colorset(temp);
  }
  */

 public:
  template <class hash_stream>
  void hash_add_data(hash_stream &stream) const noexcept {
    auto temp = this->hash_temp();

    for (const auto &cptrs : temp.color_ptrs) {
      for (const float *fptr : cptrs) {
        stream.process_bytes(fptr, temp.color_count * sizeof(float));
      }
    }

    stream.process_bytes(
        temp.color_id_ptr,
        temp.color_count *
            (temp.is_maptical ? sizeof(uint8_t) : sizeof(uint16_t)));
    stream.process_bytes(&temp.color_count, sizeof(temp.color_count));
  }
};

#endif  // SCL_NEWCOLORSET_HPP