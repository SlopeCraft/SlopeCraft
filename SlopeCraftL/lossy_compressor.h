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

// Eigen::Array<uint8_t,Eigen::Dynamic,1>
class solver_t;

class lossy_compressor {
 public:
  lossy_compressor();
  ~lossy_compressor();
  void setSource(const Eigen::ArrayXi &, std::span<const TokiColor *>);
  bool compress(uint16_t maxHeight, bool allowNaturalCompress);
  const Eigen::ArrayX<uint8_t> &getResult() const;
  double resultFitness() const;

  SlopeCraft::ui_callbacks ui;
  SlopeCraft::progress_callbacks progress_bar;

 private:
  friend class solver_t;
  std::unique_ptr<solver_t> solver;
  std::vector<const TokiColor *> source;

  static uint16_t maxGeneration;

  void runGenetic(uint16_t maxHeight, bool allowNaturalCompress);
};

double randD();
#endif  // LOSSYCOMPRESSOR_H
