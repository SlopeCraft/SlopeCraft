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

#ifndef NEWTOKICOLOR_HPP
#define NEWTOKICOLOR_HPP

// #include "../../SlopeCraftL/SlopeCraftL.h"
#include "ColorManip.h"
#include "ColorManip/ColorManip.h"
#include "colorset_maptical.hpp"
#include "colorset_optical.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <functional>

#include "../SC_GlobalEnums.h"

#ifdef SC_VECTORIZE_AVX2
#include <immintrin.h>
#include <xmmintrin.h>

constexpr int num_float_per_m256 = 256 / 32;

union alignas(32) f32_i32 {
  float f32[8];
  int i32[8];
  __m256i m256i;
};

/**
 * This function is a candidate when the real instruction can't be used.
 */
template <typename = void> inline __m256 _mm256_abs_ps(__m256 x) noexcept {
  f32_i32 t;

  static_assert(sizeof(__m256i) == 32);

  _mm256_store_ps(t.f32, x);

  t.m256i = _mm256_and_si256(t.m256i, _mm256_set1_epi32(1UL << 31));

  return _mm256_load_ps(t.f32);
}

/**
 * This function is a candidate when the real instruction can't be used.
 */
template <typename = void> inline __m256 _mm256_acos_ps(__m256 x) noexcept {
  alignas(32) float y[num_float_per_m256];

  _mm256_store_ps(y, x);

  for (int i = 0; i < 8; i++) {
    y[i] = std::acos(y[i]);
  }

  return _mm256_load_ps(y);
}

// #warning rua~
#endif // SC_VECTORIZE_AVX2

// using Eigen::Dynamic;
namespace {
constexpr float threshold = 1e-10f;

} // namespace
struct convert_unit {
  explicit convert_unit(ARGB _a, ::SCL_convertAlgo _c) : _ARGB(_a), algo(_c) {}
  ARGB _ARGB;
  ::SCL_convertAlgo algo;

  inline bool operator==(const convert_unit another) const noexcept {
    return (_ARGB == another._ARGB) && (algo == another.algo);
  }

  inline Eigen::Array3f to_c3() const noexcept {

    Eigen::Array3f c3;
    const ::ARGB rawColor = this->_ARGB;
    switch (this->algo) {
    case ::SCL_convertAlgo::RGB:
    case ::SCL_convertAlgo::RGB_Better:
    case ::SCL_convertAlgo::gaCvter:
      c3[0] = std::max(getR(rawColor) / 255.0f, threshold);
      c3[1] = std::max(getG(rawColor) / 255.0f, threshold);
      c3[2] = std::max(getB(rawColor) / 255.0f, threshold);
      break;

    case ::SCL_convertAlgo::HSV:
      RGB2HSV(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
              getB(rawColor) / 255.0f, c3[0], c3[1], c3[2]);
      break;
    case ::SCL_convertAlgo::Lab94:
    case ::SCL_convertAlgo::Lab00:
      float X, Y, Z;
      RGB2XYZ(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
              getB(rawColor) / 255.0f, X, Y, Z);
      XYZ2Lab(X, Y, Z, c3[0], c3[1], c3[2]);
      break;
    default:
      RGB2XYZ(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
              getB(rawColor) / 255.0f, c3[0], c3[1], c3[2]);
      break;
    }
    return c3;
  }
};

struct hash_cvt_unit {
public:
  inline size_t operator()(const convert_unit cu) const noexcept {
    return std::hash<uint32_t>()(cu._ARGB) ^
           std::hash<uint8_t>()(uint8_t(cu.algo));
  }
};

