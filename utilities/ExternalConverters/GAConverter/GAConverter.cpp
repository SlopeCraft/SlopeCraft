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

#include "GAConverter.h"

#include <mutex>

using namespace GACvter;
// using namespace SlopeCraft;

#define mapColor2Index(mapColor) (64 * ((mapColor) % 4) + ((mapColor) / 4))
#define index2mapColor(index) (4 * ((index) % 64) + ((index) / 64))

namespace GACvter {

void delete_GA_converter(GAConverter *g) noexcept { delete g; }
// #error "Fix singleton here"
// mapColor2Gray_LUT_t mapColor2Gray;

const mutateMap_t mutateMap = GACvter::makeMutateMap();

inline ARGB mapColor2ARGB32(const mapColor_t mC) noexcept {
  const int rIdx = mapColor2Index(mC);
  Eigen::Array<uint8_t, 3, 1> rgb;
  for (int channel = 0; channel < 3; channel++) {
    int temp = SlopeCraft::BasicRGB4External(channel)[rIdx] * 255;

    rgb[channel] = std::max(0, std::min(temp, 255));
  }

  return ARGB32(rgb(0), rgb(1), rgb(2), (mC < 4) ? 0 : 255);
}

}  // namespace GACvter

GAConverter::GAConverter() { this->setTournamentSize(3); }

mapColor2Gray_LUT_t GACvter::updateMapColor2GrayLUT() {
  mapColor2Gray_LUT_t result{};
  result.fill(0);
  for (int row = 0; row < 256; row++) {
    const mapColor_t mC = index2mapColor(row);
    const float r = SlopeCraft::BasicRGB4External(0)[row];
    const float g = SlopeCraft::BasicRGB4External(1)[row];
    const float b = SlopeCraft::BasicRGB4External(2)[row];
    result[mC] = GrayMax * RGB2Gray_Gamma(r, g, b);
  }
  return result;
}

template <bool strong>
void privateMutateFun(const Var_t *parent, Var_t *child,
                      const CvterInfo *arg) noexcept {
  constexpr float ratio = 0.01;
  constexpr float Threshold = ratio * 2 - 1;
  *child = *parent;
  if constexpr (strong  //&& false
  ) {                   //  strong mutation
    static Eigen::ArrayXXf randMat(arg->rawImageCache.rows(),
                                   arg->rawImageCache.cols());
    randMat.setRandom();

    for (int idx = 0; idx < arg->rawImageCache.size(); idx++) {
      if (randMat(idx) <= Threshold) {
        child->operator()(idx) =
            mutateMap(parent->operator()(idx), heu::randIdx(OrderMax - 1));
      }
    }
  } else {  //  weak mutation
    const int idx = heu::randIdx(parent->size());
    child->operator()(idx) =
        mutateMap(parent->operator()(idx), heu::randIdx(OrderMax - 1));
  }
}

void GACvter::iFun(Var_t *v, const CvterInfo *arg) noexcept {
  v->setZero(arg->rawImageCache.rows(), arg->rawImageCache.cols());

  if (heu::randD() < 1.0 / 3) {  //  generate by random

    std::unordered_map<ARGB, order_t> iniToolCpy = arg->iniTool;
    for (auto &i : iniToolCpy) {
      i.second = heu::randIdx(OrderMax);
    }

    for (int64_t i = 0; i < arg->rawImageCache.size(); i++) {
      v->operator()(i) = iniToolCpy[arg->rawImageCache(i)];
    }

  } else {  //  generate by seed and mutation

    if (heu::randD() < 0.4) {  //  strong mutation

      privateMutateFun<true>(&arg->seeds[heu::randIdx(arg->seeds.size())], v,
                             arg);

    } else {  //  weak mutation

      privateMutateFun<false>(&arg->seeds[heu::randIdx(arg->seeds.size())], v,
                              arg);
    }
  }
}

void GACvter::fFun(const Var_t *v, const CvterInfo *arg, double *f) noexcept {
  GrayImage gray(arg->rawImageCache.rows(), arg->rawImageCache.cols()), edged;
  for (int64_t i = 0; i < arg->rawImageCache.size(); i++) {
    gray(i) = arg->mapColor2Gray[arg->colorMap(i).mapColor(v->operator()(i))];
  }

  applyGaussian(gray, &edged, Gaussian);
  applySobel(edged, &gray);
  // applyGaussian(gray,&edged,Gaussian);

  double edgeScore = double((gray - arg->edgeFeatureMap).abs().sum());
  edgeScore /= arg->rawImageCache.size();
  *f = std::log10(edgeScore);
}

