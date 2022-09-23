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

#include "Colors.h"
#include "TokiSlopeCraft.h"

template <>
const colorset_allowed_t *const TokiColor::Allowed = &TokiSlopeCraft::Allowed;
template <>
const colorset_basic_t *const TokiColor::Basic = &TokiSlopeCraft::Basic;

void SCL_testHSV() {
  const float rgb[][3] = {{0, 0, 0}, {1, 1, 1}, {0.99, 0, 0}, {0, 1, 0},
                          {0, 0, 1}, {1, 1, 0}, {1, 0, 1},    {0, 1, 1}};

  const int N = sizeof(rgb) / sizeof(float[3]);

  float hsv[N][3], irgb[N][3];

  for (int r = 0; r < N; r++) {
    RGB2HSV(rgb[r][0], rgb[r][1], rgb[r][2], hsv[r][0], hsv[r][1], hsv[r][2]);

    HSV2RGB(hsv[r][0], hsv[r][1], hsv[r][2], irgb[r][0], irgb[r][1],
            irgb[r][2]);

    printf("RGB = [%f, %f, %f] , ", rgb[r][0], rgb[r][1], rgb[r][2]);
    printf("HSV = [%f, %f, %f] , ", hsv[r][0], hsv[r][1], hsv[r][2]);
    printf("iRGB = [%f, %f, %f]\n", irgb[r][0], irgb[r][1], irgb[r][2]);
  }
}