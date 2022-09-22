#include "sortColor.h"

using namespace GACvter;
using namespace SlopeCraft;

sortColor::sortColor() noexcept {}

void sortColor::calculate(ARGB rgb) noexcept {
  const float r = getR(rgb) / 255.0f, g = getG(rgb) / 255.0f,
              b = getB(rgb) / 255.0f;

  auto diffR = SlopeCraft::AllowedRGB4External(0) - r;
  // SlopeCraft::Allowed4External.col(0)-r;
  // SlopeCraft::Allowed4External.col(0)-r;
  auto diffG = SlopeCraft::AllowedRGB4External(1) - g;
  auto diffB = SlopeCraft::AllowedRGB4External(2) - b;

  TempVectorXf diff = diffR.square() + diffG.square() + diffB.square();

  int tempIdx = 0;
  for (order_t o = 0; o < OrderMax; o++) {
    // errors[o]=
    diff.minCoeff(&tempIdx);
    mapCs[o] = SlopeCraft::AllowedMapList4External()[tempIdx];
    // Converter::mapColorSrc->operator[](tempIdx);
    diff[tempIdx] = heu::internal::pinfF;
  }
}
