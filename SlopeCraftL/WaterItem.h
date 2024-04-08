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

#ifndef WATERITEM_H
#define WATERITEM_H

#include <stdint.h>

using TokiPos = uint32_t;
// typedef unsigned int TokiPos;
//  typedef unsigned int waterItem;
using waterItem = uint32_t;
constexpr TokiPos TokiRC(int row, int col) {  // 前16bit存储row，后16bit存储col
  /*unsigned int u;
  *((int16_t*)&u)=row;
  *(((int16_t*)&u)+1)=col;
  return u;*/
  return (row << 16) | (col & 0x0000FFFF);
}

constexpr int16_t TokiRow(TokiPos pos) { return pos >> 16; }
constexpr int16_t TokiCol(TokiPos pos) { return pos & 0x0000FFFF; }

constexpr TokiPos nullPos = TokiRC(-1, -1);
constexpr waterItem nullWater = TokiRC(-32768, -32768);
constexpr int16_t WaterColumnSize[3] = {11, 6, 1};

constexpr waterItem (*TokiWater)(int, int) = TokiRC;
constexpr int16_t (*waterHigh)(waterItem) = TokiRow;
constexpr int16_t (*waterLow)(waterItem) = TokiCol;
/*
waterItem TokiWater(int16_t high,int16_t
low);//前两字节存储high，后两字节存储low int16_t waterHigh(waterItem); int16_t
waterLow(waterItem);*/

#endif  // WATERITEM_H
