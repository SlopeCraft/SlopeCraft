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
#ifndef TOKICOLOR_H
#define TOKICOLOR_H
#include <iostream>
#include "defines.h"


ARGB ARGB32(uint r,uint g,uint b,uint a=255);
uchar getA(ARGB);
uchar getR(ARGB);
uchar getG(ARGB);
uchar getB(ARGB);


class ColorSet;
class TokiColor
{
public:
    TokiColor(ARGB,char);
    TokiColor();
    //QRgb Raw;//相当于Key
    float c3[3];//三通道的颜色值。可能为RGB,HSV,Lab,XYZ
    float sideSelectivity[2];//记录与result的深度值不同的两个有损优化候选色选择系数（升序排列），Depth=3时无效
    unsigned char sideResult[2];//记录与result的深度值不同的两个有损优化候选色（升序排列），Depth=3时无效
    char ColorSpaceType;
    unsigned char Result;//最终调色结果
    float ResultDiff;
    static bool needFindSide;
    static ColorSet * Basic;
    static ColorSet * Allowed;
    //static short DepthIndexEnd[4];
    static unsigned char DepthCount[4];
    unsigned char apply(ARGB);
private:
    unsigned char apply();
    unsigned char applyRGB();
    unsigned char applyRGB_plus();
    unsigned char applyHSV();
    unsigned char applyXYZ();
    unsigned char applyLab_old();
    unsigned char applyLab_new();
    void doSide(Eigen::VectorXf);
};

#endif // TOKICOLOR_H
