#include "Kernel.h"
#include "simpleBlock.h"
#include "TokiSlopeCraft.h"

using namespace SlopeCraft;

AbstractBlock::AbstractBlock() {

}

AbstractBlock * AbstractBlock::createSimpleBlock() {
    return new simpleBlock;
}

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

#ifdef WITH_QT
Kernel::Kernel(QObject *parent) : QObject(parent)
#else
Kernel::Kernel()
#endif
{

}

#ifdef WITH_QT
Kernel * createKernel(QObject * parent) {
    return new TokiSlopeCraft(parent);
};
#else
Kernel * Kernel::createKernel() {
    return new TokiSlopeCraft;
};
#endif