template <bool is_not_optical, class basic_t, class allowed_t>
class newTokiColor
    : public ::std::conditional_t<is_not_optical, newtokicolor_base_maptical,
                                  newtokicolor_base_optical> {

public:
  using Base_t =
      ::std::conditional_t<is_not_optical, newtokicolor_base_maptical,
                           newtokicolor_base_optical>;
  using TempVectorXf_t = typename Base_t::TempVectorXf_t;
  using result_t = typename Base_t::result_t;

  // Eigen::Array3f c3; //   color in some colorspace
  float ResultDiff; // color diff for the result

  // These two members must be defined by caller
  static const basic_t *const Basic;
  static const allowed_t *const Allowed;

public:
  explicit newTokiColor() {
    if constexpr (is_not_optical) {
      this->Result = 0;
    } else {
      this->result_color_id = colorset_optical_allowed::invalid_color_id;
    }
  }

  auto compute(convert_unit cu) noexcept {
    if (getA(cu._ARGB) == 0) {
      if constexpr (is_not_optical) {
        this->Result = 0;
        return this->Result;
      } else {
        static_assert(is_not_optical, "Not implemented here.");

        return 0;
      }
    }

    if (this->is_result_computed()) {
      if constexpr (is_not_optical) {
        return this->Result;
      } else {
        return this->result_color_id;
      }
    }

    const Eigen::Array3f c3 = cu.to_c3();

    switch (cu.algo) {
    case ::SCL_convertAlgo::RGB:
      return applyRGB(c3);
    case ::SCL_convertAlgo::RGB_Better:
      return applyRGB_plus(c3);
    case ::SCL_convertAlgo::HSV:
      return applyHSV(c3);
    case ::SCL_convertAlgo::Lab94:
      return applyLab94(c3);
    case ::SCL_convertAlgo::Lab00:
      return applyLab00(c3);
    case ::SCL_convertAlgo::XYZ:
      return applyXYZ(c3);

    default:
      exit(1);
      return result_t(0);
    }
  }

  inline auto color_id() const noexcept {
    if constexpr (is_not_optical) {
      return this->Result;
    } else {
      return this->result_color_id;
    }
  }

private:
  auto find_result(const TempVectorXf_t &diff) noexcept {
    int tempidx = 0;
    this->ResultDiff = diff.minCoeff(&tempidx);

    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempidx);
      if (Base_t::needFindSide)
        doSide(diff);

      return this->Result;
    } else {
      this->result_color_id = Allowed->color_id(tempidx);
      return this->color_id();
    }
  }

  template <typename = void> void doSide(const TempVectorXf_t &Diff) {
    static_assert(is_not_optical);

    int tempIndex = 0;
    // Diff.array()+=10.0;ResultDiff+=10.0;
    this->sideSelectivity[0] = 1e35f;
    this->sideResult[0] = 0;
    this->sideSelectivity[1] = 1e35f;
    this->sideResult[1] = 0;

    // using Base_t::DepthCount;
    // using Base_t::needFindSide;

    if (!Base_t::needFindSide)
      return;
    // qDebug("开始doSide");
    // qDebug()<<"size(Diff)=["<<Diff.rows()<<','<<Diff.cols()<<']';
    // qDebug()<<"DepthCount="<<(short)DepthCount[0]<<;
    // qDebug()<<"DepthCount=["<<(short)DepthCount[0]<<','<<(short)DepthCount[1]<<','<<(short)DepthCount[2]<<','<<(short)DepthCount[3]<<']';
    // qDebug()<<"DepthIndex=["<<DepthIndexEnd[0]<<','<<DepthIndexEnd[1]<<','<<DepthIndexEnd[2]<<','<<DepthIndexEnd[3]<<']';
    switch (this->Result % 4) {
    case 3:
      return;
    case 0: // 1,2
      if (Base_t::DepthCount[1]) {
        this->sideSelectivity[0] =
            Diff.segment(Base_t::DepthCount[0], Base_t::DepthCount[1])
                .minCoeff(&tempIndex);
        this->sideResult[0] = Allowed->Map(Base_t::DepthCount[0] + tempIndex);
      }
      if (Base_t::DepthCount[2]) {
        this->sideSelectivity[1] =
            Diff.segment(Base_t::DepthCount[0] + Base_t::DepthCount[1],
                         Base_t::DepthCount[2])
                .minCoeff(&tempIndex);
        this->sideResult[1] = Allowed->Map(Base_t::DepthCount[0] +
                                           Base_t::DepthCount[1] + tempIndex);
      }
      break;
    case 1: // 0,2
      if (Base_t::DepthCount[0]) {
        this->sideSelectivity[0] =
            Diff.segment(0, Base_t::DepthCount[0]).minCoeff(&tempIndex);
        this->sideResult[0] = Allowed->Map(0 + tempIndex);
      }
      if (Base_t::DepthCount[2]) {
        this->sideSelectivity[1] =
            Diff.segment(Base_t::DepthCount[0] + Base_t::DepthCount[1],
                         Base_t::DepthCount[2])
                .minCoeff(&tempIndex);
        this->sideResult[1] = Allowed->Map(Base_t::DepthCount[0] +
                                           Base_t::DepthCount[1] + tempIndex);
      }
      break;
    case 2: // 0,1
      if (Base_t::DepthCount[0]) {
        this->sideSelectivity[0] =
            Diff.segment(0, Base_t::DepthCount[0]).minCoeff(&tempIndex);
        this->sideResult[0] = Allowed->Map(0 + tempIndex);
      }
      if (Base_t::DepthCount[1]) {
        this->sideSelectivity[1] =
            Diff.segment(Base_t::DepthCount[0], Base_t::DepthCount[1])
                .minCoeff(&tempIndex);
        this->sideResult[1] = Allowed->Map(Base_t::DepthCount[0] + tempIndex);
      }
      break;
    }
    // sideSelectivity[0]-=1.0;sideSelectivity[1]-=1.0;
    // sideSelectivity[0]*=100.0;sideSelectivity[1]*=100.0;
    /*
    qDebug()<<"side[0]=["<<sideResult[0]<<','<<sideSelectivity[0]<<']';
    qDebug()<<"side[1]=["<<sideResult[1]<<','<<sideSelectivity[1]<<']';
    qDebug()<<"ResultDiff="<<ResultDiff;*/
    return;
  }

  auto applyRGB(const Eigen::Array3f &c3) noexcept {
#ifdef SC_VECTORIZE_AVX2
    TempVectorXf_t Diff(Allowed->color_count(), 1);

    __m256 r2 = _mm256_set1_ps(c3[0]);
    __m256 g2 = _mm256_set1_ps(c3[1]);
    __m256 b2 = _mm256_set1_ps(c3[2]);

    //__m256 *const destp = (__m256 *)Diff.data();

    for (int i = 0; i * num_float_per_m256 < Allowed->color_count(); i++) {

      __m256 r1 = _mm256_load_ps(Allowed->rgb_data(0) + i * num_float_per_m256);
      __m256 g1 = _mm256_load_ps(Allowed->rgb_data(1) + i * num_float_per_m256);
      __m256 b1 = _mm256_load_ps(Allowed->rgb_data(2) + i * num_float_per_m256);

      __m256 dr = _mm256_sub_ps(r1, r2);
      __m256 dg = _mm256_sub_ps(g1, g2);
      __m256 db = _mm256_sub_ps(b1, b2);

      dr = _mm256_mul_ps(dr, dr);
      dg = _mm256_mul_ps(dg, dg);
      db = _mm256_mul_ps(db, db);

      _mm256_store_ps(Diff.data() + i * num_float_per_m256,
                      _mm256_add_ps(dr, _mm256_add_ps(dg, db)));
      // destp[i] = ;
    }

    for (int i =
             (Allowed->color_count() / num_float_per_m256) * num_float_per_m256;
         i < Allowed->color_count(); i++) {
      Diff(i) = (Allowed->RGB(i, 0) - c3[0]) * (Allowed->RGB(i, 0) - c3[0]) +
                (Allowed->RGB(i, 1) - c3[1]) * (Allowed->RGB(i, 1) - c3[1]) +
                (Allowed->RGB(i, 2) - c3[2]) * (Allowed->RGB(i, 2) - c3[2]);
    }
#else
    auto Diff0_2 = (Allowed->rgb(0) - c3[0]).square();
    auto Diff1_2 = (Allowed->rgb(1) - c3[1]).square();
    auto Diff2_2 = (Allowed->rgb(2) - c3[2]).square();
    TempVectorXf_t Diff = Diff0_2 + Diff1_2 + Diff2_2;
#endif

    // Data.CurrentColor-=allowedColors;

    return find_result(Diff);
  }

  auto applyRGB_plus(const Eigen::Array3f &c3) noexcept {
    // const ColorList &allowedColors = Allowed->_RGB;
    constexpr float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;
#ifdef SC_VECTORIZE_AVX2

    TempVectorXf_t dist(Allowed->color_count(), 1);

    const float _r2 = c3[0], _g2 = c3[1], _b2 = c3[2];

    const __m256 r2 = _mm256_set1_ps(_r2), g2 = _mm256_set1_ps(_g2),
                 b2 = _mm256_set1_ps(_b2);
    const __m256 thre = _mm256_set1_ps(threshold);

    const float rr_plus_gg_plus_bb_2 = (_r2 * _r2 + _g2 * _g2 + _b2 * _b2);
    int i;
    for (i = 0; i < Allowed->color_count(); i += num_float_per_m256) {
      __m256 r1 = _mm256_load_ps(Allowed->rgb_data(0) + i);
      __m256 g1 = _mm256_load_ps(Allowed->rgb_data(1) + i);
      __m256 b1 = _mm256_load_ps(Allowed->rgb_data(2) + i);

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
        sigma_rgb = _mm256_mul_ps(_mm256_add_ps(temp1, temp2),
                                  _mm256_set1_ps(1.0f / 3));
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
        temp1 = _mm256_div_ps(temp1, _mm256_set1_ps(1.01f));

        __m256 temp2 = _mm256_acos_ps(temp1);
        theta = _mm256_mul_ps(temp2, _mm256_set1_ps(2.0f / M_PI));
      }

      __m256 OnedDeltaR =
          _mm256_div_ps(_mm256_abs_ps(deltaR), _mm256_add_ps(r1_plus_r2, thre));
      __m256 OnedDeltaG =
          _mm256_div_ps(_mm256_abs_ps(deltaG), _mm256_add_ps(g1_plus_g2, thre));
      __m256 OnedDeltaB =
          _mm256_div_ps(_mm256_abs_ps(deltaB), _mm256_add_ps(b1_plus_b2, thre));

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
        __m256 temp_r =
            _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(S_r, S_r),
                                        _mm256_mul_ps(deltaR, deltaR)),
                          _mm256_set1_ps(w_r));
        __m256 temp_g =
            _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(S_g, S_g),
                                        _mm256_mul_ps(deltaG, deltaG)),
                          _mm256_set1_ps(w_g));
        __m256 temp_b =
            _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(S_b, S_b),
                                        _mm256_mul_ps(deltaB, deltaB)),
                          _mm256_set1_ps(w_b));
        __m256 wr_plus_wr_plus_wb = _mm256_set1_ps(w_r + w_b + w_g);
        __m256 temp_X =
            _mm256_div_ps(_mm256_add_ps(_mm256_add_ps(temp_r, temp_g), temp_b),
                          wr_plus_wr_plus_wb);

        __m256 temp_Y = _mm256_mul_ps(_mm256_mul_ps(S_theta, S_ratio),
                                      _mm256_mul_ps(theta, theta));

        diff = _mm256_add_ps(temp_X, temp_Y);
      }

      _mm256_store_ps(dist.data() + i, diff);
    }

    for (; i < Allowed->color_count(); i++) {
      dist(i) = color_diff_RGB_plus(Allowed->RGB(i, 0), Allowed->RGB(i, 1),
                                    Allowed->RGB(i, 2), _r2, _g2, _b2);
    }

