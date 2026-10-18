/*
 Copyright © 2021-2026  TokiNoBug
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

#ifndef LOSSYCOMPRESSOR_H
#define LOSSYCOMPRESSOR_H

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <list>
#include <random>
#include <vector>
#include <memory>

#include "height_line.h"
#include "SCLDefines.h"
#include "water_item.h"

struct GA_options {
  double initialize_non_zero_ratio{0.05};
  size_t max_generation{200};
  size_t population_size{50};
  /// just like maxFailTimes
  size_t early_stop_patience{30};
  double crossover_probability{0.9};
  double mutate_probability{0.01};
};

struct lossy_compress_input {
  std::vector<const TokiColor*> source;
  std::function<bool(uint8_t)> need_support_from_base_color;

  uint16_t max_height;
  bool allow_natural_compress;
  // genetic options
  GA_options genetic_options;
  // ui pointers
  SlopeCraft::ui_callbacks ui;
  SlopeCraft::progress_callbacks progress_bar;

};

struct lossy_compress_result {
  // 0,1,2
  Eigen::ArrayX<uint8_t> gene;
  double fitness;
};

[[nodiscard]] lossy_compress_result lossy_compress(const lossy_compress_input&);

class solver_t;

// class lossy_compressor {
//  public:
//   lossy_compressor();
//   ~lossy_compressor();
//   void setSource(const Eigen::ArrayXi&, std::span<const TokiColor*>,
//                  std::function<bool(uint8_t)> need_support_from_base_color);
//   bool compress(uint16_t maxHeight, bool allowNaturalCompress);
//   const Eigen::ArrayX<uint8_t>& getResult() const;
//   double resultFitness() const;
//
//
//  private:
//   friend class solver_t;
//   std::unique_ptr<solver_t> solver;
//   std::vector<const TokiColor*> source;
//   std::function<bool(uint8_t)> need_support_from_base_color;
//
//   static const uint16_t maxGeneration;
//
//   void runGenetic(uint16_t maxHeight, bool allowNaturalCompress);
// };

#endif  // LOSSYCOMPRESSOR_H
