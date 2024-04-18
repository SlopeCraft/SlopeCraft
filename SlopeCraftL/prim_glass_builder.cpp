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

#include "prim_glass_builder.h"

const ARGB airColor = ARGB32(255, 255, 255);
const ARGB targetColor = ARGB32(0, 0, 0);
const ARGB glassColor = ARGB32(192, 192, 192);

// const std::vector<rc_pos> *edge::vertexes = nullptr;

// edge::edge() {
//   // beg=TokiRC(0,0);
//   // end=TokiRC(0,0);
//   begIdx = 0;
//   endIdx = 0;
//   lengthSquare = 0;
// }

edge::edge(uint32_t b, uint32_t e, std::span<const rc_pos> v)
    : begIdx{b}, endIdx{e}, lengthSquare{[b, e, v]() {
        const auto beg = v[b];
        const auto end = v[e];
        const int rowSpan = beg.row - end.row;
        const int colSpan = beg.col - end.col;
        return (rowSpan * rowSpan + colSpan * colSpan);
      }()} {}

rc_pos edge::beg(std::span<const rc_pos> vertices) const {
  return vertices[begIdx];
}

rc_pos edge::end(std::span<const rc_pos> vertices) const {
  return vertices[endIdx];
}

pairedEdge::pairedEdge()
    : std::pair<rc_pos, rc_pos>{{0, 0}, {0, 0}}, lengthSquare{0} {}
pairedEdge::pairedEdge(rc_pos A, rc_pos B) {
  int r1 = A.row, c1 = A.col;
  int r2 = B.row, c2 = B.col;
  first = A;
  second = B;
  int rowSpan = r1 - r2;
  int colSpan = c1 - c2;
  lengthSquare = (rowSpan * rowSpan + colSpan * colSpan);
}

pairedEdge::pairedEdge(uint32_t r1, uint32_t c1, uint32_t r2, uint32_t c2) {
  first = rc_pos{static_cast<int32_t>(r1), static_cast<int32_t>(c1)};
  second = rc_pos{static_cast<int32_t>(r2), static_cast<int32_t>(c2)};
  int rowSpan = r1 - r2;
  int colSpan = c1 - c2;
  lengthSquare = (rowSpan * rowSpan + colSpan * colSpan);
}

pairedEdge::pairedEdge(const edge &src, std::span<const rc_pos> v) {
  first = src.beg(v);
  second = src.end(v);
  lengthSquare = src.lengthSquare;
}
/*
bool edge::connectWith(rc_pos P) const {
    return pairedEdge(*this).connectWith(P);
}

void edge::drawEdge(glassMap & map, bool drawHead) const {
    pairedEdge(*this).drawEdge(map,drawHead);
    return;
}
*/
bool pairedEdge::connectWith(rc_pos P) const {
  return (first == P) || (second == P);
}

void pairedEdge::drawEdge(glassMap &map, bool drawHead) const {
  if (lengthSquare <= 2) return;
  float length = sqrt(lengthSquare);
  Eigen::Vector2f startPoint(first.row, first.col);
  Eigen::Vector2f endPoint(second.row, second.col);
  Eigen::Vector2f step = (endPoint - startPoint) / ceil(2.0 * length);
  Eigen::Vector2f cur;
  int stepCount = ceil(2.0 * length);
  int r, c;
  for (int i = 1; i < stepCount; i++) {
    cur = i * step + startPoint;
    r = floor(cur(0));
    c = floor(cur(1));
    if (r >= 0 && r < map.rows() && c >= 0 && c < map.cols()) {
      map(r, c) = prim_glass_builder::glass;
      continue;
    }
    r = ceil(cur(0));
    c = ceil(cur(1));
    if (r >= 0 && r < map.rows() && c >= 0 && c < map.cols())
      map(r, c) = prim_glass_builder::glass;
  }
  map(first.row, first.col) =
      (drawHead ? prim_glass_builder::target : prim_glass_builder::air);
  map(second.row, second.col) =
      (drawHead ? prim_glass_builder::target : prim_glass_builder::air);
}

