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

#include "OptiChain.h"

#define NInf -100000
#define MapSize (Base.rows())
// ArrayXXi OptiChain::Base=MatrixXi::Zero(0,0);
const Eigen::Array3i OptiChain::Both(-1, 2, -1);
const Eigen::Array3i OptiChain::Left(-1, 1, 0);
const Eigen::Array3i OptiChain::Right(0, 1, -1);
// QLabel* OptiChain::SinkIDP=nullptr;

#ifdef showImg
QLabel *OptiChain::SinkAll = nullptr;
bool OptiChain::AllowSinkHang = false;
#else
#define AllowSinkHang true
#endif
Region::Region(short _Beg, short _End, RegionType _Type) {
  Beg = _Beg;
  End = _End;
  type = _Type;
}

inline bool Region::isHang() const { return (type == Hang); }

inline bool Region::isIDP() const { return (type == idp); }

inline bool Region::isValid() const {
  return (type != Invalid) && (size() >= 1);
}

inline int Region::size() const { return (End - Beg + 1); }

inline short Region::indexLocal2Global(short indexLocal) const {
  return indexLocal + Beg;
}

inline short Region::indexGlobal2Local(short indexGlobal) const {
  return indexGlobal - Beg;
}

std::string Region::toString() const {
  if (!isValid())
    return '{' + std::to_string(Beg) + ',' + std::to_string(End) +
           '}'; // 无效区间用大括号
  if (isHang())
    return '[' + std::to_string(Beg) + ',' + std::to_string(End) +
           ']'; // 悬空区间用中括号

  return '(' + std::to_string(Beg) + ',' + std::to_string(End) +
         ')'; // 可沉降区间用括号
}

OptiChain::OptiChain(int size) {
  SubChain.clear();
  if (size < 0)
    return;
}

OptiChain::OptiChain(const Eigen::ArrayXi &base, const Eigen::ArrayXi &High,
                     const Eigen::ArrayXi &Low) {
  Base = base;
  HighLine = High;
  LowLine = Low;
  SubChain.clear();
}
/*
OptiChain::OptiChain(const HeightLine& src) {
    HighLine=src.HighLine;
    LowLine=src.LowLine;
    Base=src.base;
    SubChain.clear();
}*/

OptiChain::~OptiChain() { return; }

const Eigen::ArrayXi &OptiChain::getHighLine() { return HighLine; }
const Eigen::ArrayXi &OptiChain::getLowLine() { return LowLine; }

int OptiChain::validHeight(int index) const {
  if (index < 0 || index >= MapSize)
    return NInf;
  if (isAir(index))
    return NInf;
  return HighLine(index);
}

inline bool OptiChain::isAir(int index) const {
  if (index < 0 || index >= MapSize)
    return true;
  return (Base(index) == 0);
}

inline bool OptiChain::isWater(int index) const {
  if (index < 0 || index >= MapSize)
    return false;
  return (Base(index) == 12);
}

inline bool OptiChain::isSolidBlock(int index) const {
  if (index < 0 || index >= MapSize)
    return false;
  return (Base(index) != 0 && Base(index) != 12);
}

