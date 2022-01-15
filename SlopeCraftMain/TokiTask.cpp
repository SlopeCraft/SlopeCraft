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

#include "TokiTask.h"

bool TokiTask::canExportLite=true;
const bool TokiTask::canExportData=true;

TokiTask::TokiTask() {

}

TokiTask::~TokiTask() {

}

TokiPos TokiRC(int row,int col)
{
    /*unsigned int u;
    *((short*)&u)=row;
    *(((short*)&u)+1)=col;
    return u;*/
    return (row<<16)|(col&0x0000FFFF);
}
short TokiRow(TokiPos pos)
{
    return pos>>16;
}
short TokiCol(TokiPos pos)
{
    return pos&0x0000FFFF;
}

uint TokiTask::mapCount() const {
    return  std::ceil(rows()/128.0)*std::ceil(cols()/128.0);
}

ushort TokiTask::rows() const {
    return TokiRow(src_imageSize);
}

ushort TokiTask::cols() const {
    return TokiCol(src_imageSize);
}

void TokiTask::clear() {
    src_imageName="";
    src_imageSize=TokiRC(0,0);
    dst_DataFileName="";
    dst_liteFileName="";
    dst_beginSeqNum=0;
}