prim_glass_builder::prim_glass_builder() {}
glassMap prim_glass_builder::makeBridge(const TokiMap &_targetMap,
                                        walkableMap *walkable) {
  // clock_t lastTime=std::clock();
  const int rowCount = ceil(double(_targetMap.rows()) / unitL);
  const int colCount = ceil(double(_targetMap.cols()) / unitL);

  std::vector<std::vector<prim_glass_builder>> algos(rowCount);
  std::vector<std::vector<glassMap>> glassMaps(rowCount);
  std::vector<std::vector<walkableMap>> walkableMaps(rowCount);
  std::vector<std::vector<TokiMap>> targetMaps(rowCount);

  // std::cerr<<"开始分区分块，共 ["<<rowCount<<','<<colCount<<"] 个分区";
  for (int r = 0; r < rowCount; r++) {
    algos[r].resize(colCount);
    glassMaps[r].resize(colCount);
    walkableMaps[r].resize(colCount);
    targetMaps[r].resize(colCount);
    for (int c = 0; c < colCount; c++) {
      targetMaps[r][c] = _targetMap.block(
          unitL * r, unitL * c,
          std::min(long(unitL), long(_targetMap.rows() - r * unitL)),
          std::min(long(unitL), long(_targetMap.cols() - c * unitL)));

      algos[r][c] = prim_glass_builder{};
    }
  }
  // qDebug("分区分块完毕，开始在每个分区内搭桥");
  for (int r = 0; r < rowCount; r++) {
    for (int c = 0; c < colCount; c++) {
      // qDebug()<<"开始处理第 ["<<r<<","<<c<<"] 块分区";
      glassMaps[r][c] = algos[r][c].make4SingleMap(
          targetMaps[r][c],
          (walkable == nullptr) ? nullptr : (&walkableMaps[r][c]));
    }
    this->progress_bar.set_range(0, rowCount, r);
  }
  // qDebug("每个分区内的搭桥完毕，开始在分区间搭桥");
  std::stack<pairedEdge> interRegionEdges;
  for (int r = 0; r < rowCount; r++)
    for (int c = 0; c < colCount; c++) {
      if (r + 1 < rowCount) {
        pairedEdge temp =
            connectSingleMaps(algos[r][c],
                              rc_pos{static_cast<int32_t>(unitL * r),
                                     static_cast<int32_t>(unitL * c)},
                              algos[r + 1][c],
                              rc_pos{static_cast<int32_t>(unitL * (r + 1)),
                                     static_cast<int32_t>(unitL * c)});
        if (temp.lengthSquare > 2) interRegionEdges.emplace(temp);
      }
      if (c + 1 < colCount) {
        pairedEdge temp =
            connectSingleMaps(algos[r][c],
                              rc_pos{static_cast<int32_t>(unitL * r),
                                     static_cast<int32_t>(unitL * c)},
                              algos[r][c + 1],
                              rc_pos{static_cast<int32_t>(unitL * r),
                                     static_cast<int32_t>(unitL * (c + 1))});
        if (temp.lengthSquare > 2) interRegionEdges.emplace(temp);
      }
    }
  // qDebug()<<"分区间搭桥完毕，将搭建"<<interRegionEdges.size()<<"个分区间桥梁";

  // qDebug()<<"开始拼合各分区的结果";

  glassMap result;
  result.setZero(_targetMap.rows(), _targetMap.cols());

  if (walkable != nullptr)
    walkable->setZero(_targetMap.rows(), _targetMap.cols());

  for (int r = 0; r < rowCount; r++)
    for (int c = 0; c < colCount; c++) {
      /*qDebug()<<"result.block("<<unitL*r<<','<<unitL*c<<','
      <<targetMaps[r][c].rows()<<','<<targetMaps[r][c].cols()<<")=glassMaps["
      <<r<<"]["<<c<<"];";*/
      result.block(unitL * r, unitL * c, targetMaps[r][c].rows(),
                   targetMaps[r][c].cols()) = glassMaps[r][c];
      if (walkable != nullptr) {
        /*qDebug()<<"size(walkableMap)=["<<walkableMaps[r][c].rows()<<','<<walkableMaps[r][c].cols()<<"]";
        qDebug()<<"walkable->block("<<unitL*r<<','<<unitL*c<<','
        <<targetMaps[r][c].rows()<<','<<targetMaps[r][c].cols()<<")=walkableMaps["
        <<r<<"]["<<c<<"];";*/
        walkable->block(unitL * r, unitL * c, targetMaps[r][c].rows(),
                        targetMaps[r][c].cols()) = walkableMaps[r][c];
      }
    }
  // qDebug("开始绘制分区间的桥");

  while (!interRegionEdges.empty()) {
    interRegionEdges.top().drawEdge(result);
    if (walkable != nullptr) interRegionEdges.top().drawEdge(*walkable, true);
    interRegionEdges.pop();
  }
  //  // qDebug("拼合分区完毕，开始 delete 各个分区的 algo");
  //  for (int r = 0; r < rowCount; r++)
  //    for (int c = 0; c < colCount; c++) pgb.recycle(algos[r][c]);

  this->progress_bar.set_range(0, 100, 100);
  // qDebug()<<"用时"<<std::clock()-lastTime<<"毫秒";

  return result;
}

