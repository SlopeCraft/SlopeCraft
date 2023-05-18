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
#include "object_pool.hpp"

#include "SCLDefines.h"

#include "Colors.h"
#include "WaterItem.h"
#include <iostream>
#include <list>
#include <queue>
#include <random>
#include <stack>
#include <unsupported/Eigen/CXX11/Tensor>
#include <vector>

// using namespace Eigen;

extern const ARGB airColor;
extern const ARGB targetColor;
extern const ARGB glassColor;

typedef Eigen::Array<uint8_t, Eigen::Dynamic, Eigen::Dynamic> TokiMap;
typedef TokiMap glassMap;
typedef TokiMap walkableMap;

class edge {
 public:
  edge();
  edge(uint32_t begIdx, uint32_t endIdx);
  // edge(uint32_t begIdx,uint32_t endIdx);
  // edge(TokiPos,TokiPos);
  // edge(uint16_t r1,uint16_t c1,uint16_t r2,uint16_t c2);
  uint32_t begIdx;
  uint32_t endIdx;
  int lengthSquare;

  static const std::vector<TokiPos> *vertexes;

  TokiPos beg() const;
  TokiPos end() const;
  bool connectWith(TokiPos) const;
  void drawEdge(glassMap &, bool drawHead = false) const;
};

class pairedEdge : public std::pair<TokiPos, TokiPos> {
 public:
  pairedEdge();
  pairedEdge(TokiPos, TokiPos);
  pairedEdge(uint16_t r1, uint16_t c1, uint16_t r2, uint16_t c2);
  pairedEdge(const edge &);
  int lengthSquare;

  bool connectWith(TokiPos) const;
  void drawEdge(glassMap &, bool drawHead = false) const;
};

TokiMap ySlice2TokiMap(const Eigen::Tensor<uint8_t, 3> &) noexcept;
TokiMap ySlice2TokiMap_u16(const Eigen::Tensor<uint16_t, 3> &) noexcept;

glassMap connectBetweenLayers(const TokiMap &, const TokiMap &,
                              walkableMap *walkable);
// 返回值是架构在相对较高的一层上的，walkable 是各层俯视图叠加

class PrimGlassBuilder {
 public:
  PrimGlassBuilder();

  template <typename T, size_t S>
  friend class tf::ObjectPool;
  void *_object_pool_block;

  static const uint32_t unitL = 32;
  static const uint32_t reportRate = 50;
  enum blockType { air = 0, glass = 1, target = 127 };
  glassMap makeBridge(const TokiMap &_targetMap,
                      walkableMap *walkable = nullptr);
  void **windPtr;
  void (**progressRangeSetPtr)(void *, int, int, int);
  void (**progressAddPtr)(void *, int);
  void (**keepAwakePtr)(void *);

 private:
  std::vector<TokiPos> targetPoints;
  std::list<edge> edges;
  std::vector<pairedEdge> tree;
  void addEdgesToGraph();
  void runPrim();
  glassMap make4SingleMap(const TokiMap &_targetMap, walkableMap *walkable);
  static pairedEdge connectSingleMaps(const PrimGlassBuilder *map1,
                                      TokiPos offset1,
                                      const PrimGlassBuilder *map2,
                                      TokiPos offset2);
};
inline tf::ObjectPool<PrimGlassBuilder> pgb;

EImage TokiMap2EImage(const TokiMap &);

#endif  // PRIMGLASSBUILDER_H
