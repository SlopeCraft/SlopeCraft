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

  for (int i = 0; i < color_count; i += num_float_per_m256) {
    const int offset = i;
    assert(offset + num_float_per_m256 < color_count);
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