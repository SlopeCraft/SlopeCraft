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
#include <cmath>

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
  const float S_theta = S_tr + S_tg + S_b;

  const float S_ratio =
      std::max(std::max(r1, r2), std::max(std::max(g1, g2), std::max(b1, b2)));

  const float result =
      (S_r * S_r * w_r * deltaR * deltaR + S_g * S_g * w_g * deltaG * deltaG +
       S_b * S_b * w_b * deltaB * deltaB) /
          (w_r + w_g + w_b) +
      S_theta * S_ratio * theta * theta;

  return result;
}