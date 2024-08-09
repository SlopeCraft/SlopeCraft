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

#include "SCLDefines.h"
#include "SlopeCraftL.h"
#include <ColorManip/ColorManip.h>

using namespace SlopeCraft;

inline ARGB composeColor(const ARGB front, const ARGB back) {
  const ARGB red =
      (getR(front) * getA(front) + getR(back) * (255 - getA(front))) / 255;
  const ARGB green =
      (getG(front) * getA(front) + getG(back) * (255 - getA(front))) / 255;
  const ARGB blue =
      (getB(front) * getA(front) + getB(back) * (255 - getA(front))) / 255;
  return ARGB32(red, green, blue);
}

#ifndef SCL_CAPI
namespace SlopeCraft {
#endif

// 透明像素处理策略：B->替换为背景色；A->空气
// 半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色
void SCL_EXPORT SCL_preprocessImage(ARGB *data, const uint64_t imageSize,
                                    const SCL_PureTpPixelSt pSt,
                                    const SCL_HalfTpPixelSt hSt,
                                    ARGB backGround) {
  backGround |= 0xFF000000;

  if (data == nullptr) return;
  if (imageSize <= 0) return;

  for (uint64_t i = 0; i < imageSize; i++) {
    if (getA(data[i]) == 0) {  // pure transparent
      switch (pSt) {
        case SCL_PureTpPixelSt::ReplaceWithBackGround:
          data[i] = backGround;
          break;
        default:
          break;
      }
      continue;
    }

    if (getA(data[i]) < 255) {  //  half transparent
      switch (hSt) {
        case SCL_HalfTpPixelSt::ReplaceWithBackGround:
          data[i] = backGround;
          break;
        case SCL_HalfTpPixelSt::ComposeWithBackGround:
          data[i] = composeColor(data[i], backGround);
          break;
        default:
          data[i] |= 0xFF000000;
          break;
      }
    }
  }
}

SCL_EXPORT bool SCL_haveTransparentPixel(const uint32_t *ARGB32,
                                         const uint64_t imageSize) {
  for (uint64_t i = 0; i < imageSize; i++) {
    const uint32_t argb = ARGB32[i];

    if (getA(argb) != 255) {
      return true;
    }
  }

  return false;
}

SCL_gameVersion SCL_EXPORT SCL_maxAvailableVersion() {
  return SCL_gameVersion::MC21;
}

#ifndef SCL_CAPI
}  //  namespace SlopeCraft
#endif
