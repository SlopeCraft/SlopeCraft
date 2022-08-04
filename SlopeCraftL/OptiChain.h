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

#ifndef OPTICHAIN_H
#define OPTICHAIN_H
//#define showImg
//#define removeQt
//#define sendInfo

#include <iostream>
#include <string>
#include <list>
#include <queue>
#include "SCLDefines.h"

//using namespace std;
//using namespace Eigen;
enum RegionType {idp,Hang,Invalid};

#ifndef removeQt
extern ARGB isTColor;
extern ARGB isFColor;
extern ARGB WaterColor;
extern ARGB greyColor;

#endif
class Region
{
public:
    Region(short=-1,short=-1,RegionType=Invalid);
    short Beg;
    short End;
    RegionType type;
    bool isIDP() const;
    bool isHang() const;
    bool isValid() const;
    int size() const;
    short indexLocal2Global(short) const;
    short indexGlobal2Local(short) const;
    std::string toString() const;
};

class OptiChain
{
public:
    //Index一律以Height矩阵中的索引为准。Height(r,c+1)<->Base(r,c)
    //高度矩阵一律不含水柱顶的玻璃方块
    OptiChain(int Size=-1);//default Random Constructor
    OptiChain(const Eigen::ArrayXi & base,
              const Eigen::ArrayXi & High,const Eigen::ArrayXi & Low);
    ~OptiChain();

    void divideAndCompress();
    const Eigen::ArrayXi & getHighLine();
    const Eigen::ArrayXi & getLowLine();
    //ArrayXi toDepth() const;

    //static ArrayXXi Base;
    static const Eigen::Array3i Both;
    static const Eigen::Array3i Left;
    static const Eigen::Array3i Right;

#ifdef showImg
    //static QLabel *SinkIDP;
    static QLabel *SinkAll;
    static bool AllowSinkHang;
#endif

private:
    //int Col;
    Eigen::ArrayXi Base;
    Eigen::ArrayXi HighLine;
    Eigen::ArrayXi LowLine;
    std::queue<Region> Chain;//将一整列按水/空气切分为若干个大的孤立区间
    std::list<Region> SubChain;//将Chain中每个大的孤立区间切分为若干“最大单孤立区间”和“悬空区间”组成的串

    void divideToChain();
    void divideToSubChain();

    bool isAir(int index) const;
    bool isWater(int index) const;
    bool isSolidBlock(int index) const;

    void Sink(const Region&);
    int validHeight(int index) const;

    void dispSubChain() const;
//private:
    void divideToSubChain(const Region&);
};
#endif // OPTICHAIN_H
