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

#ifndef TOKICOLOR_H
#define TOKICOLOR_H
#include "../ColorManip/newColorSet.hpp"
#include "../ColorManip/newTokiColor.hpp"
#include "ColorSet.h"
#include "SCLDefines.h"
#include <iostream>

using TokiColor = newTokiColor<true, colorset_basic_t, colorset_allowed_t>;
//#define dispFunCalled
/*
template <bool isColorFixed, bool has_map_color, int max_color_count>
class colorset_new;

class TokiColor {
public:
  explicit TokiColor(ARGB);
  TokiColor();
  // QRgb Raw;//相当于Key
  // float c3[3];//三通道的颜色值。可能为RGB,HSV,Lab,XYZ
  Eigen::Array3f c3;
  float sideSelectivity
      [2];
//记录与result的深度值不同的两个有损优化候选色选择系数（升序排列），Depth=3时无效
  unsigned char sideResult
      [2];
//记录与result的深度值不同的两个有损优化候选色（升序排列），Depth=3时无效 static
::SlopeCraft::convertAlgo convertAlgo; unsigned char Result; //最终调色结果
  float ResultDiff;
  static bool needFindSide;
  // static short DepthIndexEnd[4];
  static std::array<uint8_t, 4> DepthCount;
  static const colorset_new<false, true, 256> *const Allowed;
  static const colorset_new<true, true, 256> *const Basic;
  unsigned char apply(ARGB);

private:
  unsigned char apply();
  unsigned char applyRGB();
  unsigned char applyRGB_plus();
  unsigned char applyHSV();
  unsigned char applyXYZ();
  unsigned char applyLab_old();
  unsigned char applyLab_new();
  void doSide(const TempVectorXf &);
};
*/

#endif // TOKICOLOR_H
