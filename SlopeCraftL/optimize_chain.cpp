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

#include "optimize_chain.h"

#define NInf -100000
#define MapSize (Base.rows())
// ArrayXXi optimize_chain::Base=MatrixXi::Zero(0,0);
const Eigen::Array3i optimize_chain::Both(-1, 2, -1);
const Eigen::Array3i optimize_chain::Left(-1, 1, 0);
const Eigen::Array3i optimize_chain::Right(0, 1, -1);
// QLabel* optimize_chain::SinkIDP=nullptr;

#ifdef showImg
QLabel *optimize_chain::SinkAll = nullptr;
bool optimize_chain::AllowSinkHang = false;
#else
#define AllowSinkHang true
#endif

inline bool region::isHang() const { return (type == region_type::hanging); }

inline bool region::isIDP() const { return (type == region_type::independent); }

inline bool region::isValid() const { return (size() >= 1); }

inline int region::size() const { return (end - begin + 1); }

inline int region::indexLocal2Global(int indexLocal) const {
  return indexLocal + begin;
}

inline int region::indexGlobal2Local(int indexGlobal) const {
  return indexGlobal - begin;
}

std::string region::toString() const {
  if (!isValid())
    return '{' + std::to_string(begin) + ',' + std::to_string(end) +
           '}';  // 无效区间用大括号
  if (isHang())
    return '[' + std::to_string(begin) + ',' + std::to_string(end) +
           ']';  // 悬空区间用中括号

  return '(' + std::to_string(begin) + ',' + std::to_string(end) +
         ')';  // 可沉降区间用括号
}

optimize_chain::optimize_chain(int size) {
  SubChain.clear();
  if (size < 0) return;
}

optimize_chain::optimize_chain(const Eigen::ArrayXi &base,
                               const Eigen::ArrayXi &High,
                               const Eigen::ArrayXi &Low) {
  Base = base;
  HighLine = High;
  LowLine = Low;
  SubChain.clear();
}
/*
optimize_chain::optimize_chain(const height_line& src) {
    HighLine=src.HighLine;
    LowLine=src.LowLine;
    Base=src.base;
    SubChain.clear();
}*/

optimize_chain::~optimize_chain() { return; }

const Eigen::ArrayXi &optimize_chain::high_line() { return HighLine; }
const Eigen::ArrayXi &optimize_chain::low_line() { return LowLine; }

int optimize_chain::valid_height(int index) const {
  if (index < 0 || index >= MapSize) return NInf;
  if (is_air(index)) return NInf;
  return HighLine(index);
}

inline bool optimize_chain::is_air(int index) const {
  if (index < 0 || index >= MapSize) return true;
  return (Base(index) == 0);
}

inline bool optimize_chain::is_water(int index) const {
  if (index < 0 || index >= MapSize) return false;
  return (Base(index) == 12);
}

inline bool optimize_chain::is_solid_block(int index) const {
  if (index < 0 || index >= MapSize) return false;
  return (Base(index) != 0 && Base(index) != 12);
}

void optimize_chain::dispSubChain() const {
  std::string out = "";
  for (auto it = SubChain.cbegin(); it != SubChain.cend(); it++) {
    out += it->toString();
  }
#ifdef sendInfo
  std::cout << out << std::endl;
#endif
  // cout<<out<<endl;
}