#else
    float R = c3[0];
    float g = c3[1];
    float b = c3[2];
    auto SqrModSquare = ((R * R + g * g + b * b) *
                         (Allowed->rgb(0).square() + Allowed->rgb(1).square() +
                          Allowed->rgb(2).square()))
                            .sqrt();
    auto deltaR = (R - Allowed->rgb(0));
    auto deltaG = (g - Allowed->rgb(1));
    auto deltaB = (b - Allowed->rgb(2));
    auto SigmaRGB =
        (R + g + b + Allowed->rgb(0) + Allowed->rgb(1) + Allowed->rgb(2)) /
        3.0f;
    auto S_r =
        ((Allowed->rgb(0) + R) < SigmaRGB)
            .select((Allowed->rgb(0) + R) / (SigmaRGB + threshold), 1.0f);
    auto S_g =
        ((Allowed->rgb(1) + g) < SigmaRGB)
            .select((Allowed->rgb(1) + g) / (SigmaRGB + threshold), 1.0f);
    auto S_b =
        ((Allowed->rgb(2) + b) < SigmaRGB)
            .select((Allowed->rgb(2) + b) / (SigmaRGB + threshold), 1.0f);
    auto sumRGBsquare =
        R * Allowed->rgb(0) + g * Allowed->rgb(1) + b * Allowed->rgb(2);
    auto theta =
        2.0 / M_PI * (sumRGBsquare / (SqrModSquare + threshold) / 1.01f).acos();
    auto OnedDeltaR = deltaR.abs() / (R + Allowed->rgb(0) + threshold);
    auto OnedDeltaG = deltaG.abs() / (g + Allowed->rgb(1) + threshold);
    auto OnedDeltaB = deltaB.abs() / (b + Allowed->rgb(2) + threshold);
    auto sumOnedDelta = OnedDeltaR + OnedDeltaG + OnedDeltaB + threshold;
    auto S_tr = OnedDeltaR / sumOnedDelta * S_r.square();
    auto S_tg = OnedDeltaG / sumOnedDelta * S_g.square();
    auto S_tb = OnedDeltaB / sumOnedDelta * S_b.square();
    auto S_theta = S_tr + S_tg + S_tb;

    auto Rmax = Allowed->rgb(0)
                    .max(Allowed->rgb(1))
                    .max(Allowed->rgb(2))
                    .max(threshold);
    // auto Rmax = allowedColors.rowwise().maxCoeff();
    auto S_ratio = Rmax.max(std::max(R, std::max(g, b)));

    TempVectorXf_t dist =
        (S_r.square() * w_r * deltaR.square() +
         S_g.square() * w_g * deltaG.square() +
         S_b.square() * w_b * deltaB.square()) /
            (w_r + w_g + w_b) +
        S_theta * S_ratio * theta.square(); //+S_theta*S_ratio*theta.square()
