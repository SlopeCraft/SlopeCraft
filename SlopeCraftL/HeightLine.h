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

#ifndef HEIGHTLINE_H
#define HEIGHTLINE_H

#include "Colors.h"
#include "OptiChain.h"
#include "SCLDefines.h"
#include "WaterItem.h"
#include <iostream>
#include <map>
#include <vector>

class HeightLine {
public:
  HeightLine();
  float make(const TokiColor *[],
             const Eigen::Array<uint8_t, Eigen::Dynamic, 1> &,
             bool allowNaturalCompress, Eigen::ArrayXi *dst = nullptr);
  void make(const Eigen::ArrayXi &mapColorCol, bool allowNaturalCompress);
  void updateWaterMap();
  uint16_t maxHeight() const;
  const Eigen::ArrayXi &getHighLine() const;
  const Eigen::ArrayXi &getLowLine() const;
  const Eigen::ArrayXi &getBase() const;
  const std::map<uint16_t, waterItem> &getWaterMap() const;
  EImage toImg() const;

  static const ARGB BlockColor;
  static const ARGB AirColor;
  static const ARGB WaterColor;
  static const ARGB greyColor;

private:
  Eigen::ArrayXi base;
  Eigen::ArrayXi HighLine;
  Eigen::ArrayXi LowLine;
  std::map<uint16_t, waterItem> waterMap;
};

#endif // HEIGHTLINE_H
