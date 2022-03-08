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

#ifdef SLOPECRAFTL_CAPI
extern "C" {
AbstractBlock * SCL_EXPORT createBlock() {
    return new simpleBlock;
}

unsigned long long SCL_EXPORT size(const AbstractBlock * t) {
    return t->size();
}

///id of this block
const char* SCL_EXPORT getId(const AbstractBlock * t) {
    return t->getId();
}

///first version
unsigned char SCL_EXPORT getVersion(const AbstractBlock * t) {
    return t->getVersion();
}
///id in 1.12
const char* SCL_EXPORT getIdOld(const AbstractBlock * t) {
    return t->getIdOld();
}
///if this block needs a glass block under it
bool SCL_EXPORT getNeedGlass(const AbstractBlock * t) {
    return t->getNeedGlass();
}
///if this block emits light
bool SCL_EXPORT getDoGlow(const AbstractBlock * t) {
    return t->getDoGlow();
}
///if this block can be stolen by enderman
bool SCL_EXPORT getEndermanPickable(const AbstractBlock * t) {
    return t->getEndermanPickable();
}
///if this block can be burnt
bool SCL_EXPORT getBurnable(const AbstractBlock * t) {
    return t->getBurnable();
}
///if this block can be used in wall-map
bool SCL_EXPORT getWallUseable(const AbstractBlock * t) {
    return t->getWallUseable();
}

///set block id
void SCL_EXPORT setId(AbstractBlock * t,const char* id) {
    t->setId(id);
}
///set first version
void SCL_EXPORT setVersion(AbstractBlock * t,unsigned char v) {
    t->setVersion(v);
}
///set id in 1.12
void SCL_EXPORT setIdOld(AbstractBlock * t,const char* io) {
    t->setIdOld(io);
}
///set if this block needs a glass block under it
void SCL_EXPORT setNeedGlass(AbstractBlock * t,bool ng) {
    t->setNeedGlass(ng);
}
///set if this block emits light
void SCL_EXPORT setDoGlow(AbstractBlock * t,bool dg) {
    t->setDoGlow(dg);
}
///set if this block can be stolen by enderman
void SCL_EXPORT setEndermanPickable(AbstractBlock * t,bool ep) {
    t->setEndermanPickable(ep);
}
///set if this block can be burnt
void SCL_EXPORT setBurnable(AbstractBlock * t,bool b) {
    t->setBurnable(b);
}
///set if this block can be used in wall-map
void SCL_EXPORT setWallUseable(AbstractBlock * t,bool wu) {
    t->setWallUseable(wu);
}
///let *b equal to *this
void SCL_EXPORT copyTo(AbstractBlock * t,AbstractBlock * b) {
    t->copyTo(b);
}
///set this block to air
void SCL_EXPORT clear(AbstractBlock * t) {
    t->clear();
}
///replacement for operator delete
void SCL_EXPORT destroy(AbstractBlock * t) {
    t->destroy();
}


}   //  extern "C"

#endif  //  ifdef SLOPECRAFTL_CAPI