#endif
    return find_result(dist);
  }

  auto applyHSV(const Eigen::Array3f &c3) noexcept {

    // const ColorList &allowedColors = Allowed->HSV;

    auto S_times_V = Allowed->hsv(1) * Allowed->hsv(2);
    const float s_times_v = c3[1] * c3[2];
    auto deltaX = 50.0f * (Allowed->hsv(0).cos() * S_times_V -
                           s_times_v * std::cos(c3[0]));
    auto deltaY = 50.0f * (Allowed->hsv(0).sin() * S_times_V -
                           s_times_v * std::sin(c3[0]));
    auto deltaZ = 50.0f * (Allowed->hsv(2) - c3[2]);
    TempVectorXf_t Diff = deltaX.square() + deltaY.square() + deltaZ.square();

    return find_result(Diff);
  }

  auto applyXYZ(const Eigen::Array3f &c3) noexcept {
#ifdef SC_VECTORIZE_AVX2
    TempVectorXf_t Diff(Allowed->color_count(), 1);
    // const __m256 *const x1p = (const __m256 *)Allowed->xyz_data(0);
    // const __m256 *const y1p = (const __m256 *)Allowed->xyz_data(1);
    // const __m256 *const z1p = (const __m256 *)Allowed->xyz_data(2);

    __m256 x2 = _mm256_set1_ps(c3[0]);
    __m256 y2 = _mm256_set1_ps(c3[1]);
    __m256 z2 = _mm256_set1_ps(c3[2]);

    //__m256 *const destp = (__m256 *)Diff.data();

    for (int i = 0; i * num_float_per_m256 < Allowed->color_count(); i++) {
      __m256 x1 = _mm256_load_ps(Allowed->xyz_data(0) + i * num_float_per_m256);
      __m256 y1 = _mm256_load_ps(Allowed->xyz_data(1) + i * num_float_per_m256);
      __m256 z1 = _mm256_load_ps(Allowed->xyz_data(2) + i * num_float_per_m256);

      __m256 dx = _mm256_sub_ps(x1, x2);
      __m256 dy = _mm256_sub_ps(y1, y2);
      __m256 dz = _mm256_sub_ps(z1, z2);

      dx = _mm256_mul_ps(dx, dx);
      dy = _mm256_mul_ps(dy, dy);
      dz = _mm256_mul_ps(dz, dz);

      _mm256_store_ps(Diff.data() + i * num_float_per_m256,
                      _mm256_add_ps(dx, _mm256_add_ps(dy, dz)));
      // destp[i] = _mm256_add_ps(dx, _mm256_add_ps(dy, dz));
    }

    for (int i =
             (Allowed->color_count() / num_float_per_m256) * num_float_per_m256;
         i < Allowed->color_count(); i++) {
      Diff(i) = (Allowed->XYZ(i, 0) - c3[0]) * (Allowed->XYZ(i, 0) - c3[0]) +
                (Allowed->XYZ(i, 1) - c3[1]) * (Allowed->XYZ(i, 1) - c3[1]) +
                (Allowed->XYZ(i, 2) - c3[2]) * (Allowed->XYZ(i, 2) - c3[2]);
    }
#else
    auto Diff0_2 = (Allowed->xyz(0) - c3[0]).square();
    auto Diff1_2 = (Allowed->xyz(1) - c3[1]).square();
    auto Diff2_2 = (Allowed->xyz(2) - c3[2]).square();

    TempVectorXf_t Diff = Diff0_2 + Diff1_2 + Diff2_2;
#endif
    // Data.CurrentColor-=allowedColors;
    return find_result(Diff);
  }

  auto applyLab94(const Eigen::Array3f &c3) noexcept {
#ifdef SC_VECTORIZE_AVX2
    TempVectorXf_t Diff(Allowed->color_count(), 1);

    __m256 L2 = _mm256_set1_ps(c3[0]);
    __m256 a2 = _mm256_set1_ps(c3[1]);
    __m256 b2 = _mm256_set1_ps(c3[2]);
    __m256 C1_2, SC_2, sqrt_C1_2;
    {
      float L = c3[0];
      float a = c3[1];
      float b = c3[2];
      float _C1_2 = a * a + b * b;
      float _SC_2 =
          (sqrt(_C1_2) * 0.045f + 1.0f) * (sqrt(_C1_2) * 0.045f + 1.0f);

      C1_2 = _mm256_set1_ps(_C1_2);
      SC_2 = _mm256_set1_ps(_SC_2);
      sqrt_C1_2 = _mm256_set1_ps(sqrt(_C1_2));
    }

    int i;
    for (i = 0; i < Allowed->color_count(); i += num_float_per_m256) {

      __m256 L1 = _mm256_load_ps(Allowed->lab_data(0) + i);
      __m256 a1 = _mm256_load_ps(Allowed->lab_data(1) + i);
      __m256 b1 = _mm256_load_ps(Allowed->lab_data(2) + i);
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

      constexpr float SL = 1, kL = 1;
      constexpr float K1 = 0.045f;
      constexpr float K2 = 0.015f;

      __m256 SH_2;
      {
        __m256 temp = _mm256_add_ps(
            _mm256_mul_ps(_mm256_sqrt_ps(C2_2), _mm256_set1_ps(K2)),
            _mm256_set1_ps(1.0f));
        SH_2 = _mm256_mul_ps(temp, temp);
      }

      __m256 diff;
      {
        __m256 temp_C = _mm256_div_ps(deltaCab_2, SC_2);
        __m256 temp_H = _mm256_div_ps(deltaHab_2, SH_2);
        diff = _mm256_add_ps(deltaL_2, _mm256_add_ps(temp_C, temp_H));
      }

      _mm256_store_ps(Diff.data() + i, diff);
    }

    const float L = c3[0];
    const float a = c3[1];
    const float b = c3[2];
    float _C1_2 = a * a + b * b;
    float _SC_2 = (sqrt(_C1_2) * 0.045f + 1.0f) * (sqrt(_C1_2) * 0.045f + 1.0f);

    for (; i < Allowed->color_count(); i++) {
      // auto deltaL_2 = (Allowed->lab(0) - L).square();

      const float deltaL_2 =
          (Allowed->Lab(i, 0) - L) * (Allowed->Lab(i, 0) - L);

      const float C2_2 = Allowed->Lab(i, 1) * Allowed->Lab(i, 1) +
                         Allowed->Lab(i, 2) * Allowed->Lab(i, 2);
      float deltaCab_2;
      {
        float temp = sqrt(_C1_2) - sqrt(C2_2);
        deltaCab_2 = temp * temp;
      }

      float deltaHab_2;
      {
        float diff_a = Allowed->Lab(i, 1) - a;
        float diff_b = Allowed->Lab(i, 2) - b;
        deltaHab_2 = diff_a * diff_a + diff_b * diff_b;
      }

      float SH_2;
      {
        float temp = sqrt(C2_2) * 0.015f + 1.0f;
        SH_2 = temp * temp;
      }
      // delete &SH_2;
      Diff(i) = deltaL_2 + deltaCab_2 / _SC_2 + deltaHab_2 / SH_2;
    }

#else
    float L = c3[0];
    float a = c3[1];
    float b = c3[2];
    // const ColorList &allowedColors = Allowed->Lab;
    auto deltaL_2 = (Allowed->lab(0) - L).square();
    float C1_2 = a * a + b * b;
    TempVectorXf_t C2_2 = Allowed->lab(1).square() + Allowed->lab(2).square();
    auto deltaCab_2 = (sqrt(C1_2) - C2_2.sqrt()).square();
    auto deltaHab_2 = (Allowed->lab(1) - a).square() +
                      (Allowed->lab(2) - b).square() - deltaCab_2;
    // SL=1,kL=1
    // K1=0.045f
    // K2=0.015f
    float SC_2 = (sqrt(C1_2) * 0.045f + 1.0f) * (sqrt(C1_2) * 0.045f + 1.0f);
    auto SH_2 = (C2_2.sqrt() * 0.015f + 1.0f).square();
    TempVectorXf_t Diff = deltaL_2 + deltaCab_2 / SC_2 + deltaHab_2 / SH_2;
#endif
    return find_result(Diff);
  }

  auto applyLab00(const Eigen::Array3f &c3) noexcept {
    int tempIndex = 0;
    float L1s = c3[0];
    float a1s = c3[1];
    float b1s = c3[2];
    // const ColorList &allow = Allowed->Lab;
    TempVectorXf_t Diff(Allowed->color_count());

    for (short i = 0; i < Allowed->color_count(); i++) {
      Diff(i) = Lab00_diff(L1s, a1s, b1s, Allowed->Lab(i, 0),
                           Allowed->Lab(i, 1), Allowed->Lab(i, 2));
    }

    return find_result(Diff);
  }
};

#endif // NEWTOKICOLOR_HPP