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

#ifndef PRIMGLASSBUILDER_H
#define PRIMGLASSBUILDER_H
#include <iostream>
#include <list>
#include <queue>
#include <random>
#include <stack>
#include <unsupported/Eigen/CXX11/Tensor>
#include <vector>
#include <utilities/Schem/Schem.h>

#include "SCLDefines.h"
#include "water_item.h"

// using namespace Eigen;

extern const ARGB airColor;
extern const ARGB targetColor;
extern const ARGB glassColor;

typedef Eigen::Array<uint8_t, Eigen::Dynamic, Eigen::Dynamic> TokiMap;
typedef TokiMap glassMap;
typedef TokiMap walkableMap;

class edge {
 public:
  explicit edge() = delete;
  edge(uint32_t begIdx, uint32_t endIdx, std::span<const rc_pos> vertices);
  const uint32_t begIdx;
  const uint32_t endIdx;
  const int lengthSquare;

  rc_pos beg(std::span<const rc_pos> vertices) const;
  rc_pos end(std::span<const rc_pos> vertices) const;
  //  bool connectWith(rc_pos) const;
  //  void drawEdge(glassMap &, bool drawHead = false) const;
};

class pairedEdge : public std::pair<rc_pos, rc_pos> {
 public:
  pairedEdge();
  pairedEdge(rc_pos, rc_pos);
  pairedEdge(uint32_t r1, uint32_t c1, uint32_t r2, uint32_t c2);
  pairedEdge(const edge &, std::span<const rc_pos> vertices);

  int lengthSquare;

  bool connectWith(rc_pos) const;
  void drawEdge(glassMap &, bool drawHead = false) const;
};

//[[deprecated]] TokiMap ySlice2TokiMap(
//    const Eigen::Tensor<uint8_t, 3> &) noexcept;
//[[deprecated]] TokiMap ySlice2TokiMap_u16(
//    const Eigen::Tensor<uint32_t, 3> &) noexcept;
TokiMap ySlice2TokiMap_u16(const Eigen::Tensor<uint16_t, 3> &xzy,
                           std::span<const int, 3> start_xzy,
                           std::span<const int, 3> extension_xzy) noexcept;

glassMap connectBetweenLayers(const TokiMap &, const TokiMap &,
                              walkableMap *walkable);
// 返回值是架构在相对较高的一层上的，walkable 是各层俯视图叠加

class prim_glass_builder {
 public:
  prim_glass_builder();

  static const uint32_t unitL = 32;
  static const uint32_t reportRate = 50;
  enum blockType { air = 0, glass = 1, target = 127 };
  glassMap makeBridge(const TokiMap &_targetMap,
                      walkableMap *walkable = nullptr);

  SlopeCraft::ui_callbacks ui;
  SlopeCraft::progress_callbacks progress_bar;

 private:
  std::vector<rc_pos> targetPoints;
  std::list<edge> edges;
  std::vector<pairedEdge> tree;
  void addEdgesToGraph();
  void runPrim();
  glassMap make4SingleMap(const TokiMap &_targetMap, walkableMap *walkable);
  static pairedEdge connectSingleMaps(const prim_glass_builder &map1,
                                      rc_pos offset1,
                                      const prim_glass_builder &map2,
                                      rc_pos offset2);
};

EImage TokiMap2EImage(const TokiMap &);

#endif  // PRIMGLASSBUILDER_H