glassMap prim_glass_builder::make4SingleMap(const TokiMap &_targetMap,
                                            walkableMap *walkable) {
  if (_targetMap.rows() > unitL || _targetMap.cols() > unitL) {
    // qDebug("错误！make4SingleMap 不应当收到超过 unitL*unitL 的图");
    return glassMap(0, 0);
  }
  targetPoints.clear();
  for (short r = 0; r < _targetMap.rows(); r++)
    for (short c = 0; c < _targetMap.cols(); c++) {
      if (_targetMap(r, c)) {
        if (r > 1 && c > 1 && r + 1 < _targetMap.rows() &&
            c + 1 < _targetMap.cols() && _targetMap(r + 1, c) &&
            _targetMap(r - 1, c) && _targetMap(r, c + 1) &&
            _targetMap(r, c - 1))
          continue;
        else
          targetPoints.emplace_back(rc_pos{r, c});
      }
    }

  glassMap result(_targetMap.rows(), _targetMap.cols());
  result.setZero();

  if (targetPoints.size() > 1) {
    addEdgesToGraph();
    // std::cerr<<"edges.size="<<edges.size()<<std::endl;
    runPrim();
    // std::cerr<<"tree.size="<<tree.size()<<std::endl;
  } else {
    return result;
  }

  for (auto it = tree.cbegin(); it != tree.cend(); it++) it->drawEdge(result);

  for (auto it = targetPoints.cbegin(); it != targetPoints.cend(); it++)
    result(it->row, it->col) = 0;

  if (walkable != nullptr) *walkable = result;

  for (auto it = targetPoints.cbegin(); it != targetPoints.cend(); it++) {
    result(it->row, it->col) = 0;
    if (walkable != nullptr)
      walkable->operator()(it->row, it->col) = blockType::target;
  }
  return result;
}

