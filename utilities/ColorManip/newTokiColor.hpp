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

#ifndef NEWTOKICOLOR_HPP
#define NEWTOKICOLOR_HPP

// #include "../../SlopeCraftL/SlopeCraftL.h"
#include "../SC_GlobalEnums.h"
#include "ColorManip.h"
#include "ColorManip/ColorManip.h"
#include "colorset_maptical.hpp"
#include "colorset_optical.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <functional>
#include <iostream>
#include <cereal/types/array.hpp>

// using Eigen::Dynamic;
namespace {
inline constexpr float threshold = 1e-10f;

}  // namespace
struct convert_unit {
  convert_unit() = default;
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

  template <class archive>
  void save(archive &ar) const {
    ar(this->_ARGB, this->algo);
  }
  template <class archive>
  void load(archive &ar) {
    ar(this->_ARGB, this->algo);
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
  float ResultDiff;  // color diff for the result

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

  template <typename = void>
  void set_gpu_result(uint16_t __result_color_id,
                      float __result_diff) noexcept {
    static_assert(!is_not_optical,
                  "set_gpu_result is only avaliable for VisualCraftL.");

    this->ResultDiff = __result_diff;
    this->result_color_id = __result_color_id;
  }

  auto compute(convert_unit cu) noexcept {
    if (getA(cu._ARGB) == 0) {
      if constexpr (is_not_optical) {
        this->Result = 0;
        return this->Result;
      } else {
        // static_assert(is_not_optical, "Not implemented here.");
        this->result_color_id = 0xFFFF;
        return uint16_t(0xFFFF);
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
        abort();
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
    if (diff.isNaN().any()) {
      for (int idx = 0; idx < diff.size(); idx++) {
        assert(!std::isnan(diff[idx]));
        assert(diff[idx] >= 0);
      }
    }
    int tempidx = 0;
    this->ResultDiff = diff.minCoeff(&tempidx);

    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempidx);
      if (Base_t::needFindSide) this->doSide(diff);

      return this->Result;
    } else {
      this->result_color_id = Allowed->color_id(tempidx);
      // std::cout << tempidx << '\t' << this->result_color_id << '\n';
      return this->color_id();
    }
  }

  auto find_result(const std::vector<float> &diff) noexcept {
    int minidx = 0;
    float min = diff[0];

    for (int i = 1; i < int(diff.size()); i++) {
      assert(!std::isnan(diff[i]));
      assert(diff[i] >= 0);
      if (diff[i] < min) {
        minidx = i;
        min = diff[i];
      }
    }
    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(minidx);
      if (Base_t::needFindSide) this->doSide(diff);

      return this->Result;
    } else {
      this->result_color_id = Allowed->color_id(minidx);
      return this->color_id();
    }
  }