void GACvter::cFun(const Var_t *p1, const Var_t *p2, Var_t *c1, Var_t *c2,
                   const CvterInfo *arg) noexcept {
  const uint32_t rows = arg->rawImageCache.rows();
  const uint32_t cols = arg->rawImageCache.cols();
  const uint32_t rS = heu::randD(1, rows - 2);
  const uint32_t cS = heu::randD(1, cols - 2);

  const Var_t *src[2] = {p1, p2};
  Var_t *dst[2] = {c1, c2};

  for (uint8_t i = 0; i < 2; i++) {
    uint8_t srcIdx;
    dst[i]->resize(rows, cols);
    srcIdx = heu::randD() < 0.5;
    dst[i]->block(0, 0, rS, cS) = src[srcIdx]->block(0, 0, rS, cS);
    srcIdx = heu::randD() < 0.5;
    dst[i]->block(rS, 0, rows - rS, cS) =
        src[srcIdx]->block(rS, 0, rows - rS, cS);
    srcIdx = heu::randD() < 0.5;
    dst[i]->block(0, cS, rS, cols - cS) =
        src[srcIdx]->block(0, cS, rS, cols - cS);
    srcIdx = heu::randD() < 0.5;
    dst[i]->block(rS, cS, rows - rS, cols - cS) =
        src[srcIdx]->block(rS, cS, rows - rS, cols - cS);
  }
}

void GACvter::mFun(const Var_t *parent, Var_t *child,
                   const CvterInfo *arg) noexcept {
  if (arg->strongMutation) {
    privateMutateFun<true>(parent, child, arg);
  } else {
    privateMutateFun<false>(parent, child, arg);
  }
}

void GACvter::GAConverter::setUiPack(const uiPack &_u) noexcept {
  this->_args.ui = _u;
}

void GACvter::GAConverter::setRawImage(const EImage &src) noexcept {
  this->_args.rawImageCache = src;
  GrayImage gray, edged;
  EImg2GrayImg<0, 0>(this->_args.rawImageCache, &gray);
  applyGaussian(gray, &edged, GACvter::Gaussian);
  applySobel(edged, &this->_args.edgeFeatureMap);

  std::unordered_map<ARGB, sortColor> colorHash;

  colorHash.clear();
  colorHash.reserve(this->_args.rawImageCache.size());
  this->_args.colorMap.resize(this->_args.rawImageCache.rows(),
                              this->_args.rawImageCache.cols());

  for (int64_t i = 0; i < this->_args.rawImageCache.size(); i++) {
    colorHash[this->_args.rawImageCache(i)];
  }

  this->_args.iniTool.clear();
  this->_args.iniTool.reserve(colorHash.size());

  for (auto &i : colorHash) {
    i.second.calculate(i.first, *this->_args.allowed_colorset);
    this->_args.iniTool[i.first] = 0;
  }

  for (int i = 0; i < this->_args.rawImageCache.size(); i++) {
    this->_args.colorMap(i) = colorHash[this->_args.rawImageCache(i)];
  }

  this->_args.mapColor2Gray = updateMapColor2GrayLUT();
}

void GACvter::GAConverter::setSeeds(
    const std::vector<const Eigen::ArrayXX<mapColor_t> *>
        &mapColorMats) noexcept {
  this->_args.seeds.clear();
  this->_args.seeds.resize(mapColorMats.size());
  const int Rows = this->_args.rawImageCache.rows(),
            Cols = this->_args.rawImageCache.cols();
  for (size_t idx = 0; idx < mapColorMats.size(); idx++) {
    this->_args.seeds[idx].setZero(Rows, Cols);
    // convert mapColor_t to order_t

    for (int64_t pixIdx = 0; pixIdx < Rows * Cols; pixIdx++) {
      this->_args.seeds[idx](pixIdx) = 0;

      for (order_t o = 0; o < OrderMax; o++) {
        //  go through the 4 colors to find if there is a color same as that in
        //  seed.
        if (mapColorMats[idx]->operator()(pixIdx) ==
            this->_args.colorMap(pixIdx).mapColor(o)) {
          this->_args.seeds[idx](pixIdx) = o;
          break;
        }
      }
    }
  }
}

void GACvter::GAConverter::run() {
  this->_args.prevClock = std::clock();
  this->_args.strongMutation = true;

  this->initializePop();

  this->template __impl_run<GACvter::GAConverter>();
}

void GACvter::GAConverter::resultImage(EImage *dst) {
  const Var_t &res = this->result();
  dst->resize(res.rows(), res.cols());

  for (int64_t pixIdx = 0; pixIdx < res.size(); pixIdx++) {
    const order_t o = res(pixIdx);
    const mapColor_t mC = this->_args.colorMap(pixIdx).mapColor(o);
    const ARGB pixColor = mapColor2ARGB32(mC);
    dst->operator()(pixIdx) = pixColor;
  }
}
