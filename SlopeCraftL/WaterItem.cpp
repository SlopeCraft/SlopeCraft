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

#include "WaterItem.h"

const TokiPos nullPos = TokiRC(-1, -1);
const waterItem nullWater = TokiRC(-32768, -32768);
const short WaterColumnSize[3] = {11, 6, 1};
waterItem (*TokiWater)(int, int) = TokiRC;
short (*waterHigh)(waterItem) = TokiRow;
short (*waterLow)(waterItem) = TokiCol;
TokiPos TokiRC(int row, int col) {
  /*unsigned int u;
  *((short*)&u)=row;
  *(((short*)&u)+1)=col;
  return u;*/
  return (row << 16) | (col & 0x0000FFFF);
}
short TokiRow(TokiPos pos) { return pos >> 16; }
short TokiCol(TokiPos pos) { return pos & 0x0000FFFF; }
/*
waterItem TokiWater(short high,short low)
{
    unsigned int u;
    *((short*)&u)=high;
    *(((short*)&u)+1)=low;
    return u;
}
short waterHigh(waterItem item)
{
    return *((short*)&item);
}
short waterLow(waterItem item)
{
    return *(((short*)&item)+1);
}
*/
