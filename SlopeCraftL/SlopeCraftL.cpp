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

#include "SlopeCraftL.h"
#include "simpleBlock.h"
#include "TokiSlopeCraft.h"
using namespace SlopeCraft;

AbstractBlock * AbstractBlock::create() {
    return new simpleBlock;
}

AbstractBlock::AbstractBlock() {}

void AbstractBlock::copyTo(AbstractBlock *dst) const {
    dst->setBurnable(getBurnable());
    dst->setDoGlow(getDoGlow());
    dst->setEndermanPickable(getEndermanPickable());
    dst->setId(getId());
    dst->setIdOld(getIdOld());
    dst->setNeedGlass(getNeedGlass());
    dst->setVersion(getVersion());
    dst->setWallUseable(getWallUseable());
}

void AbstractBlock::clear() {
    setBurnable(false);
    setDoGlow(false);
    setEndermanPickable(false);
    setId("minecraft:air");
    setIdOld("");
    setNeedGlass(false);
    setVersion(0);
    setWallUseable(false);
}

Kernel::Kernel() {
}

void Kernel::getColorMapPtrs(const float** f,const unsigned char** m,int* rows) {
    *f=TokiSlopeCraft::Allowed._RGB.data();
    *m=TokiSlopeCraft::Allowed.Map.data();
    *rows=TokiSlopeCraft::Allowed.Map.size();
}

const float * Kernel::getBasicColorMapPtrs() {
    return TokiSlopeCraft::Basic._RGB.data();
}

Kernel * Kernel::create() {
    return (new TokiSlopeCraft)->toBaseClassPtr();
}
