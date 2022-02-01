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

#ifndef LOSSYCOMPRESSOR_H
#define LOSSYCOMPRESSOR_H


#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <random>

#include "defines.h"
#include "WaterItem.h"
#include "ColorSet.h"
#include "HeightLine.h"

//Eigen::Array<uchar,Eigen::Dynamic,1>
class solver_t;

class LossyCompressor
{
public:
    LossyCompressor();
    ~LossyCompressor();
    void setSource(const Eigen::ArrayXi & ,const TokiColor *[]);
    bool compress(ushort maxHeight,bool allowNaturalCompress=false);
    const Eigen::ArrayX<uchar> & getResult() const;
    double resultFitness() const;

    void ** windPtr;
    void (**progressRangeSetPtr)(void*,int min,int max,int val);
    void (**progressAddPtr)(void*,int);
    void (**keepAwakePtr)(void*);

private:
    solver_t * solver;
    std::vector<const TokiColor*> source;

    static ushort maxGeneration;

    void runGenetic(ushort maxHeight,bool allowNaturalCompress);
};

double randD();
#endif // LOSSYCOMPRESSOR_H
