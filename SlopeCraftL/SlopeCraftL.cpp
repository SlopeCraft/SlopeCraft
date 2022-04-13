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


namespace SlopeCraft
{
const ColorList *const Basic4External=(ColorList *)BasicalRGBList4AiCvters();
const ColorList *const Allowed4External=(ColorList *)AllowedRGBList4AiCvters();

void * SCL_EXPORT AllowedRGBList4AiCvters() {
    return &TokiSlopeCraft::Allowed._RGB;
}

void * SCL_EXPORT BasicalRGBList4AiCvters() {
    return &TokiSlopeCraft::Basic._RGB;
}
}   //  end namespace SlopeCraft

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

const char * Kernel::getSCLVersion() {
    return "v3.7.0";
}

void Kernel::getColorMapPtrs(const float** f,const unsigned char** m,int* rows) {
    if(f!=nullptr)
        *f=TokiSlopeCraft::Allowed._RGB.data();
    if(m!=nullptr)
        *m=TokiSlopeCraft::Allowed.Map.data();
    if(rows!=nullptr)
        *rows=TokiSlopeCraft::Allowed.Map.size();
}

const float * Kernel::getBasicColorMapPtrs() {
    return TokiSlopeCraft::Basic._RGB.data();
}

Kernel * Kernel::create() {
    return (new TokiSlopeCraft)->toBaseClassPtr();
}


uint64_t Kernel::mcVersion2VersionNumber(gameVersion g) {
    switch (g) {
    case gameVersion::ANCIENT:
        return 114514;
    case gameVersion::MC12:
        return 1631;
    case gameVersion::MC13:
        return 1976;
    case gameVersion::MC14:
        return 2230;
    case gameVersion::MC15:
        return 2230;
    case gameVersion::MC16:
        return 2586;
    case gameVersion::MC17:
        return 2730;
    case gameVersion::MC18:
        return 2865;
    default:
        return 1919810;
    }
}

