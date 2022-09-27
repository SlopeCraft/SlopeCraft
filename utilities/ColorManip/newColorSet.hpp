/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SCL_NEWCOLORSET_HPP
#define SCL_NEWCOLORSET_HPP

#include "ColorManip.h"
#include "colorset_maptical.hpp"
#include "colorset_optical.hpp"
#include "newTokiColor.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <type_traits>

// using Eigen::Dynamic;

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
};

#endif // SCL_NEWCOLORSET_HPP