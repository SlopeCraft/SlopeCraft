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

#ifndef EXTERNAL_CONVERTER_STATIC_INTERFACE_H
#define EXTERNAL_CONVERTER_STATIC_INTERFACE_H

#include <Eigen/Dense>

#include "../ColorManip/ColorManip.h"

using EImage = Eigen::ArrayXX<uint32_t>;

using MapList = Eigen::Array<uint8_t, Eigen::Dynamic, 1, Eigen::ColMajor, 256>;
using ColorList = Eigen::Array<float, Eigen::Dynamic, 3, Eigen::ColMajor, 256>;
using TempVectorXf =
    Eigen::Array<float, Eigen::Dynamic, 1, Eigen::ColMajor, 256>;

namespace SlopeCraft {

extern int colorCount4External();

extern Eigen::Map<const Eigen::ArrayXf> BasicRGB4External(int channel);

// extern Eigen::Map<const Eigen::ArrayXf> AllowedRGB4External(int channel);
//
// extern Eigen::Map<const Eigen::Array<uint8_t, Eigen::Dynamic, 1>>
// AllowedMapList4External();

}  // namespace SlopeCraft

#endif  // EXTERNAL_CONVERTER_STATIC_INTERFACE_H