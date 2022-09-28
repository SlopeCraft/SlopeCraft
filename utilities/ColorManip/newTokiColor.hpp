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

#include "../../SlopeCraftL/SlopeCraftL.h"
#include "ColorManip.h"
#include "colorset_maptical.hpp"
#include "colorset_optical.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <functional>

// using Eigen::Dynamic;

struct convert_unit {
  explicit convert_unit(ARGB _a, ::SCL_convertAlgo _c) : ARGB(_a), algo(_c) {}
  ARGB ARGB;
  ::SCL_convertAlgo algo;

  inline bool operator==(const convert_unit another) const noexcept {
    return (ARGB == another.ARGB) && (algo == another.algo);
  }
};

struct hash_cvt_unit {
public:
  inline size_t operator()(const convert_unit cu) const noexcept {
    return std::hash<uint32_t>()(cu.ARGB) ^
           std::hash<uint8_t>()(uint8_t(cu.algo));
  }
};

template <bool is_not_optical, class basic_t, class allowed_t>
class newTokiColor
    : public ::std::conditional_t<is_not_optical, newtokicolor_base_maptical,
                                  newtokicolor_base_optical> {
private:
  static constexpr float threshold = 1e-10f;

public:
  using Base_t =
      ::std::conditional_t<is_not_optical, newtokicolor_base_maptical,
                           newtokicolor_base_optical>;
  using TempVectorXf_t = typename Base_t::TempVectorXf_t;
  using result_t = typename Base_t::result_t;

  Eigen::Array3f c3; //   color in some colorspace
  float ResultDiff;  // color diff for the result

  inline auto color_id() const noexcept {
    if constexpr (is_not_optical) {
      return this->Result;
    } else {
      return this->result_color_id;
    }
  }

  // These two members must be defined by caller
  static const basic_t *const Basic;
  static const allowed_t *const Allowed;

public:
  explicit newTokiColor(convert_unit cu) {
    const ARGB rawColor = cu.ARGB;
    switch (cu.algo) {
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
  }

  newTokiColor() {
    if constexpr (is_not_optical) {
      this->Result = 0;
    } else {
      this->result_color_id = colorset_optical_allowed::invalid_color_id;
    }
  }

  auto compute(convert_unit cu) noexcept {
    if (getA(cu.ARGB) == 0) {
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

    switch (cu.algo) {
    case ::SCL_convertAlgo::RGB:
      return applyRGB();
    case ::SCL_convertAlgo::RGB_Better:
      return applyRGB_plus();
    case ::SCL_convertAlgo::HSV:
      return applyHSV();
    case ::SCL_convertAlgo::Lab94:
      return applyLab94();
    case ::SCL_convertAlgo::Lab00:
      return applyLab00();
    case ::SCL_convertAlgo::XYZ:
      return applyXYZ();

    default:
      exit(1);
      return result_t(0);
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
    static_assert(is_not_optical, "");

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

  auto applyRGB() noexcept {

    auto Diff0_2 = (Allowed->rgb(0) - c3[0]).square();
    auto Diff1_2 = (Allowed->rgb(1) - c3[1]).square();
    auto Diff2_2 = (Allowed->rgb(2) - c3[2]).square();

    TempVectorXf_t Diff = Diff0_2 + Diff1_2 + Diff2_2;
    // Data.CurrentColor-=allowedColors;

    return find_result(Diff);
  }

  auto applyRGB_plus() noexcept {
    // const ColorList &allowedColors = Allowed->_RGB;
    float R = c3[0];
    float g = c3[1];
    float b = c3[2];
    float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;
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

    return find_result(dist);
  }

  auto applyHSV() noexcept {

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

  auto applyXYZ() noexcept {

    auto Diff0_2 = (Allowed->xyz(0) - c3[0]).square();
    auto Diff1_2 = (Allowed->xyz(1) - c3[1]).square();
    auto Diff2_2 = (Allowed->xyz(2) - c3[2]).square();

    TempVectorXf_t Diff = Diff0_2 + Diff1_2 + Diff2_2;
    // Data.CurrentColor-=allowedColors;
    return find_result(Diff);
  }

  auto applyLab94() noexcept {

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

    return find_result(Diff);
  }

  auto applyLab00() noexcept {
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