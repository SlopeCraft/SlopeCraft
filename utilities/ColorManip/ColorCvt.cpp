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

#include "ColorManip.h"

#include <math.h>
#include <stdio.h>
#include <utility>

static constexpr float threshold = 1e-10f;

#define deg2rad(deg) ((deg)*M_PI / 180.0)

ARGB ComposeColor(const ARGB front, const ARGB back) noexcept {
  int red =
      (getR(front) * getA(front) + getR(back) * (255 - getA(front))) / 255;
  int green =
      (getG(front) * getA(front) + getG(back) * (255 - getA(front))) / 255;
  int blue =
      (getB(front) * getA(front) + getB(back) * (255 - getA(front))) / 255;
  return ARGB32(red, green, blue);
}

ARGB ComposeColor_background_half_transparent(const ARGB front,
                                              const ARGB back) noexcept {
  int alpha_front = getA(front);
  int alpha_back = getA(back);

  if (alpha_back <= 0) {
    return front;
  }
  if (alpha_front <= 0)
    return back;

  if (alpha_front >= 255)
    return front;

  int result_alpha = alpha_front + alpha_back - alpha_back * alpha_front / 255;

  if (result_alpha <= 0) {
    return 0;
  }

  int result_red = (getR(front) * alpha_front * (255 - alpha_back) +
                    getR(back) * alpha_back * 255) /
                   255 / result_alpha;
  int result_green = (getG(front) * alpha_front * (255 - alpha_back) +
                      getG(back) * alpha_back * 255) /
                     255 / result_alpha;
  int result_blue = (getB(front) * alpha_front * (255 - alpha_back) +
                     getB(back) * alpha_back * 255) /
                    255 / result_alpha;

  return ARGB32(result_red, result_green, result_blue, result_alpha);
}

void RGB2HSV(float r, float g, float b, float &h, float &s, float &v) noexcept {
  float K = 0.0f;

  if (g > b) {
    K = 2.0f;
  } else {
    std::swap(g, b);
    K = 4.0f;
  }

  if (r > g) {
    K = 6.0f;
  } else {
    std::swap(r, g);

    if (K == 2.0f) {
      std::swap(g, b);
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

void HSV2RGB(float H, float S, float V, float &r, float &g, float &b) noexcept {
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

void RGB2XYZ(float R, float G, float B, float &X, float &Y, float &Z) noexcept {
  X = 0.412453f * R + 0.357580f * G + 0.180423f * B;
  Y = 0.212671f * R + 0.715160f * G + 0.072169f * B;
  Z = 0.019334f * R + 0.119193f * G + 0.950227f * B;
  return;
}

inline void f(float &I) noexcept {
  if (I > 0.008856f)
    I = pow(I, 1.0f / 3.0f);
  else
    I = 7.787f * I + 16.0f / 116.0f;
  return;
}

inline void invf(float &I) noexcept {
  if (I > pow(0.008856, 1.0 / 3.0))
    I = I * I * I;
  else
    I = (I - 16.0 / 116.0) / 7.787;
  return;
}

void XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b) noexcept {
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

void Lab2XYZ(float L, float a, float b, float &X, float &Y, float &Z) noexcept {
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

inline float squeeze01(float t) noexcept {
  if (t < 0.0)
    return 0.0f;
  if (t > 1.0)
    return 1.0f;
  return t;
}

ARGB RGB2ARGB(float r, float g, float b) noexcept {
  return ARGB32(255 * squeeze01(r), 255 * squeeze01(g), 255 * squeeze01(b));
}

ARGB XYZ2ARGB(float x, float y, float z) noexcept {
  /*
[3.2404814,-1.5371516,-0.4985363;
-0.9692550,1.8759900,0.0415559;
0.0556466,-0.2040413,1.0573111]
*/
  return RGB2ARGB(3.2404814 * x - 1.5371516 * y - 0.4985363 * z,
                  -0.9692550 * x + 1.8759900 * y + 0.0415559 * z,
                  0.0556466 * x - 0.2040413 * y + 1.0573111 * z);
}

ARGB Lab2ARGB(float L, float a, float b) noexcept {
  float x, y, z;
  Lab2XYZ(L, a, b, x, y, z);
  return XYZ2ARGB(x, y, z);
}

ARGB HSV2ARGB(float H, float S, float V) noexcept {
  float r, g, b;
  HSV2RGB(H, S, V, r, g, b);
  return RGB2ARGB(r, g, b);
}
