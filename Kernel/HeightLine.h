#ifndef HEIGHTLINE_H
#define HEIGHTLINE_H

#include <iostream>
#include <vector>
#include <map>
#include <Eigen/Dense>
#include "ColorSet.h"
#include "WaterItem.h"
#include "OptiChain.h"
using namespace Eigen;

typedef unsigned short ushort;


class HeightLine
{
public:
    HeightLine();
    float make(const TokiColor*[], const Array<uchar,Dynamic,1>&);
    void make(const ArrayXi  mapColorCol,bool allowNaturalCompress=false);
    void updateWaterMap();
    ushort maxHeight() const;
    const ArrayXi & getHighLine() const;
    const ArrayXi & getLowLine() const;
    const ArrayXi & getBase() const;
    const std::map<ushort,waterItem> & getWaterMap() const;
private:
    ArrayXi base;
    ArrayXi HighLine;
    ArrayXi LowLine;
    std::map<ushort,waterItem> waterMap;
};

#endif // HEIGHTLINE_H