pairedEdge prim_glass_builder::connectSingleMaps(const prim_glass_builder &map1,
                                                 rc_pos offset1,
                                                 const prim_glass_builder &map2,
                                                 rc_pos offset2) {
  if (map1.targetPoints.size() <= 0 || map2.targetPoints.size() <= 0)
    return pairedEdge();

  uint32_t offsetR1 = offset1.row, offsetC1 = offset1.col;
  uint32_t offsetR2 = offset2.row, offsetC2 = offset2.col;

  uint32_t r1, r2, c1, c2;

  pairedEdge current;

  pairedEdge min;
  min.lengthSquare = 0x7FFFFFFF;

  for (auto it = map1.targetPoints.cbegin(); it != map1.targetPoints.cend();
       it++)
    for (auto jt = map2.targetPoints.cbegin(); jt != map2.targetPoints.cend();
         jt++) {
      r1 = offsetR1 + it->row;
      c1 = offsetC1 + it->col;
      r2 = offsetR2 + jt->row;
      c2 = offsetC2 + jt->col;
      current = pairedEdge(r1, c1, r2, c2);
      if (current.lengthSquare <= 2) return current;

      if (min.lengthSquare > current.lengthSquare) min = current;
    }
  return min;
}

void prim_glass_builder::addEdgesToGraph() {
  edges.clear();
  //  edge::vertexes = std::addressof(targetPoints);
  // int taskCount=(targetPoints.size()*(targetPoints.size()-1))/2;
  // progressRangeSet(*windPtr,0,taskCount,0);
  for (uint32_t i = 0; i < targetPoints.size(); i++) {
    for (uint32_t j = i + 1; j < targetPoints.size(); j++) {
      edges.emplace_back(edge(i, j, this->targetPoints));
    }
    // emit keepAwake();
    // emit progressAdd(targetPoints.size()-i);
  }
  // qDebug("插入了所有的边");
}
void prim_glass_builder::runPrim() {
  tree.clear();
  tree.reserve(targetPoints.size() - 1);

  std::vector<bool> isFound(targetPoints.size(), false);
  isFound[0] = true;

  uint32_t foundCount = 1;

  // progressRangeSet(*windPtr,0,targetPoints.size(),0);

  std::stack<std::list<edge>::iterator> eraseTask;

  while (!eraseTask.empty()) eraseTask.pop();

  while (foundCount < targetPoints.size()) {
    while (!eraseTask.empty()) {
      edges.erase(eraseTask.top());
      eraseTask.pop();
    }

    auto selectedEdge = edges.begin();

    // 从列表中第一个元素开始搜索第一个可行边
    for (;;) {
      if (selectedEdge == edges.end()) {
        std::cerr << "Error: failed to find valid edge!\n";
        break;
      }
      // rc_pos z=selectedEdge->beg();
      // rc_pos w=selectedEdge->end();
      bool fz = isFound[(selectedEdge)->begIdx];
      bool fw = isFound[(selectedEdge)->endIdx];

      if (fz && fw) {
        eraseTask.emplace(selectedEdge);
        selectedEdge++;
        // 如果一条边的首尾都是已经被连接到的点，那么移除这条边
        continue;
      }
      bool uz = !fz;
      bool uw = !fw;
      if ((fz && uw) || (fw && uz)) {
        // 找到了第一条可行的边
        break;
      }
      selectedEdge++;
    }

    // 从找到的第一条边开始，寻找长度最小的可行边
    for (auto it = selectedEdge; it != edges.end();) {
      // if(selectedEdge->lengthSquare<=2)break;
      // rc_pos x=it->beg(),y=it->end();
      bool fx = isFound[(it)->begIdx];
      bool fy = isFound[(it)->endIdx];
      if (fx && fy) {
        eraseTask.emplace(it);
        it++;  // 如果一条边的首尾都是已经被连接到的点，那么移除这条边
        continue;
      }
      bool ux = !fx;
      bool uy = !fy;

      if ((fx && uy) || (fy && ux)) {
        if ((it)->lengthSquare < (selectedEdge)->lengthSquare)
          selectedEdge = it;
      }
      it++;
    }

    // 将选中边装入树中，
    // 并从集合 unsearched 中删除选中边的两个端点，
    // 向集合 found 中加入选中边的两个端点
    {
      // rc_pos x=selectedEdge->beg();
      // rc_pos y=selectedEdge->end();
      isFound[(selectedEdge)->begIdx] = true;
      isFound[(selectedEdge)->endIdx] = true;
      foundCount++;
      // found.emplace(x);
      // found.emplace(y);
      // unsearched.erase(x);
      // unsearched.erase(y);
      tree.emplace_back(*selectedEdge, this->targetPoints);
    }
    // if(foundCount%reportRate==0) {
    //     //progressRangeSet(*windPtr,0,targetPoints.size(),foundCount);
    //     emit keepAwake();
    // }
  }
  // qDebug("prim 算法完毕");
}

