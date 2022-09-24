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

//透明像素处理策略：B->替换为背景色；A->空气
//半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色
void SCL_EXPORT SCL_preprocessImage(ARGB *data, const uint64_t imageSize,
                                    const SCL_PureTpPixelSt pSt,
                                    const SCL_HalfTpPixelSt hSt,
                                    ARGB backGround) {
  backGround |= 0xFF000000;

  if (data == nullptr)
    return;
  if (imageSize <= 0)
    return;

  for (uint64_t i = 0; i < imageSize; i++) {
    if (getA(data[i]) == 0) { // pure transparent
      switch (pSt) {
      case SCL_PureTpPixelSt::ReplaceWithBackGround:
        data[i] = backGround;
        break;
      default:
        break;
      }
    }

    if (getA(data[i]) < 255) { //  half transparent
      switch (hSt) {
      case SCL_HalfTpPixelSt::ReplaceWithBackGround_:
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

unsigned char SCL_EXPORT SCL_maxAvailableVersion() { return 19; }

#ifndef SCL_CAPI
} //  namespace SlopeCraft
#endif
