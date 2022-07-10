#include "sortColor.h"

using namespace GACvter;
using namespace SlopeCraft;

sortColor::sortColor() noexcept {

}

void sortColor::calculate(ARGB rgb) noexcept {
    const float r=getR(rgb)/255.0f,
            g=getG(rgb)/255.0f,b=getB(rgb)/255.0f;

    auto diffR=SlopeCraft::Allowed4External.col(0)-r;
            //SlopeCraft::Allowed4External.col(0)-r;
    auto diffG=SlopeCraft::Allowed4External.col(1)-g;
    auto diffB=SlopeCraft::Allowed4External.col(2)-b;

    TempVectorXf diff=diffR.square()+diffG.square()+diffB.square();

    int tempIdx=0;
    for(order_t o=0;o<OrderMax;o++) {
        //errors[o]=
        diff.minCoeff(&tempIdx);
        mapCs[o]=SlopeCraft::AllowedMapList4External[tempIdx];
                //Converter::mapColorSrc->operator[](tempIdx);
        diff[tempIdx]=heu::internal::pinfF;
    }
}
