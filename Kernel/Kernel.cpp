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

#include "Kernel.h"
#include "simpleBlock.h"
#include "TokiSlopeCraft.h"

using namespace SlopeCraft;

AbstractBlock::AbstractBlock() {

#ifdef STRAIGHT_INCLUDE_KERNEL
#ifndef NO_DLL
#error YOU MUST DEFINE NO_DLL WHEN DEFINED STRAIGHT_INCLUDE_KERNEL
#endif
#endif

}
#ifndef NO_DLL
AbstractBlock * AbstractBlock::createSimpleBlock() {
    return new simpleBlock;
}
#endif
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

void AbstractBlock::setEmpty() {
    setBurnable(false);
    setDoGlow(false);
    setEndermanPickable(false);
    setId("minecraft:air");
    setIdOld("");
    setNeedGlass(false);
    setVersion(0);
    setWallUseable(false);
}

#ifdef WITH_QT
Kernel::Kernel(QObject *parent) : QObject(parent)
#else
Kernel::Kernel()
#endif
{

}
#ifndef NO_DLL
#ifdef WITH_QT
Kernel * createKernel(QObject * parent) {
    return new TokiSlopeCraft(parent);
};
#else
Kernel * Kernel::createKernel() {
    return new TokiSlopeCraft;
};
#endif

#endif
