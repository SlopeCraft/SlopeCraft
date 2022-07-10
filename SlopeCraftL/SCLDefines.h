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

#ifndef SCLDEFINES_H
#define SCLDEFINES_H

#define EIGEN_NO_DEBUG
#include <Eigen/Dense>
#include <iostream>
#include "SlopeCraftL.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

using std::cout,std::cerr,std::endl;

namespace SlopeCraft {

extern const float RGBBasicSource[256*3];

}

using ARGB = uint32_t;
using EImage = Eigen::Array<ARGB,Eigen::Dynamic,Eigen::Dynamic>;
using MapList = Eigen::Array<uint8_t,Eigen::Dynamic,1,Eigen::ColMajor,256>;
using ColorList = Eigen::Array<float,Eigen::Dynamic,3,Eigen::ColMajor,256>;
using TempVectorXf = Eigen::Array<float,Eigen::Dynamic,1,Eigen::ColMajor,256>;

#endif // SCLDEFINES_H
