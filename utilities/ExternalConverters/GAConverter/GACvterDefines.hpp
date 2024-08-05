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

#ifndef GACVTERDEFINES_H
#define GACVTERDEFINES_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <complex>
#include <Eigen/Dense>
#include <HeuristicFlow/Genetic>
#include <HeuristicFlow/Global>

#include "../ExternalConverterStaticInterface.h"

// using namespace SlopeCraft;
namespace GACvter {
using heu::FitnessOption;
using heu::RecordOption;

using gray_t = int32_t;
const gray_t GrayMax = 0xFF;
using GrayImage = Eigen::ArrayXX<gray_t>;

using mapColor_t = uint8_t;

using order_t = uint8_t;
constexpr order_t OrderMax = 4;

using mutateMap_t = Eigen::Array<order_t, OrderMax, OrderMax - 1>;

using RGBSrc_t = ColorList;
using mapColorSrc_t = MapList;
using mapColor2Gray_LUT_t = std::array<gray_t, 256>;
using mapColor2ARGB_LUT_t = std::array<ARGB, 256>;

using Var_t = Eigen::ArrayXX<order_t>;

const Eigen::Array33i SobelX = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
const Eigen::Array33i SobelY = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

const Eigen::Array<int, 5, 5> Gaussian = {{2, 4, 5, 4, 2},
                                          {4, 9, 12, 9, 4},
                                          {5, 12, 15, 12, 5},
                                          {4, 9, 12, 9, 4},
                                          {2, 4, 5, 4, 2}};
// const int GauSum = Gaussian.sum();

inline mutateMap_t makeMutateMap() {
  mutateMap_t res;
  for (order_t r = 0; r < OrderMax; r++) {
    order_t c = 0;
    for (order_t o = 0; o < OrderMax; o++) {
      if (o == r) {
        continue;
      }
      res(r, c) = o;
      c++;
    }
  }
  return res;
}

inline float RGB2Gray_Gamma(const float r, const float g, const float b) {
  float poweredSum =
      std::pow(r, 2.2) + std::pow(1.5 * g, 2.2) + std::pow(0.6 * b, 2.2);
  static const float poweredWeight =
      1 + std::pow(1.5, 2.2) + std::pow(0.6, 2.2);
  return std::pow(poweredSum / poweredWeight, 1.0 / 2.2);
}

inline void applyGaussian(const GrayImage &src, GrayImage *dest,
                          const Eigen::Array<int, 5, 5> &core) {
  const int coreSum = core.sum();
  dest->resize(src.rows() - 4, src.cols() - 4);
  for (int r = 0; r < dest->rows(); r++) {
    for (int c = 0; c < dest->cols(); c++) {
      dest->operator()(r, c) = (src.block<5, 5>(r, c) * core).sum() / coreSum;
    }
  }
}

inline void applySobel(const GrayImage &gray, GrayImage *feature) {
  static const uint8_t padding = 1;
  GrayImage xFeature(gray.rows() - 2 * padding, gray.cols() - 2 * padding),
      yFeature(gray.rows() - 2 * padding, gray.cols() - 2 * padding);

  for (uint32_t r = 0; r < xFeature.rows(); r++) {
    for (uint32_t c = 0; c < xFeature.cols(); c++) {
      xFeature(r, c) = (SobelX * (gray.block<3, 3>(r, c))).sum();
      yFeature(r, c) = (SobelY * (gray.block<3, 3>(r, c))).sum();
    }
  }
  *feature = (xFeature.square() + yFeature.square()).sqrt();
}

template <uint8_t Padding, int PaddingVal>
void EImg2GrayImg(const EImage &e, GrayImage *gImg) noexcept {
  gImg->resize(e.rows() + 2 * Padding, e.cols() + 2 * Padding);
  gImg->setConstant(PaddingVal);

  for (uint32_t r = 0; r < e.rows(); r++) {
    for (uint32_t c = 0; c < e.cols(); c++) {
      ARGB curColor = e(r, c);
      if (getA(curColor) <= 0) {
        continue;
      }
      const float c_r = getR(curColor) / 255.0, c_g = getG(curColor) / 255.0,
                  c_b = getB(curColor) / 255.0;

      gImg->operator()(Padding + r, Padding + c) =
          GrayMax * RGB2Gray_Gamma(c_r, c_g, c_b);
    }
  }
}
}  //  namespace GACvter

#endif  // GACVTERDEFINES_H
