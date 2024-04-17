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

#ifndef OPTICHAIN_H
#define OPTICHAIN_H
// #define showImg
// #define removeQt
// #define sendInfo

#include "SCLDefines.h"
#include <iostream>
#include <list>
#include <queue>
#include <string>

// using namespace std;
// using namespace Eigen;
enum class region_type { independent, hanging };

#ifndef removeQt
extern ARGB isTColor;
extern ARGB isFColor;
extern ARGB WaterColor;
extern ARGB greyColor;

#endif
class region {
 public:
  region() = delete;
  region(int b, int e, region_type t) : begin{b}, end{e}, type{t} {}
  int begin;
  int end;
  region_type type;
  bool isIDP() const;
  bool isHang() const;
  bool isValid() const;
  int size() const;
  int indexLocal2Global(int) const;
  int indexGlobal2Local(int) const;
  std::string toString() const;
};

class optimize_chain {
 public:
  // Index一律以Height矩阵中的索引为准。Height(r,c+1)<->Base(r,c)
  // 高度矩阵一律不含水柱顶的玻璃方块
  optimize_chain(int Size = -1);  // default Random Constructor
  optimize_chain(const Eigen::ArrayXi &base, const Eigen::ArrayXi &High,
                 const Eigen::ArrayXi &Low);
  ~optimize_chain();

  void divide_and_compress();
  const Eigen::ArrayXi &high_line();
  const Eigen::ArrayXi &low_line();
  // ArrayXi toDepth() const;

  // static ArrayXXi Base;
  static const Eigen::Array3i Both;
  static const Eigen::Array3i Left;
  static const Eigen::Array3i Right;

#ifdef showImg
  // static QLabel *SinkIDP;
  static QLabel *SinkAll;
  static bool AllowSinkHang;
#endif

 private:
  // int Col;
  Eigen::ArrayXi Base;
  Eigen::ArrayXi HighLine;
  Eigen::ArrayXi LowLine;
  std::queue<region> Chain;  // 将一整列按水/空气切分为若干个大的孤立区间
  std::list<region>
      SubChain;  // 将Chain中每个大的孤立区间切分为若干“最大单孤立区间”和“悬空区间”组成的串

  void divideToChain();
  void divide_into_subchain();

  bool is_air(int index) const;
  bool is_water(int index) const;
  bool is_solid_block(int index) const;

  void sink(const region &);
  int valid_height(int index) const;

  void dispSubChain() const;
  // private:
  void divide_into_subchain(const region &);
};
#endif  // OPTICHAIN_H
