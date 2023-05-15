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

#include "ColorManip.h"
#include <cmath>
#include "newTokiColor.hpp"

inline float square(float x) { return x * x; }

constexpr float thre = 1e-10f;

float color_diff_RGB_plus(const float r1, const float g1, const float b1,
                          const float r2, const float g2,
                          const float b2) noexcept {
  constexpr float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;

  const float SqrModSquare =
      (r1 * r1 + g1 * g1 + b1 * b1) * (r2 * r2 + g2 * g2 + b2 * b2);

  const float deltaR = r1 - r2;
  const float deltaG = g1 - g2;
  const float deltaB = b1 - b2;

  const float SigmaRGB = (r1 + g1 + b1 + r2 + g2 + b2) / 3;
  const float S_r = std::min((r1 + r2) / (SigmaRGB + thre), 1.0f);

  const float S_g = std::min((g1 + g2) / (SigmaRGB + thre), 1.0f);

  const float S_b = std::min((b1 + b2) / (SigmaRGB + thre), 1.0f);

  const float sumRGBsquare = r1 * r2 + g1 * g2 + b1 * b2;

  const float theta =
      2.0f / M_PI * std::acos(sumRGBsquare / (SqrModSquare + thre) / 1.01f);

  const float OnedDeltaR = std::abs(deltaR) / (r1 + r2 + thre);
  const float OnedDeltaG = std::abs(deltaG) / (g1 + g2 + thre);
  const float OnedDeltaB = std::abs(deltaB) / (b1 + b2 + thre);

  const float sumOnedDelta = OnedDeltaR + OnedDeltaG + OnedDeltaB + thre;

  const float S_tr = OnedDeltaR / sumOnedDelta * S_r * S_r;
  const float S_tg = OnedDeltaG / sumOnedDelta * S_g * S_g;
  const float S_tb = OnedDeltaB / sumOnedDelta * S_b * S_b;
  const float S_theta = S_tr + S_tg + S_tb;

  const float S_ratio =
      std::max(std::max(r1, r2), std::max(std::max(g1, g2), std::max(b1, b2)));

  const float result =
      (S_r * S_r * w_r * deltaR * deltaR + S_g * S_g * w_g * deltaG * deltaG +
       S_b * S_b * w_b * deltaB * deltaB) /
          (w_r + w_g + w_b) +
      S_theta * S_ratio * theta * theta;

  return result;
}

float color_diff_HSV(float h1, float s1, float v1, float h2, float s2,
                     float v2) noexcept {
  const float sv_1 = s1 * v1;
  const float sv_2 = s2 * v2;

  const float dX = 50.0f * (std::cos(h1) * sv_1 - std::cos(h2) * sv_2);
  const float dY = 50.0f * (std::sin(h1) * sv_1 - std::sin(h2) * sv_2);
  const float dZ = 50.0f * (v1 - v2);

  return dX * dX + dY * dY + dZ * dZ;
}

#ifdef SC_VECTORIZE_AVX2
#include <immintrin.h>
#include <xmmintrin.h>

constexpr int num_float_per_m256 = 256 / 32;

union alignas(32) f32_i32 {
  float f32[8];
  int i32[8];
  __m256i m256i;
};

inline void take_abs_f32_8(float *p) noexcept {
  for (size_t i = 0; i < 8; i++) {
    *p = std::abs(*p);
  }
}

/**
 * This function is a candidate when the real instruction can't be used.
 */

/**
 * This function is a candidate when the real instruction can't be used.
 */
inline __m256 _mm256_acos_ps__manually(__m256 x) noexcept {
  alignas(32) float y[num_float_per_m256];

  _mm256_store_ps(y, x);

  for (int i = 0; i < num_float_per_m256; i++) {
    y[i] = std::acos(y[i]);
  }

  return _mm256_load_ps(y);
}

// #warning rua~
#endif  // SC_VECTORIZE_AVX2

