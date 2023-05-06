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

#include "MapImageCvter.h"

#include <ExternalConverters/ExternalConverterStaticInterface.h>
#include <ExternalConverters/GAConverter/GAConverter.h>
#include <bxzstr.hpp>
#include <cereal/archives/binary.hpp>
#include <seralize_funs.hpp>

using namespace libImageCvt;

libMapImageCvt::MapImageCvter::MapImageCvter()
    : gacvter(new GACvter::GAConverter) {}

void libMapImageCvt::MapImageCvter::convert_image(
    const ::SCL_convertAlgo algo, bool dither,
    const heu::GAOption *const opt) noexcept {
  if (algo != ::SCL_convertAlgo::gaCvter) {
    Base_t::convert_image(algo, dither);
    return;
  }
  // dither = false;
  constexpr int seed_num = 5;
  constexpr std::array<::SCL_convertAlgo, seed_num> seed_algos = {
      ::SCL_convertAlgo::RGB, ::SCL_convertAlgo::RGB_Better,
      ::SCL_convertAlgo::Lab94, ::SCL_convertAlgo::HSV, ::SCL_convertAlgo::XYZ};

  std::array<Eigen::ArrayXX<uint8_t>, seed_num> cvtedmap;
  std::vector<const Eigen::ArrayXX<uint8_t> *> seeds(seed_num);

  for (int a = 0; a < seed_num; a++) {
    Base_t::convert_image(seed_algos[a], false);
    cvtedmap[a] = this->mapcolor_matrix();
    seeds[a] = &cvtedmap[a];
  }
  gacvter->setRawImage(this->raw_image());

  gacvter->setOption(*opt);
  gacvter->setSeeds(seeds);

  gacvter->setUiPack(this->ui);

  gacvter->run();

  Eigen::ArrayXX<ARGB> raw_image_cache = this->_raw_image;

  gacvter->resultImage(&this->_raw_image);

  Base_t::convert_image(::SCL_convertAlgo::RGB_Better, dither);

  this->_raw_image = raw_image_cache;
}

bool libMapImageCvt::MapImageCvter::save_cache(
    const char *filename) const noexcept {
  bxz::ofstream ofs{filename, std::ios::binary, bxz::Compression::z, 9};
  if (!ofs) {
    return false;
  }

  {
    cereal::BinaryOutputArchive boa(ofs);

    boa(*this);
  }

  ofs.close();

  return true;
}