/*
void height_line::segment2Brackets(list<Pair>&List,short sBeg,short sEnd)
{
    if(sEnd<sBeg||sBeg<0)return;
    List.clear();

    if(sBeg==sEnd)
    {
        List.push_back(Pair('(',sBeg));
        List.push_back(Pair(')',sEnd));
        return;
    }

    queue<region>Pure;
    queue<region> disPure;//极大值区间
    region Temp;
    bool isReady=false;
    VectorXi VHL=ValidHighLine();
    VectorXi ScanBoth=VHL,ScanLeft=VHL,ScanRight=VHL;
    ScanBoth.setZero();ScanLeft.setZero();ScanRight.setZero();
    for(int i=sBeg+1;i<sEnd-1;i++)//分别用三个算子处理
    {
        ScanBoth(i)=(VHL.segment(i-1,3).array()*Both.array()).sum();
        ScanLeft(i)=(VHL.segment(i-1,3).array()*Left.array()).sum();
        ScanRight(i)=(VHL.segment(i-1,3).array()*Right.array()).sum();
    }
ScanBoth=(ScanBoth.array()>=0).select(ScanBoth,0);
ScanLeft=(ScanLeft.array()>=0).select(ScanLeft,0);
ScanRight=(ScanRight.array()>=0).select(ScanRight,0);
    isReady=false;
    for(int i=sBeg+1;i<sEnd-1;i++)
    {
        if(!isReady&&ScanBoth(i)&&ScanLeft(i))
        {
            isReady=true;
            Temp.Begin=i;
        }
        if(isReady&&ScanBoth(i)&&ScanRight(i))
        {
            Temp.End=i;
            disPure.push(Temp);
            Temp.Begin=-1;
            Temp.End=-1;
            isReady=false;
        }
    }
    Temp.Begin=sBeg;
    Temp.End=sEnd;
    while(!disPure.empty())
    {
        Temp.End=disPure.front().Begin-1;
        Pure.push(Temp);
        Temp.Begin=disPure.front().End+1;
        disPure.pop();
        if(Temp.Begin>=sEnd)Temp.Begin=sEnd;
        Temp.End=sEnd;
    }
    Pure.push(Temp);

    while(!Pure.empty())
    {
        DealRegion(Pure.front(),List);
#ifndef NoOutPut
        cout<<'['<<Pure.front().Begin<<','<<Pure.front().End<<']'<<"->";
#endif
        Pure.pop();
    }
    List.push_front(Pair('(',sBeg));
    List.push_back(Pair(')',sEnd));
#ifndef NoOutPut
    disp(List);
#endif
}

inline void height_line::DealRegion(region PR, list<Pair> &List)
{
    if(PR.Begin<0||PR.End<PR.Begin)return;
    List.push_back(Pair('(',PR.Begin));
    List.push_back(Pair(')',PR.End));
}
*/

void optimize_chain::divide_and_compress() {
  divideToChain();
  while (!Chain.empty()) {
    divide_into_subchain();

    for (auto it = SubChain.begin(); it != SubChain.end(); it++)
      if (it->isIDP()) sink(*it);

    for (auto it = SubChain.begin(); it != SubChain.end(); it++)
      if (AllowSinkHang && it->isHang()) sink(*it);
  }
#ifdef showImg

  int scaledH = SinkAll->height() - 2;
  int scaledW = SinkAll->width() - 2;
  SinkAll->setPixmap(QPixmap::fromImage(toQImage(3)).scaled(scaledW, scaledH));
#endif
}

void optimize_chain::divideToChain() {
  while (!Chain.empty()) Chain.pop();
  region Temp(0, MapSize - 1, region_type::independent);
  for (int i = 1; i < MapSize; i++) {
    if (is_air(i)) {
      Temp.end = i - 1;
      Chain.push(Temp);
      Temp.begin = i;
    }
    if (is_water(i)) {
      Temp.end = i - 1;
      Chain.push(Temp);
      Temp.begin = i;
    }
    if (is_solid_block(i) && is_air(i)) {
      Temp.end = i - 1;
      Chain.push(Temp);
      Temp.begin = i;
    }
  }
  Temp.end = MapSize - 1;
  Chain.push(Temp);
#ifdef sendInfo
  std::cout << "Divided coloum " << " into " << Chain.size()
            << "isolated region(s)" << std::endl;
#endif
}

void optimize_chain::divide_into_subchain() {
  if (!Chain.front().isValid()) {
    Chain.pop();
    return;
  }
  SubChain.clear();

  divide_into_subchain(Chain.front());
  Chain.pop();
}

