/*
 Copyright © 2021  TokiNoBug
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
#define removeQt

#include <iostream>
#include <string>
#include <Eigen/Dense>
#include <list>
#include <queue>

#ifndef removeQt
    #include <QDebug>
    #include <QRgb>
    #include <QImage>
    #include <QLabel>
#endif
using namespace std;
using namespace Eigen;
enum RegionType {idp,Hang,Invalid};

#ifndef removeQt
extern QRgb isTColor;
extern QRgb isFColor;
extern QRgb WaterColor;
extern QRgb greyColor;

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
    string toString() const;
};

class OptiChain
{
public:
    //Index一律以Height矩阵中的索引为准。Height(r,c+1)<->Base(r,c)
    //高度矩阵一律不含水柱顶的玻璃方块
    OptiChain(int Size=-1);//default Random Constructor
    OptiChain(const ArrayXi & base, const ArrayXi & High,const ArrayXi & Low);
    ~OptiChain();

    void divideAndCompress();
    const ArrayXi & getHighLine();
    const ArrayXi & getLowLine();
    //ArrayXi toDepth() const;

    //static ArrayXXi Base;
    static const Array3i Both;
    static const Array3i Left;
    static const Array3i Right;

#ifdef showImg
    //static QLabel *SinkIDP;
    static QLabel *SinkAll;
    static bool AllowSinkHang;
#endif

private:
    //int Col;
    ArrayXi Base;
    ArrayXi HighLine;
    ArrayXi LowLine;
    queue<Region> Chain;//将一整列按水/空气切分为若干个大的孤立区间
    list<Region> SubChain;//将Chain中每个大的孤立区间切分为若干“最大单孤立区间”和“悬空区间”组成的串

    void divideToChain();
    void divideToSubChain();

    bool isAir(int index) const;
    bool isWater(int index) const;
    bool isSolidBlock(int index) const;

    void Sink(const Region&);
    int validHeight(int index) const;
#ifndef removeQt
    QImage toQImage(int pixelSize);
#endif
    void dispSubChain() const;
//private:
    void divideToSubChain(const Region&);
};
#ifndef removeQt
QImage Mat2Image(const ArrayXXi&,int pixelSize);
#endif
#endif // OPTICHAIN_H
