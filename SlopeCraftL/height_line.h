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

#ifndef HEIGHTLINE_H
#define HEIGHTLINE_H

#include <iostream>
#include <map>
#include <vector>
#include "optimize_chain.h"
#include "SCLDefines.h"
#include "water_item.h"

class height_line {
 public:
  height_line();
  float make(const TokiColor *[],
             const Eigen::Array<uint8_t, Eigen::Dynamic, 1> &,
             bool allowNaturalCompress, Eigen::ArrayXi *dst = nullptr);
  void make(const Eigen::ArrayXi &mapColorCol, bool allowNaturalCompress);
  void updateWaterMap();
  uint32_t maxHeight() const;

  const Eigen::ArrayXi &getHighLine() const noexcept { return HighLine; }
  const Eigen::ArrayXi &getLowLine() const noexcept { return LowLine; }

  auto &getBase() const noexcept { return this->base; }
  const std::map<uint32_t, water_y_range> &getWaterMap() const;
  EImage toImg() const;

  static const ARGB BlockColor;
  static const ARGB AirColor;
  static const ARGB WaterColor;
  static const ARGB greyColor;

 private:
  Eigen::ArrayXi base;
  Eigen::ArrayXi HighLine;
  Eigen::ArrayXi LowLine;
  std::map<uint32_t, water_y_range> waterMap;
};

#endif  // HEIGHTLINE_H