  template <typename = void>
  void doSide(const TempVectorXf_t &Diff) {
    static_assert(is_not_optical);

    int tempIndex = 0;
    // Diff.array()+=10.0;ResultDiff+=10.0;
    this->sideSelectivity[0] = 1e35f;
    this->sideResult[0] = 0;
    this->sideSelectivity[1] = 1e35f;
    this->sideResult[1] = 0;

    if (!Base_t::needFindSide) return;

    switch (this->Result % 4) {
      case 3:
        return;
      case 0:  // 1,2
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
      case 1:  // 0,2
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
      case 2:  // 0,1
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

    return;
  }

  auto applyRGB(const Eigen::Array3f &c3) noexcept {
    TempVectorXf_t Diff(Allowed->color_count(), 1);
    std::span<float> diff_span{Diff.data(), (size_t)Diff.size()};
    std::span<const float, 3> c3span{c3.data(), 3};
    colordiff_RGB_batch(Allowed->rgb_data_span(0), Allowed->rgb_data_span(1),
                        Allowed->rgb_data_span(2), c3span, diff_span);

    // Data.CurrentColor-=allowedColors;
    const auto ret = find_result(Diff);
    return ret;
  }

  auto applyRGB_plus(const Eigen::Array3f &c3) noexcept {
    // const ColorList &allowedColors = Allowed->_RGB;

    TempVectorXf_t Diff(Allowed->color_count(), 1);
    std::span<float> diff_span{Diff.data(), (size_t)Diff.size()};
    std::span<const float, 3> c3span{c3.data(), 3};
    colordiff_RGBplus_batch(Allowed->rgb_data_span(0),
                            Allowed->rgb_data_span(1),
                            Allowed->rgb_data_span(2), c3span, diff_span);

    // Data.CurrentColor-=allowedColors;
    const auto ret = find_result(Diff);
    return ret;

#if false
    constexpr float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;
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

    TempVectorXf_t dist(Allowed->color_count(), 1);
    if constexpr (is_not_optical) {
      dist = (S_r.square() * w_r * deltaR.square() +
              S_g.square() * w_g * deltaG.square() +
              S_b.square() * w_b * deltaB.square()) /
                 (w_r + w_g + w_b) +
             S_theta * S_ratio * theta.square();
    } else {
      dist.base() = (S_r.square() * w_r * deltaR.square() +
                     S_g.square() * w_g * deltaG.square() +
                     S_b.square() * w_b * deltaB.square()) /
                        (w_r + w_g + w_b) +
                    S_theta * S_ratio * theta.square();
    }
    //+S_theta*S_ratio*theta.square()
#endif
    return find_result(Diff);
  }

  auto applyHSV(const Eigen::Array3f &c3) noexcept {
    // const ColorList &allowedColors = Allowed->HSV;

    TempVectorXf_t Diff(Allowed->color_count(), 1);
    std::span<float> diff_span{Diff.data(), (size_t)Diff.size()};
    std::span<const float, 3> c3span{c3.data(), 3};

    colordiff_HSV_batch(Allowed->hsv_data_span(0), Allowed->hsv_data_span(1),
                        Allowed->hsv_data_span(2), c3span, diff_span);

    return find_result(Diff);
  }

  auto applyXYZ(const Eigen::Array3f &c3) noexcept {
    TempVectorXf_t Diff(Allowed->color_count(), 1);
    std::span<float> diff_span{Diff.data(), (size_t)Diff.size()};
    std::span<const float, 3> c3span{c3.data(), 3};
    colordiff_RGB_batch(Allowed->xyz_data_span(0), Allowed->xyz_data_span(1),
                        Allowed->xyz_data_span(2), c3span, diff_span);

    // Data.CurrentColor-=allowedColors;
    const auto ret = find_result(Diff);
    return ret;
  }

  auto applyLab94(const Eigen::Array3f &c3) noexcept {
    TempVectorXf_t Diff(Allowed->color_count(), 1);
    std::span<float> diff_span{Diff.data(), (size_t)Diff.size()};
    std::span<const float, 3> c3span{c3.data(), 3};
    colordiff_Lab94_batch(Allowed->lab_data_span(0), Allowed->lab_data_span(1),
                          Allowed->lab_data_span(2), c3span, diff_span);
    return find_result(Diff);
  }

  auto applyLab00(const Eigen::Array3f &c3) noexcept {
    // int tempIndex = 0;
    const float L1s = c3[0];
    const float a1s = c3[1];
    const float b1s = c3[2];
    // const ColorList &allow = Allowed->Lab;
    TempVectorXf_t Diff(Allowed->color_count(), 1);

    for (int i = 0; i < Allowed->color_count(); i++) {
      Diff(i) = Lab00_diff(L1s, a1s, b1s, Allowed->Lab(i, 0),
                           Allowed->Lab(i, 1), Allowed->Lab(i, 2));
    }

    return find_result(Diff);
  }

 public:
  template <class archive>
  void save(archive &ar) const {
    static_assert(is_not_optical,
                  "Serialization is only avaliable for maptical maps");
    ar(this->sideSelectivity, this->sideResult, this->Result, this->ResultDiff);
  }

  template <class archive>
  void load(archive &ar) {
    static_assert(is_not_optical,
                  "Serialization is only avaliable for maptical maps");
    ar(this->sideSelectivity, this->sideResult, this->Result, this->ResultDiff);
  }
};

#endif  // NEWTOKICOLOR_HPP