void colordiff_RGB_batch(std::span<const float> r1p, std::span<const float> g1p,
                         std::span<const float> b1p,
                         std::span<const float, 3> rgb2,
                         std::span<float> dest) noexcept {
  assert(r1p.size() == g1p.size());
  assert(g1p.size() == b1p.size());
  assert(b1p.size() == dest.size());

  const int color_count = r1p.size();

#ifdef SC_VECTORIZE_AVX2
  const __m256 r2 = _mm256_set1_ps(rgb2[0]);
  const __m256 g2 = _mm256_set1_ps(rgb2[1]);
  const __m256 b2 = _mm256_set1_ps(rgb2[2]);

  for (int i = 0; i + num_float_per_m256 <= color_count;
       i += num_float_per_m256) {
    const int offset = i;
    __m256 r1 = _mm256_load_ps(r1p.data() + offset);
    __m256 g1 = _mm256_load_ps(g1p.data() + offset);
    __m256 b1 = _mm256_load_ps(b1p.data() + offset);

    __m256 dr = _mm256_sub_ps(r1, r2);
    __m256 dg = _mm256_sub_ps(g1, g2);
    __m256 db = _mm256_sub_ps(b1, b2);

    dr = _mm256_mul_ps(dr, dr);
    dg = _mm256_mul_ps(dg, dg);
    db = _mm256_mul_ps(db, db);

    _mm256_store_ps(dest.data() + offset,
                    _mm256_add_ps(dr, _mm256_add_ps(dg, db)));
  }
  const int loop_start =
      (color_count / num_float_per_m256) * num_float_per_m256;
#else
  const int loop_start = 0;
#endif
  for (int i = loop_start; i < color_count; i++) {
    dest[i] = square(r1p[i] - rgb2[0]) + square(g1p[i] - rgb2[1]) +
              square(b1p[i] - rgb2[2]);
  }
}