void OptiChain::dispSubChain() const {
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
void HeightLine::segment2Brackets(list<Pair>&List,short sBeg,short sEnd)
{
    if(sEnd<sBeg||sBeg<0)return;
    List.clear();

    if(sBeg==sEnd)
    {
        List.push_back(Pair('(',sBeg));
        List.push_back(Pair(')',sEnd));
        return;
    }

    queue<Region>Pure;
    queue<Region> disPure;//极大值区间
    Region Temp;
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

inline void HeightLine::DealRegion(Region PR, list<Pair> &List)
{
    if(PR.Begin<0||PR.End<PR.Begin)return;
    List.push_back(Pair('(',PR.Begin));
    List.push_back(Pair(')',PR.End));
}
*/

void OptiChain::divideAndCompress() {
  divideToChain();
  while (!Chain.empty()) {
    divideToSubChain();

    for (auto it = SubChain.begin(); it != SubChain.end(); it++)
      if (it->isIDP())
        Sink(*it);

    for (auto it = SubChain.begin(); it != SubChain.end(); it++)
      if (AllowSinkHang && it->isHang())
        Sink(*it);
  }
#ifdef showImg

  int scaledH = SinkAll->height() - 2;
  int scaledW = SinkAll->width() - 2;
  SinkAll->setPixmap(QPixmap::fromImage(toQImage(3)).scaled(scaledW, scaledH));
#endif
}

void OptiChain::divideToChain() {
  while (!Chain.empty())
    Chain.pop();
  Region Temp(0, MapSize - 1, idp);
  for (int i = 1; i < MapSize; i++) {
    if (isAir(i)) {
      Temp.End = i - 1;
      Chain.push(Temp);
      Temp.Beg = i;
    }
    if (isWater(i)) {
      Temp.End = i - 1;
      Chain.push(Temp);
      Temp.Beg = i;
    }
    if (isSolidBlock(i) && isAir(i)) {
      Temp.End = i - 1;
      Chain.push(Temp);
      Temp.Beg = i;
    }
  }
  Temp.End = MapSize - 1;
  Chain.push(Temp);
#ifdef sendInfo
  std::cout << "Divided coloum "
            << " into " << Chain.size() << "isolated region(s)" << std::endl;
#endif
}

void OptiChain::divideToSubChain() {
  if (!Chain.front().isValid()) {
    Chain.pop();
    return;
  }
  SubChain.clear();

  divideToSubChain(Chain.front());
  Chain.pop();
}

void OptiChain::divideToSubChain(const Region &Cur) {
#ifdef sendInfo
  std::cout << "ready to analyse" << Cur.toString() << std::endl;
#endif
  if (Cur.size() <= 3) {
    SubChain.push_back(Region(Cur.Beg, Cur.End, idp));
#ifdef sendInfo
    std::cout << "Region" << Cur.toString()
              << " in Chain is too thin, sink directly." << std::endl;
#endif
    return;
  }

  Eigen::ArrayXi HL;
  HL.setZero(Cur.size() + 1);
  HL.segment(0, Cur.size()) = HighLine.segment(Cur.Beg, Cur.size());
  HL(Cur.size()) = NInf;

  Eigen::ArrayXi ScanBoth, ScanLeft, ScanRight;
  ScanBoth.setZero(Cur.size());
  ScanLeft.setZero(Cur.size());
  ScanRight.setZero(Cur.size());
  // qDebug("开始用三个一维算子扫描HighLine和LowLine");
  for (int i = 1; i < Cur.size(); i++) // 用三个算子扫描一个大孤立区间
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
  Region Temp(-1, -1, Hang); // 均写入绝对index而非相对index
  for (int i = 0; i < Cur.size(); i++) {
    if (!isReady && ScanLeft(i)) {
      isReady = true;
      Temp.Beg = Cur.indexLocal2Global(i);
      Temp.End = -1;
    }
    if (isReady && ScanRight(i)) {
      isReady = false;
      Temp.End = Cur.indexLocal2Global(i);
      SubChain.push_back(Temp);
      Temp.Beg = -1;
      Temp.End = -1;
    }
  }
  // qDebug("已将极大值区间串联成链，即将开始填充孤立区间。此时的SubChain为：");
  // dispSubChain();
  auto prev = SubChain.begin();
  for (auto it = SubChain.begin(); it != SubChain.end(); prev = it++) {
    if (it == SubChain.begin()) {
      Temp.Beg = Cur.indexLocal2Global(0);
      Temp.End = (it->Beg - 1);
      Temp.type = idp;
      if (Temp.isValid())
        SubChain.insert(it, Temp);
    } else {
      Temp.Beg = (prev->End + 1);
      Temp.End = (it->Beg - 1);
      Temp.type = idp;
      if (Temp.isValid())
        SubChain.insert(it, Temp);
    }
  }

  if (SubChain.size() <= 0) {
    SubChain.push_back(Cur);
  } else if (SubChain.back().End < Cur.End)
    SubChain.push_back(Region(SubChain.back().End + 1, Cur.End, idp));

#ifdef sendInfo
  std::cout << "SubChain constructed" << std::endl;
#endif
}

void OptiChain::Sink(const Region &Reg) {
  if (!Reg.isValid()) {
    std::cout << "Invalid region: " << Reg.toString() << std::endl;
    return;
  }
  if (Reg.isIDP()) {
    HighLine.segment(Reg.Beg, Reg.size()) -=
        LowLine.segment(Reg.Beg, Reg.size()).minCoeff();
    LowLine.segment(Reg.Beg, Reg.size()) -=
        LowLine.segment(Reg.Beg, Reg.size()).minCoeff();
    return;
  }
  if (AllowSinkHang && Reg.isHang()) {
    int BegGap;

    BegGap = validHeight(Reg.Beg) - validHeight(Reg.Beg - 1);

    int EndGap;
    if (isSolidBlock(Reg.End + 1))
      EndGap = validHeight(Reg.End) - validHeight(Reg.End + 1);
    else
      EndGap = validHeight(Reg.End) - NInf;
    int offset = std::min(std::max(std::min(BegGap, EndGap) - 1, 0),
                          LowLine.segment(Reg.Beg, Reg.size()).minCoeff());
    HighLine.segment(Reg.Beg, Reg.size()) -= offset;
    LowLine.segment(Reg.Beg, Reg.size()) -= offset;
  }
}
