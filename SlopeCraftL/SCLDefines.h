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

#ifndef SCLDEFINES_H
#define SCLDEFINES_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

// #include <omp.h>

#define EIGEN_NO_DEBUG
#include <Eigen/Dense>
#include <iostream>
#include <ColorManip/newColorSet.hpp>
#include <ColorManip/newTokiColor.hpp>

#include "SlopeCraftL.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define mapColor2Index(mapColor) (64 * ((mapColor) % 4) + ((mapColor) / 4))
#define index2mapColor(index) (4 * ((index) % 64) + ((index) / 64))
#define mapColor2baseColor(mapColor) ((mapColor) >> 2)
#define index2baseColor(index) (mapColor2baseColor(index2mapColor(index)))
#define mapColor2depth(mapColor) ((mapColor) % 4)
#define index2depth(index) (mapColor2depth(index2mapColor(index)))

using std::cout, std::cerr, std::endl;

using Eigen::Dynamic;

using colorset_allowed_t = colorset_new<false, true>;
using colorset_basic_t = colorset_new<true, true>;

using TokiColor = newTokiColor<true, colorset_basic_t, colorset_allowed_t>;

namespace SlopeCraft {

extern const float RGBBasicSource[256 * 3];
extern const std::unique_ptr<const colorset_basic_t> basic_colorset;

}  // namespace SlopeCraft

#define SC_HASH_ADD_DATA(hasher, obj) hasher.process_bytes(&obj, sizeof(obj));

using ARGB = uint32_t;
using EImage = Eigen::Array<ARGB, Dynamic, Dynamic>;
using MapList = Eigen::Array<uint8_t, Dynamic, 1, Eigen::ColMajor, 256>;
using ColorList = Eigen::Array<float, Dynamic, 3, Eigen::ColMajor, 256>;
using TempVectorXf = Eigen::Array<float, Dynamic, 1, Eigen::ColMajor, 256>;

#endif  // SCLDEFINES_H
