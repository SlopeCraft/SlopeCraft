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

#ifndef DEFINES_H
#define DEFINES_H

#define EIGEN_NO_DEBUG
#include <Eigen/Dense>

#include "SlopeCraftL.h"

#ifndef WITH_QT
    #define removeQt
    #define emit ;
#endif

#ifdef STRAIGHT_INCLUDE_KERNEL
#ifndef NO_DLL
#error YOU MUST DEFINE NO_DLL WHEN DEFINED STRAIGHT_INCLUDE_KERNEL
#endif
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned int ARGB;
typedef Eigen::Array<ARGB,Eigen::Dynamic,Eigen::Dynamic> EImage;
typedef Eigen::Array<uint8_t,Eigen::Dynamic,1,Eigen::ColMajor,256> MapList;
typedef Eigen::Array<float,Eigen::Dynamic,3,Eigen::ColMajor,256> ColorList;
typedef Eigen::Array<float,Eigen::Dynamic,1,Eigen::ColMajor,256> TempVectorXf;
#endif // DEFINES_H
