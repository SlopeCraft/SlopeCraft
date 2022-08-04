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

#ifndef colorset_cpp
#define colorset_cpp
#include "ColorSet.h"
#include "TokiSlopeCraft.h"

const ColorSet *const TokiColor::Allowed = &TokiSlopeCraft::Allowed;
const ConstColorSet *const TokiColor::Basic = &TokiSlopeCraft::Basic;
// short TokiColor::DepthIndexEnd[4]={63,127,191,255};
std::array<uint8_t, 4> TokiColor::DepthCount = {64, 64, 64, 64};
bool TokiColor::needFindSide = false;
char TokiColor::convertAlgo = 'R';

ColorSet::ColorSet() {
  Map.setZero(256, 3);
  _RGB.setZero(256, 3);
  HSV.setZero(256, 3);
  Lab.setZero(256, 3);
  XYZ.setZero(256, 3);
}

ColorSet::ColorSet(int Num) {
  Map.setZero(Num, 3);
  assert(Num <= 256);
  _RGB.setZero(Num, 3);
  HSV.setZero(Num, 3);
  Lab.setZero(Num, 3);
  XYZ.setZero(Num, 3);
}

ConstColorSet::ConstColorSet(const float *rgbSrc) {
  for (short r = 0; r < 256; r++)
    Map(r) = 4 * (r % 64) + r / 64;

  _RGB.setZero(256, 3);
  HSV.setZero(256, 3);
  Lab.setZero(256, 3);
  XYZ.setZero(256, 3);

  for (int i = 0; i < 256 * 3; i++) {
    _RGB(i) = rgbSrc[i];
  }

  for (int row = 0; row < 256; row++) {
    const float r = _RGB(row, 0), g = _RGB(row, 1), b = _RGB(row, 2);
    RGB2HSV(r, g, b, HSV(row, 0), HSV(row, 1), HSV(row, 2));
    RGB2XYZ(r, g, b, XYZ(row, 0), XYZ(row, 1), XYZ(row, 2));
    XYZ2Lab(XYZ(row, 0), XYZ(row, 1), XYZ(row, 2), Lab(row, 0), Lab(row, 1),
            Lab(row, 2));
  }
}

bool ColorSet::ApplyAllowed(const ConstColorSet &standard, bool *MIndex) {
  TokiColor::DepthCount[0] = 0;
  TokiColor::DepthCount[1] = 0;
  TokiColor::DepthCount[2] = 0;
  TokiColor::DepthCount[3] = 0;
  short totalAllowColorCount = 0;
  for (short r = 0; r < 256; r++) {
    totalAllowColorCount += MIndex[r]; // if r%64==0, that is air, air isn't
                                       // used as color(it's transparent)
    TokiColor::DepthCount[(4 * (r % 64) + (r / 64)) % 4] += MIndex[r];
    //(4*(r%64)+(r/64))
  }
  /*
  TokiColor::DepthIndexEnd[0]=TokiColor::DepthCount[0]-1;
  TokiColor::DepthIndexEnd[1]=TokiColor::DepthIndexEnd[0]+TokiColor::DepthCount[1];
  TokiColor::DepthIndexEnd[2]=TokiColor::DepthIndexEnd[1]+TokiColor::DepthCount[2];
  TokiColor::DepthIndexEnd[3]=TokiColor::DepthIndexEnd[2]+TokiColor::DepthCount[3];
*/
  if (totalAllowColorCount <= 1) {
    std::cerr << "Too few color allowed!\n";
    _RGB.setZero(1, 3);
    HSV.setZero(1, 3);
    Lab.setZero(1, 3);
    XYZ.setZero(1, 3);
    Map.setZero(1);
    return false;
  }
  std::cerr << totalAllowColorCount << "colors allowed.\n";

  _RGB.setZero(totalAllowColorCount, 3);
  HSV.setZero(totalAllowColorCount, 3);
  Lab.setZero(totalAllowColorCount, 3);
  XYZ.setZero(totalAllowColorCount, 3);
  Map.setZero(totalAllowColorCount, 1);
  short write = 0;
  for (short readIndex = 0; readIndex <= 256; readIndex++) {

    if (write >= totalAllowColorCount)
      break;
    if (MIndex[readIndex]) {
      _RGB.row(write) = standard._RGB.row(readIndex);
      HSV.row(write) = standard.HSV.row(readIndex);
      Lab.row(write) = standard.Lab.row(readIndex);
      XYZ.row(write) = standard.XYZ.row(readIndex);
      Map(write) = standard.Map(readIndex);
      write++;
    }
  }
  // std::cout<<Map<<std::endl;
  return true;
}

void GetMap(unsigned char *Map) {
  for (short r = 0; r < 256; r++)
    Map[r] = 4 * (r % 64) + r / 64;
  return;
}

uint16_t ColorSet::colorCount() const { return _RGB.rows(); }

ARGB ComposeColor(const ARGB front, const ARGB back) {
  int red =
      (getR(front) * getA(front) + getR(back) * (255 - getA(front))) / 255;
  int green =
      (getG(front) * getA(front) + getG(back) * (255 - getA(front))) / 255;
  int blue =
      (getB(front) * getA(front) + getB(back) * (255 - getA(front))) / 255;
  return ARGB32(red, green, blue);
}

#endif
