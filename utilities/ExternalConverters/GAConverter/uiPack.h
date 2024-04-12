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

#ifndef UIPACK_H
#define UIPACK_H
#include "GACvterDefines.hpp"
#include "sortColor.h"

#include <HeuristicFlow/SimpleMatrix>

#include <uiPack/uiPack.h>

namespace GACvter {

using uiPack = ::uiPack;

struct CvterInfo {
  EImage rawImageCache;
  heu::MatrixDynamicSize<sortColor>
      colorMap;  //  the relationship between pixels and sortColor s
  std::clock_t prevClock;
  bool strongMutation;
  GrayImage edgeFeatureMap;
  std::vector<Var_t> seeds;  //     seeds to initialize population
  std::unordered_map<ARGB, order_t> iniTool;
  uiPack ui;
  mapColor2Gray_LUT_t mapColor2Gray{};
  const colorset_allowed_t* allowed_colorset{nullptr};
};

}  //  namespace GACvter

#endif  // UIPACK_H
