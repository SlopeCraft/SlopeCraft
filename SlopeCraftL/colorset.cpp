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

#include <math.h>
#include <stdio.h>

static constexpr float threshold = 1e-10f;

#define deg2rad(deg) ((deg)*M_PI / 180.0)
/*
class ColorSet_t : public colorset_new<false, true, 256> {};
class ConstColorSet_t : public colorset_new<true, true, 256> {};
*/

const colorset_new<false, true, 256> *const TokiColor::Allowed =
    &TokiSlopeCraft::Allowed;
const colorset_new<true, true, 256> *const TokiColor::Basic =
    &TokiSlopeCraft::Basic;
// short TokiColor::DepthIndexEnd[4]={63,127,191,255};
std::array<uint8_t, 4> TokiColor::DepthCount = {64, 64, 64, 64};
bool TokiColor::needFindSide = false;
::SlopeCraft::convertAlgo TokiColor::convertAlgo =
    ::SlopeCraft::convertAlgo::RGB_Better;
/*
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
*/
extern "C" {
ARGB ComposeColor(const ARGB front, const ARGB back) {
  int red =
      (getR(front) * getA(front) + getR(back) * (255 - getA(front))) / 255;
  int green =
      (getG(front) * getA(front) + getG(back) * (255 - getA(front))) / 255;
  int blue =
      (getB(front) * getA(front) + getB(back) * (255 - getA(front))) / 255;
  return ARGB32(red, green, blue);
}

inline void mySwapFloat(float &fa, float &fb) {
  int32_t &a = (int32_t &)fa;
  int32_t &b = (int32_t &)(fb);

  a = a ^ b;
  b = a ^ b;
  a = a ^ b;
}

void RGB2HSV(float r, float g, float b, float &h, float &s, float &v) {
  float K = 0.0f;

  if (g > b) {
    K = 2.0f;
  } else {
    mySwapFloat(g, b);
    K = 4.0f;
  }

  if (r > g) {
    K = 6.0f;
  } else {
    mySwapFloat(r, g);

    if (K == 2.0f) {
      mySwapFloat(g, b);
    }
  }

  const float delta = r - fmin(g, b);

  // Here we represent R,G,B as the original value before swap, and r,g,b as the
  // variable after swapping. Now R has the greatest value, and when : When R is
  // max, g=G and b=B, so g-b = G-B; K=6; while h = pi/3*((G-B)/delta+6) When G
  // is max, g=B and b=R, so g-b = B-R; K=2; while h = pi/3*((B-G)/delta+2) When
  // B is max, g=R and b=G, so g-b = R-G; K=4; while h = pi/3*((R-G)/delta+2)
  //
  // r = max(R,G,B), and min(R,G,B) is in either g or b.
  // So we have delta = r-min(g,b)
  // and h=pi/3*((g-b)/delta+K)

  h = M_PI / 3.0f * ((g - b) / (delta + threshold) + K);
  s = delta / (r + threshold);
  v = r;

  return;
}

void HSV2RGB(float H, float S, float V, float &r, float &g, float &b) {
  float C = V * S;
  float X = C * (1 - abs(int(H / deg2rad(60.0)) % 2 - 1));
  float m = V - C;
  if (H < deg2rad(60)) {
    r = C + m;
    g = X + m;
    b = 0 + m;
    return;
  }
  if (H < deg2rad(120)) {
    r = X + m;
    g = C + m;
    b = 0 + m;
    return;
  }
  if (H < deg2rad(180)) {
    r = 0 + m;
    g = C + m;
    b = X + m;
    return;
  }
  if (H < deg2rad(240)) {
    r = 0 + m;
    g = X + m;
    b = C + m;
    return;
  }
  if (H < deg2rad(300)) {
    r = X + m;
    g = 0 + m;
    b = C + m;
    return;
  }

  r = C + m;
  g = 0 + m;
  b = X + m;
  return;
}

void RGB2XYZ(float R, float G, float B, float &X, float &Y, float &Z) {
  X = 0.412453f * R + 0.357580f * G + 0.180423f * B;
  Y = 0.212671f * R + 0.715160f * G + 0.072169f * B;
  Z = 0.019334f * R + 0.119193f * G + 0.950227f * B;
  return;
}

void f(float &I) {
  if (I > 0.008856f)
    I = pow(I, 1.0f / 3.0f);
  else
    I = 7.787f * I + 16.0f / 116.0f;
  return;
}

void invf(float &I) {
  if (I > pow(0.008856, 1.0 / 3.0))
    I = I * I * I;
  else
    I = (I - 16.0 / 116.0) / 7.787;
  return;
}

void XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b) {
  X /= 0.9504f;
  f(X);
  Y /= 1.0f;
  f(Y);
  Z /= 1.0888f;
  f(Z);
  L = 116.0f * X - 16.0f;
  a = 500.0f * (X - Y);
  b = 200.0f * (Y - Z);
  return;
}

void Lab2XYZ(float L, float a, float b, float &X, float &Y, float &Z) {
  L += 16.0;
  X = 0.008620689655172 * L;
  Y = X - 0.002 * a;
  Z = Y - 0.005 * b;
  /*
[X';Y';Z']=[0.008620689655172,0,0;
0.008620689655172,-0.002,0;
0.008620689655172,-0.002,-0.005]*[L+16;a;b]
*/
  invf(X);
  invf(Y);
  invf(Z);
  X *= 0.9504f;
  Y *= 1.0f;
  Z *= 1.0888f;
}

inline float squeeze01(float t) {
  if (t < 0.0)
    return 0.0f;
  if (t > 1.0)
    return 1.0f;
  return t;
}

ARGB RGB2ARGB(float r, float g, float b) {
  return ARGB32(255 * squeeze01(r), 255 * squeeze01(g), 255 * squeeze01(b));
}

ARGB XYZ2ARGB(float x, float y, float z) {
  /*
[3.2404814,-1.5371516,-0.4985363;
-0.9692550,1.8759900,0.0415559;
0.0556466,-0.2040413,1.0573111]
*/
  return RGB2ARGB(3.2404814 * x - 1.5371516 * y - 0.4985363 * z,
                  -0.9692550 * x + 1.8759900 * y + 0.0415559 * z,
                  0.0556466 * x - 0.2040413 * y + 1.0573111 * z);
}

ARGB Lab2ARGB(float L, float a, float b) {
  float x, y, z;
  Lab2XYZ(L, a, b, x, y, z);
  return XYZ2ARGB(x, y, z);
}

ARGB HSV2ARGB(float H, float S, float V) {
  float r, g, b;
  HSV2RGB(H, S, V, r, g, b);
  return RGB2ARGB(r, g, b);
}

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
}
#endif
