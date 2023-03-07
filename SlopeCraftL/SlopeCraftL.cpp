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
#include "TokiSlopeCraft.h"
#include "simpleBlock.h"

using namespace SlopeCraft;

AbstractBlock *AbstractBlock::create() { return new simpleBlock; }

AbstractBlock::AbstractBlock() {}

void AbstractBlock::copyTo(AbstractBlock *dst) const {
  dst->setBurnable(getBurnable());
  dst->setDoGlow(getDoGlow());
  dst->setEndermanPickable(getEndermanPickable());
  dst->setId(getId());
  dst->setIdOld(getIdOld());
  dst->setNeedGlass(getNeedGlass());
  dst->setVersion(getVersion());
  // dst->setWallUseable(getWallUseable());
}

void AbstractBlock::clear() {
  setBurnable(false);
  setDoGlow(false);
  setEndermanPickable(false);
  setId("minecraft:air");
  setIdOld("");
  setNeedGlass(false);
  setVersion(0);
  // setWallUseable(false);
}

Kernel::Kernel() {}

Kernel *Kernel::create() {
  // return (new TokiSlopeCraft)->toBaseClassPtr();
  return static_cast<Kernel *>(new TokiSlopeCraft);
}

using namespace SlopeCraft;

extern "C" {

SCL_EXPORT Kernel *SCL_createKernel() { return new TokiSlopeCraft; }
SCL_EXPORT void SCL_destroyKernel(Kernel *k) {
  delete static_cast<TokiSlopeCraft *>(k);
}

SCL_EXPORT AbstractBlock *SCL_createBlock() { return new simpleBlock; }
SCL_EXPORT void SCL_destroyBlock(AbstractBlock *b) {
  delete static_cast<simpleBlock *>(b);
}

SCL_EXPORT AiCvterOpt *SCL_createAiCvterOpt() { return new AiCvterOpt; }
void SCL_EXPORT SCL_destroyAiCvterOpt(AiCvterOpt *a) { delete a; }

void SCL_EXPORT SCL_setPopSize(AiCvterOpt *a, unsigned int p) {
  a->popSize = p;
}
void SCL_EXPORT SCL_setMaxGeneration(AiCvterOpt *a, unsigned int p) {
  a->maxGeneration = p;
}
void SCL_EXPORT SCL_setMaxFailTimes(AiCvterOpt *a, unsigned int p) {
  a->maxFailTimes = p;
}
void SCL_EXPORT SCL_setCrossoverProb(AiCvterOpt *a, double p) {
  a->crossoverProb = p;
}
void SCL_EXPORT SCL_setMutationProb(AiCvterOpt *a, double p) {
  a->mutationProb = p;
}

unsigned int SCL_EXPORT SCL_getPopSize(const AiCvterOpt *a) {
  return a->popSize;
}
unsigned int SCL_EXPORT SCL_getMaxGeneration(const AiCvterOpt *a) {
  return a->maxGeneration;
}
unsigned int SCL_EXPORT SCL_getMaxFailTimes(const AiCvterOpt *a) {
  return a->maxFailTimes;
}
double SCL_EXPORT SCL_getCrossoverProb(const AiCvterOpt *a) {
  return a->crossoverProb;
}
double SCL_EXPORT SCL_getMutationProb(const AiCvterOpt *a) {
  return a->mutationProb;
}

SCL_EXPORT void SCL_getColorMapPtrs(const float **const rdata,
                                    const float **const gdata,
                                    const float **const bdata,
                                    const uint8_t **mapdata, int *num) {
  TokiSlopeCraft::getColorMapPtrs(rdata, gdata, bdata, mapdata, num);
}

SCL_EXPORT const float *SCL_getBasicColorMapPtrs() {
  return TokiSlopeCraft::getBasicColorMapPtrs();
}

SCL_EXPORT const char *SCL_getSCLVersion() {
  return TokiSlopeCraft::getSCLVersion();
}
}