EImage TokiMap2EImage(const TokiMap &tm) {
  EImage result(tm.rows(), tm.cols());
  result.setConstant(airColor);
  for (uint32_t r = 0; r < tm.rows(); r++)
    for (uint32_t c = 0; c < tm.cols(); c++) {
      if (tm(r, c) == 1) result(r, c) = glassColor;
      if (tm(r, c) > 1) result(r, c) = targetColor;
    }
  return result;
}

glassMap connectBetweenLayers(const TokiMap &map1, const TokiMap &map2,
                              walkableMap *walkable) {
  std::list<rc_pos> target1, target2;
  target1.clear();
  target2.clear();
  for (int r = 0; r < map1.rows(); r++)
    for (int c = 0; c < map1.cols(); c++) {
      if (map1(r, c) >= prim_glass_builder::target)
        target1.emplace_back(rc_pos{r, c});
      if (map2(r, c) >= prim_glass_builder::target)
        target2.emplace_back(rc_pos{r, c});
    }
  std::list<pairedEdge> linkEdges;
  linkEdges.clear();
  pairedEdge min, temp;
  for (auto t1 = target1.cbegin(); t1 != target1.cend(); t1++) {
    min.lengthSquare = 0x7FFFFFFF;
    for (auto t2 = target2.cbegin(); t2 != target2.cend(); t2++) {
      temp = pairedEdge(*t1, *t2);
      if (min.lengthSquare > temp.lengthSquare) min = temp;
      if (min.lengthSquare <= 2) break;
    }
    linkEdges.emplace_back(min);
  }

  glassMap result;
  result.setZero(map1.rows(), map1.cols());

  for (auto it = linkEdges.cbegin(); it != linkEdges.cend(); it++)
    it->drawEdge(result);

  if (walkable != nullptr) *walkable = result;

  for (auto t = target1.cbegin(); t != target1.cend(); t++) {
    result(t->row, t->col) = prim_glass_builder::air;
    if (walkable != nullptr)
      walkable->operator()(t->row, t->col) = prim_glass_builder::target;
  }
  for (auto t = target2.cbegin(); t != target2.cend(); t++) {
    result(t->row, t->col) = prim_glass_builder::air;
    if (walkable != nullptr)
      walkable->operator()(t->row, t->col) = prim_glass_builder::target;
  }
  return result;
}

template <typename ele_t>
TokiMap impl_ySlice2TokiMap(const Eigen::Tensor<ele_t, 3> &raw) noexcept {
  assert(raw.dimension(2) == 1);

  TokiMap result(raw.dimension(0), raw.dimension(1));
  result.setZero();
  for (int i = 0; i < raw.size(); i++)
    if (raw(i) > 1) result(i) = prim_glass_builder::target;
  return result;
}

TokiMap ySlice2TokiMap_u16(const Eigen::Tensor<uint16_t, 3> &xzy,
                           std::span<const int, 3> start_xzy,
                           std::span<const int, 3> extension_xzy) noexcept {
  // assert(raw.dimension(2) == 1);
  assert(extension_xzy[2] == 1);

  TokiMap result;

  result.setZero(extension_xzy[0], extension_xzy[1]);
  for (int x = 0; x < result.rows(); x++) {
    for (int z = 0; z < result.cols(); z++) {
      if (xzy(x + start_xzy[0], z + start_xzy[1], start_xzy[2]) > 1)
        result(x, z) = prim_glass_builder::target;
    }
  }
  return result;
}