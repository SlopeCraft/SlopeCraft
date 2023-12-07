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
#include <xsimd/xsimd.hpp>
#include "newTokiColor.hpp"

inline float square(float x) { return x * x; }

constexpr float thre = 1e-10f;

float color_diff_RGB_plus(const float r1, const float g1, const float b1,
                          const float r2, const float g2,
                          const float b2) noexcept {
  constexpr float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;

  const float SqrModSquare =
      std::sqrt((r1 * r1 + g1 * g1 + b1 * b1) * (r2 * r2 + g2 * g2 + b2 * b2));

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

  assert(!std::isnan(result));
  assert(result >= 0);

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

// using selected_arch = xsimd::default_arch;
using batch_t = xsimd::batch<float>;
constexpr size_t batch_size = batch_t::size;
void colordiff_RGB_batch(std::span<const float> r1p, std::span<const float> g1p,
                         std::span<const float> b1p,
                         std::span<const float, 3> rgb2,
                         std::span<float> dest) noexcept {
  assert(r1p.size() == g1p.size());
  assert(g1p.size() == b1p.size());
  assert(b1p.size() == dest.size());

  const size_t color_count = r1p.size();

  const size_t vec_size = color_count - color_count % batch_size;

  const float r2{rgb2[0]}, g2{rgb2[1]}, b2{rgb2[2]};

  for (size_t idx = 0; idx < vec_size; idx += batch_size) {
    batch_t r1{batch_t ::load_aligned(r1p.data() + idx)};
    batch_t g1{batch_t ::load_aligned(g1p.data() + idx)};
    batch_t b1{batch_t ::load_aligned(b1p.data() + idx)};

    auto dr = r1 - r2;
    auto dg = g1 - g2;
    auto db = b1 - b2;
    batch_t diff = dr * dr + dg * dg + db * db;
    diff.store_aligned(dest.data() + idx);
  }
  for (size_t idx = vec_size; idx < color_count; idx++) {
    const float r1 = r1p[idx], g1 = g1p[idx], b1 = b1p[idx];
    const float dr = r1 - r2;
    const float dg = g1 - g2;
    const float db = b1 - b2;
    auto diff = dr * dr + dg * dg + db * db;
    dest[idx] = diff;
  }
}

inline void assert_if_nan([[maybe_unused]] batch_t val) noexcept {
  for (size_t i = 0; i < batch_size; i++) {
    assert(!std::isnan(val.get(i)));
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

  std::fill(dest.begin(), dest.end(), NAN);

  const size_t color_count = r1p.size();
  const size_t vec_size = color_count - color_count % batch_size;
  const float r2 = c3[0], g2 = c3[1], b2 = c3[2];

  // const batch_t r2{_r2}, g2{_g2}, b2{_b2};

  // const batch_t thre_{threshold};

  const float rr_plus_gg_plus_bb_2 = (r2 * r2 + g2 * g2 + b2 * b2);
  constexpr float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;

  for (size_t i = 0; i < vec_size; i += batch_size) {
    batch_t r1{batch_t ::load_aligned(r1p.data() + i)};
    batch_t g1{batch_t ::load_aligned(g1p.data() + i)};
    batch_t b1{batch_t ::load_aligned(b1p.data() + i)};

    auto deltaR = r1 - r2;
    auto deltaG = g1 - g2;
    auto deltaB = b1 - b2;

    batch_t SqrModSquare;
    {
      const batch_t rr_plus_gg_plus_bb_1 = r1 * r1 + g1 * g1 + b1 * b1;

      SqrModSquare = rr_plus_gg_plus_bb_1 * rr_plus_gg_plus_bb_2;
      SqrModSquare = xsimd::sqrt(SqrModSquare);
    }
    assert_if_nan(SqrModSquare);

    const batch_t sigma_rgb = (r1 + g1 + b1 + r2 + g2 + b2) * float(1.0f / 3);

    const batch_t sigma_rgb_plus_thre = sigma_rgb + threshold;

    const batch_t r1_plus_r2 = r1 + r2;
    const batch_t g1_plus_g2 = g1 + g2;
    const batch_t b1_plus_b2 = b1 + b2;
    batch_t S_r, S_g, S_b;
    {
      batch_t temp_r = r1_plus_r2 / sigma_rgb_plus_thre;
      batch_t temp_g = g1_plus_g2 / sigma_rgb_plus_thre;
      batch_t temp_b = b1_plus_b2 / sigma_rgb_plus_thre;

      S_r = min(temp_r, batch_t{1.0f});
      S_g = min(temp_g, batch_t{1.0f});
      S_b = min(temp_b, batch_t{1.0f});
    }

    const batch_t sumRGBsquare = r1 * r2 + g1 * g2 + b1 * b2;

    batch_t theta;
    {
      batch_t temp1 = sumRGBsquare / (SqrModSquare + threshold);

      temp1 /= 1.01f;
      const batch_t temp2 = acos(temp1);
      // batch_t temp2 = _mm256_acos_ps__manually(temp1);
      theta = temp2 * float(2.0 / M_PI);
    }

    const batch_t OnedDeltaR = abs(deltaR) / (r1_plus_r2 * thre);
    const batch_t OnedDeltaG = abs(deltaG) / (g1_plus_g2 * thre);
    const batch_t OnedDeltaB = abs(deltaB) / (b1_plus_b2 * thre);

    const batch_t sumOnedDelta = OnedDeltaR + OnedDeltaG + OnedDeltaB + thre;

    batch_t S_tr = OnedDeltaR / sumOnedDelta * (S_r * S_r);
    batch_t S_tg = OnedDeltaG / sumOnedDelta * (S_g * S_g);
    batch_t S_tb = OnedDeltaB / sumOnedDelta * (S_b * S_b);

    batch_t S_theta = S_tr + S_tg + S_tb;

    batch_t S_ratio;
    {
      batch_t max_r = max(r1, {r2});
      batch_t max_g = max(g1, {g2});
      batch_t max_b = max(b1, {b2});
      S_ratio = max(max_r, max(max_g, max_b));
    }
    /*
     *
        const float result =
            (S_r * S_r * w_r * deltaR * deltaR + S_g * S_g * w_g * deltaG *
     deltaG + S_b * S_b * w_b * deltaB * deltaB) / (w_r + w_g + w_b) + S_theta *
     S_ratio * theta * theta;
     * */
    batch_t diff;
    {
      batch_t temp_r = S_r * S_r * deltaR * deltaR * w_r;
      batch_t temp_g = S_g * S_g * deltaG * deltaG * w_g;
      batch_t temp_b = S_b * S_b * deltaB * deltaB * w_b;
      batch_t wr_plus_wr_plus_wb{w_r + w_b + w_g};
      batch_t temp_X = (temp_r + temp_g + temp_b) / wr_plus_wr_plus_wb;

      batch_t temp_Y = S_theta * S_ratio * theta * theta;

      diff = temp_X + temp_Y;
    }
    diff.store_aligned(dest.data() + i);
  }

  for (size_t i = vec_size; i < color_count; i++) {
    dest[i] = color_diff_RGB_plus(r1p[i], g1p[i], b1p[i], r2, g2, b2);
  }
}

void colordiff_HSV_batch(std::span<const float> h1p, std::span<const float> s1p,
                         std::span<const float> v1p,
                         std::span<const float, 3> hsv2,
                         std::span<float> dest) noexcept {
  assert(h1p.size() == s1p.size());
  assert(s1p.size() == v1p.size());
  assert(v1p.size() == dest.size());

  const size_t color_count = h1p.size();
  const size_t vec_size = color_count - color_count % batch_size;

  const float h2 = hsv2[0];
  const float s2 = hsv2[1];
  const float v2 = hsv2[2];

  for (size_t idx = 0; idx < vec_size; idx += batch_size) {
    const batch_t h1 = batch_t::load_aligned(h1p.data() + idx);
    const batch_t s1 = batch_t::load_aligned(s1p.data() + idx);
    const batch_t v1 = batch_t::load_aligned(v1p.data() + idx);
    auto sv_1 = s1 * v1;
    auto sv_2 = s2 * v2;

    const auto dX = 50.0f * (cos(h1) * sv_1 - std::cos(h2) * sv_2);
    const auto dY = 50.0f * (sin(h1) * sv_1 - std::sin(h2) * sv_2);
    const auto dZ = 50.0f * (v1 - v2);

    const auto diff = dX * dX + dY * dY + dZ * dZ;
    diff.store_aligned(dest.data() + idx);
  }

  for (size_t idx = vec_size; idx < color_count; idx++) {
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

  const size_t color_count = l1p.size();
  const size_t vec_size = color_count - color_count % batch_size;

  const float L2 = c3[0];
  const float a2 = c3[1];
  const float b2 = c3[2];
  //__m256 C1_2;
  const float sqrt_C1_2 = sqrt(a2 * a2 + b2 * b2);
  const float SC_2 = square(sqrt_C1_2 * 0.045f + 1.0f);

  for (size_t i = 0; i < vec_size; i += batch_size) {
    batch_t L1 = batch_t::load_aligned(l1p.data() + i);
    batch_t a1 = batch_t::load_aligned(a1p.data() + i);
    batch_t b1 = batch_t::load_aligned(b1p.data() + i);

    batch_t deltaL_2;
    {
      batch_t Ldiff = L1 - L2;
      deltaL_2 = (Ldiff * Ldiff);
    }

    batch_t C2_2 = ((a1 * a1) + (b1 * b1));

    batch_t deltaCab_2;
    {
      batch_t temp = (sqrt_C1_2 - sqrt(C2_2));
      deltaCab_2 = (temp * temp);
    }

    batch_t deltaHab_2;
    {
      batch_t a_diff = (a1 - a2);
      batch_t b_diff = (b1 - b2);

      deltaHab_2 = ((a_diff * a_diff) + (b_diff * b_diff));
      deltaHab_2 = (deltaHab_2 - deltaCab_2);
    }

    // constexpr float SL = 1;
    //  constexpr float kL = 1;
    // constexpr float K1 = 0.045f;
    constexpr float K2 = 0.015f;

    batch_t SH_2;
    {
      batch_t temp = ((sqrt(C2_2) * K2) + 1.0f);
      SH_2 = (temp * temp);
    }

    batch_t diff;
    {
      batch_t temp_C = (deltaCab_2 / SC_2);
      batch_t temp_H = (deltaHab_2 / SH_2);
      diff = (deltaL_2 + (temp_C + temp_H));
    }

    diff.store_aligned(dest.data() + i);
  }

  {
    const float L = c3[0];
    const float a = c3[1];
    const float b = c3[2];
    const float _C1_2 = a * a + b * b;
    const float _SC_2 =
        (sqrt(_C1_2) * 0.045f + 1.0f) * (sqrt(_C1_2) * 0.045f + 1.0f);

    for (size_t i = vec_size; i < color_count; i++) {
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