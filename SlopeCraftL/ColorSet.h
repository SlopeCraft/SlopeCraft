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

#ifndef COLORSET_H
#define COLORSET_H
#include "defines.h"
#include "TokiColor.h"

//using namespace Eigen;

class ColorSet{
    public:
        ColorList _RGB;
        ColorList  HSV;
        ColorList  Lab;
        ColorList  XYZ;
        MapList  Map;
        ColorSet();
        bool ApplyAllowed(ColorSet*standard,bool *MIndex);
        uint16_t colorCount() const;
};

void GetMap(unsigned char *Map);

void GetMap(MapList &Map);

void f(float &);
void invf(float&);
void RGB2HSV(float, float, float,  float &, float &, float &);
void HSV2RGB(float,float,float,float&,float&,float&);
void RGB2XYZ(float R, float G, float B, float &X, float &Y, float &Z);
void XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b);
void Lab2XYZ(float L,float a,float b,float&X,float&Y,float&Z);

float Lab00_diff(float,float,float,float,float,float);

float squeeze01(float);
ARGB RGB2ARGB(float,float,float);
ARGB HSV2ARGB(float,float,float);
ARGB XYZ2ARGB(float,float,float);
ARGB Lab2ARGB(float,float,float);
ARGB ComposeColor(const ARGB&front,const ARGB&back);
bool readFromFile(const char*FileName,Eigen::ArrayXXf & M);
bool readFromTokiColor(const char*,Eigen::ArrayXXf &,const std::string &);
uint8_t h2d(char);
#endif // COLORSET_H
