#ifndef COLORSET_H
#define COLORSET_H
#include <Eigen/Dense>
#include <QDebug>
#include "TokiColor.h"
using namespace Eigen;
class ColorSet{
    public:
        ArrayXXf _RGB;
        ArrayXXf  HSV;
        ArrayXXf  Lab;
        ArrayXXf  XYZ;
        VectorXi  Map;
        ColorSet();
        void ApplyAllowed(ColorSet*standard,bool *MIndex);
};

void GetMap(unsigned char *Map);

void GetMap(VectorXi &Map);
#endif // COLORSET_H