void optimize_chain::divide_into_subchain(const region &Cur) {
#ifdef sendInfo
  std::cout << "ready to analyse" << Cur.toString() << std::endl;
#endif
  if (Cur.size() <= 3) {
    SubChain.push_back(region(Cur.begin, Cur.end, region_type::independent));
#ifdef sendInfo
    std::cout << "region" << Cur.toString()
              << " in Chain is too thin, sink directly." << std::endl;
#endif
    return;
  }

  Eigen::ArrayXi HL;
  HL.setZero(Cur.size() + 1);
  HL.segment(0, Cur.size()) = HighLine.segment(Cur.begin, Cur.size());
  HL(Cur.size()) = NInf;

  Eigen::ArrayXi ScanBoth, ScanLeft, ScanRight;
  ScanBoth.setZero(Cur.size());
  ScanLeft.setZero(Cur.size());
  ScanRight.setZero(Cur.size());
  // qDebug("开始用三个一维算子扫描HighLine和LowLine");
  for (int i = 1; i < Cur.size(); i++)  // 用三个算子扫描一个大孤立区间
  {
    ScanBoth(i) = (HL.segment(i - 1, 3) * Both).sum() > 0;
    ScanLeft(i) = (HL.segment(i - 1, 3) * Left).sum() > 0;
    ScanRight(i) = (HL.segment(i - 1, 3) * Right).sum() > 0;
  }
  ScanLeft *= ScanBoth;
  ScanRight *= ScanBoth;

#ifdef sendInfo
  std::cout << "scanning finished" << std::endl;
#endif

  bool isReady = false;

  // 表示已经检测出极大值区间的入口，找到出口就装入一个极大值区间
  region Temp(-1, -1, region_type::hanging);  // 均写入绝对index而非相对index
  for (int i = 0; i < Cur.size(); i++) {
    if (!isReady && ScanLeft(i)) {
      isReady = true;
      Temp.begin = Cur.indexLocal2Global(i);
      Temp.end = -1;
    }
    if (isReady && ScanRight(i)) {
      isReady = false;
      Temp.end = Cur.indexLocal2Global(i);
      SubChain.push_back(Temp);
      Temp.begin = -1;
      Temp.end = -1;
    }
  }
  // qDebug("已将极大值区间串联成链，即将开始填充孤立区间。此时的SubChain为：");
  // dispSubChain();
  auto prev = SubChain.begin();
  for (auto it = SubChain.begin(); it != SubChain.end(); prev = it++) {
    if (it == SubChain.begin()) {
      Temp.begin = Cur.indexLocal2Global(0);
      Temp.end = (it->begin - 1);
      Temp.type = region_type::independent;
      if (Temp.isValid()) SubChain.insert(it, Temp);
    } else {
      Temp.begin = (prev->end + 1);
      Temp.end = (it->begin - 1);
      Temp.type = region_type::independent;
      if (Temp.isValid()) SubChain.insert(it, Temp);
    }
  }

  if (SubChain.size() <= 0) {
    SubChain.push_back(Cur);
  } else if (SubChain.back().end < Cur.end)
    SubChain.push_back(
        region(SubChain.back().end + 1, Cur.end, region_type::independent));

#ifdef sendInfo
  std::cout << "SubChain constructed" << std::endl;
#endif
}

void optimize_chain::sink(const region &Reg) {
  if (!Reg.isValid()) {
    std::cout << "invalid region: " << Reg.toString() << std::endl;
    return;
  }
  if (Reg.isIDP()) {
    HighLine.segment(Reg.begin, Reg.size()) -=
        LowLine.segment(Reg.begin, Reg.size()).minCoeff();
    LowLine.segment(Reg.begin, Reg.size()) -=
        LowLine.segment(Reg.begin, Reg.size()).minCoeff();
    return;
  }
  if (AllowSinkHang && Reg.isHang()) {
    int BegGap;

    BegGap = valid_height(Reg.begin) - valid_height(Reg.begin - 1);

    int EndGap;
    if (is_solid_block(Reg.end + 1))
      EndGap = valid_height(Reg.end) - valid_height(Reg.end + 1);
    else
      EndGap = valid_height(Reg.end) - NInf;
    int offset = std::min(std::max(std::min(BegGap, EndGap) - 1, 0),
                          LowLine.segment(Reg.begin, Reg.size()).minCoeff());
    HighLine.segment(Reg.begin, Reg.size()) -= offset;
    LowLine.segment(Reg.begin, Reg.size()) -= offset;
  }
}
