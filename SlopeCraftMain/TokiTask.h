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

#ifndef TOKITASK_H
#define TOKITASK_H
#include <QString>
#include <cmath>
#include "WaterItem.h"

class TokiTask
{
public:
    TokiTask();
    ~TokiTask();
    //friend class BatchOperator;
    uint mapCount() const;
    ushort rows() const;
    ushort cols() const;
    void clear();

    QString src_imageName;
    TokiPos src_imageSize;
    QString dst_liteFileName;
    QString dst_DataFileName;
    uint dst_beginSeqNum;

    static bool canExportLite;
    static const bool canExportData;

};
#endif // TOKITASK_H