void colordiff_RGBplus_batch(std::span<const float> r1p,
                             std::span<const float> g1p,
                             std::span<const float> b1p,
                             std::span<const float, 3> c3,
                             std::span<float> dest) noexcept {
  assert(r1p.size() == g1p.size());
  assert(g1p.size() == b1p.size());
  assert(b1p.size() == dest.size());

  const int color_count = r1p.size();
  const float _r2 = c3[0], _g2 = c3[1], _b2 = c3[2];
#ifdef SC_VECTORIZE_AVX2

  const __m256 r2 = _mm256_set1_ps(_r2), g2 = _mm256_set1_ps(_g2),
               b2 = _mm256_set1_ps(_b2);
  const __m256 thre = _mm256_set1_ps(threshold);

  const float rr_plus_gg_plus_bb_2 = (_r2 * _r2 + _g2 * _g2 + _b2 * _b2);
  constexpr float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;

  for (int i = 0; i + num_float_per_m256 <= color_count;
       i += num_float_per_m256) {
    __m256 r1 = _mm256_load_ps(r1p.data() + i);
    __m256 g1 = _mm256_load_ps(g1p.data() + i);
    __m256 b1 = _mm256_load_ps(b1p.data() + i);

    __m256 deltaR = _mm256_sub_ps(r1, r2);
    __m256 deltaG = _mm256_sub_ps(g1, g2);
    __m256 deltaB = _mm256_sub_ps(b1, b2);

    __m256 SqrModSquare;
    {
      __m256 temp_r = _mm256_mul_ps(r1, r1);
      __m256 temp_g = _mm256_mul_ps(g1, g1);
      __m256 temp_b = _mm256_mul_ps(b1, b1);

      SqrModSquare = _mm256_add_ps(
          _mm256_add_ps(temp_r, temp_g),
          _mm256_add_ps(temp_b, _mm256_set1_ps(rr_plus_gg_plus_bb_2)));
    }

    __m256 sigma_rgb;
    {
      __m256 temp1 = _mm256_add_ps(r1, g1);
      __m256 temp2 = _mm256_add_ps(g1, _mm256_set1_ps(_r2 + _g2 + _b2));
      sigma_rgb =
          _mm256_mul_ps(_mm256_add_ps(temp1, temp2), _mm256_set1_ps(1.0f / 3));
    }
    __m256 sigma_rgb_plus_thre =
        _mm256_add_ps(sigma_rgb, _mm256_set1_ps(threshold));

    const __m256 r1_plus_r2 = _mm256_add_ps(r1, r2);
    const __m256 g1_plus_g2 = _mm256_add_ps(g1, g2);
    const __m256 b1_plus_b2 = _mm256_add_ps(b1, b2);
    __m256 S_r, S_g, S_b;
    {
      const __m256 m256_1f = _mm256_set1_ps(1.0f);
      __m256 temp_r = _mm256_div_ps(r1_plus_r2, sigma_rgb_plus_thre);
      __m256 temp_g = _mm256_div_ps(g1_plus_g2, sigma_rgb_plus_thre);
      __m256 temp_b = _mm256_div_ps(b1_plus_b2, sigma_rgb_plus_thre);

      S_r = _mm256_min_ps(temp_r, m256_1f);
      S_g = _mm256_min_ps(temp_g, m256_1f);
      S_b = _mm256_min_ps(temp_b, m256_1f);
    }

    __m256 sumRGBsquare;
    {
      __m256 r1r2 = _mm256_mul_ps(r1, r2);
      __m256 g1g2 = _mm256_mul_ps(g1, g2);
      __m256 b1b2 = _mm256_mul_ps(b1, b2);
      sumRGBsquare = _mm256_add_ps(r1r2, _mm256_add_ps(g1g2, b1b2));
    }

    __m256 theta;
    {
      __m256 temp1 =
          _mm256_div_ps(sumRGBsquare, _mm256_add_ps(SqrModSquare, thre));
      //__m256 _mm256_rsqrt_ps (__m256 a)
      /*
      __m256 temp1 = _mm256_mul_ps(
          sumRGBsquare, _mm256_rsqrt_ps(_mm256_add_ps(SqrModSquare, thre)));
          */
      temp1 = _mm256_div_ps(temp1, _mm256_set1_ps(1.01f));
      __m256 temp2;
      {
        alignas(32) float arr[8];
        _mm256_store_ps(arr, temp1);
        for (size_t i = 0; i < 8; i++) {
          arr[i] = std::acos(arr[i]);
        }
        temp2 = _mm256_load_ps(arr);
      }
      //__m256 temp2 = _mm256_acos_ps__manually(temp1);
      theta = _mm256_mul_ps(temp2, _mm256_set1_ps(2.0f / M_PI));
    }

    alignas(32) float arr[8];
    __m256 OnedDeltaR =
        _mm256_div_ps((_mm256_store_ps(arr, deltaR), take_abs_f32_8(arr),
                       _mm256_load_ps(arr)),
                      _mm256_add_ps(r1_plus_r2, thre));
    __m256 OnedDeltaG =
        _mm256_div_ps((_mm256_store_ps(arr, deltaG), take_abs_f32_8(arr),
                       _mm256_load_ps(arr)),
                      _mm256_add_ps(g1_plus_g2, thre));
    __m256 OnedDeltaB =
        _mm256_div_ps((_mm256_store_ps(arr, deltaB), take_abs_f32_8(arr),
                       _mm256_load_ps(arr)),
                      _mm256_add_ps(b1_plus_b2, thre));

    __m256 sumOnedDelta = _mm256_add_ps(_mm256_add_ps(OnedDeltaR, OnedDeltaG),
                                        _mm256_add_ps(OnedDeltaB, thre));

    __m256 S_tr = _mm256_div_ps(
        OnedDeltaR, _mm256_mul_ps(sumOnedDelta, _mm256_mul_ps(S_r, S_r)));
    __m256 S_tg = _mm256_div_ps(
        OnedDeltaG, _mm256_mul_ps(sumOnedDelta, _mm256_mul_ps(S_g, S_g)));
    __m256 S_tb = _mm256_div_ps(
        OnedDeltaB, _mm256_mul_ps(sumOnedDelta, _mm256_mul_ps(S_b, S_b)));

    __m256 S_theta = _mm256_add_ps(S_tr, _mm256_add_ps(S_tg, S_tb));

    __m256 S_ratio;
    {
      __m256 max_r = _mm256_max_ps(r1, r2);
      __m256 max_g = _mm256_max_ps(g1, g2);
      __m256 max_b = _mm256_max_ps(b1, b2);
      S_ratio = _mm256_max_ps(max_r, _mm256_max_ps(max_g, max_b));
    }

    __m256 diff;
    {
      __m256 temp_r = _mm256_mul_ps(
          _mm256_mul_ps(_mm256_mul_ps(S_r, S_r), _mm256_mul_ps(deltaR, deltaR)),
          _mm256_set1_ps(w_r));
      __m256 temp_g = _mm256_mul_ps(
          _mm256_mul_ps(_mm256_mul_ps(S_g, S_g), _mm256_mul_ps(deltaG, deltaG)),
          _mm256_set1_ps(w_g));
      __m256 temp_b = _mm256_mul_ps(
          _mm256_mul_ps(_mm256_mul_ps(S_b, S_b), _mm256_mul_ps(deltaB, deltaB)),
          _mm256_set1_ps(w_b));
      __m256 wr_plus_wr_plus_wb = _mm256_set1_ps(w_r + w_b + w_g);
      __m256 temp_X =
          _mm256_div_ps(_mm256_add_ps(_mm256_add_ps(temp_r, temp_g), temp_b),
                        wr_plus_wr_plus_wb);

      __m256 temp_Y = _mm256_mul_ps(_mm256_mul_ps(S_theta, S_ratio),
                                    _mm256_mul_ps(theta, theta));

      diff = _mm256_add_ps(temp_X, temp_Y);
    }

    _mm256_store_ps(dest.data() + i, diff);
  }

  const int loop_start =
      (color_count / num_float_per_m256) * num_float_per_m256;
#else
  const int loop_start = 0;
#endif

  for (int i = loop_start; i < color_count; i++) {
    dest[i] = color_diff_RGB_plus(r1p[i], g1p[i], b1p[i], _r2, _g2, _b2);
  }
}

