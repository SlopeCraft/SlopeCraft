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

#ifndef HEIGHTLINE_H
#define HEIGHTLINE_H

#include <iostream>
#include <map>
#include <vector>
#include <functional>

#include <mc_block.h>
#include "optimize_chain.h"
#include "SCLDefines.h"
#include "water_item.h"

class height_line {
 public:
  height_line() = default;
  float make(const TokiColor*const[],
             const Eigen::Array<uint8_t, Eigen::Dynamic, 1>&,
             bool allowNaturalCompress,
             const std::function<bool(uint8_t)>& need_support_from_base_color,
             Eigen::ArrayXi* dst = nullptr);
  void make(const Eigen::ArrayXi& mapColorCol, bool allowNaturalCompress,
            const std::function<bool(uint8_t)>& need_support_from_base_color);
  void updateWaterMap();
  [[nodiscard]] uint32_t maxHeight() const;

  [[nodiscard]] const Eigen::ArrayXi& getHighLine() const noexcept {
    return HighLine;
  }
  [[nodiscard]] const Eigen::ArrayXi& getLowLine() const noexcept {
    return LowLine;
  }

  [[nodiscard]] auto& getBase() const noexcept { return this->base; }
  [[nodiscard]] const std::map<uint32_t, water_y_range>& getWaterMap() const;
  [[nodiscard]] EImage toImg() const;

  static constexpr ARGB BlockColor = ARGB32(0, 0, 0);
  static constexpr ARGB AirColor = ARGB32(255, 255, 255);
  static constexpr ARGB WaterColor = ARGB32(0, 64, 255);
  static constexpr ARGB greyColor = ARGB32(192, 192, 192);

 private:
  Eigen::ArrayXi base;
  Eigen::ArrayXi HighLine;
  Eigen::ArrayXi LowLine;
  std::map<uint32_t, water_y_range> waterMap;
};

#endif  // HEIGHTLINE_H
