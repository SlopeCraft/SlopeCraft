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

#ifndef WATERITEM_H
#define WATERITEM_H

typedef unsigned int TokiPos;
typedef unsigned int waterItem;
extern const TokiPos nullPos;
extern const waterItem nullWater;
extern const short WaterColumnSize[3];
TokiPos TokiRC(int row, int col); //前16bit存储row，后16bit存储col
short TokiRow(TokiPos);
short TokiCol(TokiPos);

extern waterItem (*TokiWater)(int, int);
extern short (*waterHigh)(waterItem);
extern short (*waterLow)(waterItem);
/*
waterItem TokiWater(short high,short low);//前两字节存储high，后两字节存储low
short waterHigh(waterItem);
short waterLow(waterItem);*/

#endif // WATERITEM_H