void colordiff_HSV_batch(std::span<const float> h1p, std::span<const float> s1p,
                         std::span<const float> v1p,
                         std::span<const float, 3> hsv2,
                         std::span<float> dest) noexcept {
  assert(h1p.size() == s1p.size());
  assert(s1p.size() == v1p.size());
  assert(v1p.size() == dest.size());

  const int color_count = h1p.size();

  for (int idx = 0; idx < color_count; idx++) {
    const float diff =
        color_diff_HSV(hsv2[0], hsv2[1], hsv2[2], h1p[idx], s1p[idx], v1p[idx]);
    dest[idx] = diff;
  }
}

void colordiff_Lab94_batch(std::span<const float> l1p,
                           std::span<const float> a1p,
                           std::span<const float> b1p,
                           std::span<const float, 3> lab2,
                           std::span<float> dest) noexcept {
  const auto &c3 = lab2;

  assert(l1p.size() == a1p.size());
  assert(a1p.size() == b1p.size());
  assert(b1p.size() == dest.size());

  const int color_count = l1p.size();

#ifdef SC_VECTORIZE_AVX2
  const __m256 L2 = _mm256_set1_ps(c3[0]);
  const __m256 a2 = _mm256_set1_ps(c3[1]);
  const __m256 b2 = _mm256_set1_ps(c3[2]);
  //__m256 C1_2;
  __m256 SC_2, sqrt_C1_2;
  {
    // float L = c3[0];
    float a = c3[1];
    float b = c3[2];
    float _C1_2 = a * a + b * b;
    float _SC_2 = (sqrt(_C1_2) * 0.045f + 1.0f) * (sqrt(_C1_2) * 0.045f + 1.0f);

    // C1_2 = _mm256_set1_ps(_C1_2);
    SC_2 = _mm256_set1_ps(_SC_2);
    sqrt_C1_2 = _mm256_set1_ps(sqrt(_C1_2));
  }

  for (int i = 0; i + num_float_per_m256 <= color_count;
       i += num_float_per_m256) {
    __m256 L1 = _mm256_load_ps(l1p.data() + i);
    __m256 a1 = _mm256_load_ps(a1p.data() + i);
    __m256 b1 = _mm256_load_ps(b1p.data() + i);
    //      auto deltaL_2 = (Allowed->lab(0) - L).square();

    __m256 deltaL_2;
    {
      __m256 Ldiff = _mm256_sub_ps(L1, L2);
      deltaL_2 = _mm256_mul_ps(Ldiff, Ldiff);
    }

    __m256 C2_2 = _mm256_add_ps(_mm256_mul_ps(a1, a1), _mm256_mul_ps(b1, b1));

    __m256 deltaCab_2;
    {
      __m256 temp = _mm256_sub_ps(sqrt_C1_2, _mm256_sqrt_ps(C2_2));
      deltaCab_2 = _mm256_mul_ps(temp, temp);
    }

    __m256 deltaHab_2;
    {
      __m256 a_diff = _mm256_sub_ps(a1, a2);
      __m256 b_diff = _mm256_sub_ps(b1, b2);

      deltaHab_2 = _mm256_add_ps(_mm256_mul_ps(a_diff, a_diff),
                                 _mm256_mul_ps(b_diff, b_diff));
      deltaHab_2 = _mm256_sub_ps(deltaHab_2, deltaCab_2);
    }

    // constexpr float SL = 1;
    //  constexpr float kL = 1;
    // constexpr float K1 = 0.045f;
    constexpr float K2 = 0.015f;

    __m256 SH_2;
    {
      __m256 temp =
          _mm256_add_ps(_mm256_mul_ps(_mm256_sqrt_ps(C2_2), _mm256_set1_ps(K2)),
                        _mm256_set1_ps(1.0f));
      SH_2 = _mm256_mul_ps(temp, temp);
    }

    __m256 diff;
    {
      __m256 temp_C = _mm256_div_ps(deltaCab_2, SC_2);
      __m256 temp_H = _mm256_div_ps(deltaHab_2, SH_2);
      diff = _mm256_add_ps(deltaL_2, _mm256_add_ps(temp_C, temp_H));
    }

    _mm256_store_ps(dest.data() + i, diff);
  }

  const int loop_start =
      (color_count / num_float_per_m256) * num_float_per_m256;
#else
  const int loop_start = 0;

#endif

  {
    const float L = c3[0];
    const float a = c3[1];
    const float b = c3[2];
    const float _C1_2 = a * a + b * b;
    const float _SC_2 =
        (sqrt(_C1_2) * 0.045f + 1.0f) * (sqrt(_C1_2) * 0.045f + 1.0f);

    for (int i = loop_start; i < color_count; i++) {
      // auto deltaL_2 = (Allowed->lab(0) - L).square();

      const float deltaL_2 = (l1p[i] - L) * (l1p[i] - L);

      const float C2_2 = a1p[i] * a1p[i] + b1p[i] * b1p[i];
      float deltaCab_2;
      {
        float temp = sqrt(_C1_2) - sqrt(C2_2);
        deltaCab_2 = temp * temp;
      }

      float deltaHab_2;
      {
        float diff_a = a1p[i] - a;
        float diff_b = b1p[i] - b;
        deltaHab_2 = diff_a * diff_a + diff_b * diff_b;
      }

      float SH_2;
      {
        float temp = sqrt(C2_2) * 0.015f + 1.0f;
        SH_2 = temp * temp;
      }
      // delete &SH_2;
      dest[i] = deltaL_2 + deltaCab_2 / _SC_2 + deltaHab_2 / SH_2;
    }
  }
}