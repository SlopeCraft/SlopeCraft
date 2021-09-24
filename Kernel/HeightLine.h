#ifndef HEIGHTLINE_H
#define HEIGHTLINE_H

#include <iostream>
#include <vector>
#include <map>
#include <Eigen/Dense>
#include "defines.h"
#include "ColorSet.h"
#include "WaterItem.h"
#include "OptiChain.h"

class HeightLine
{
public:
    HeightLine();
    float make(const TokiColor*[], const Eigen::Array<uchar,Eigen::Dynamic,1>&
               ,bool allowNaturalCompress, Eigen::ArrayXi* dst=nullptr);
    void make(const Eigen::ArrayXi & mapColorCol,bool allowNaturalCompress);
    void updateWaterMap();
    ushort maxHeight() const;
    const Eigen::ArrayXi & getHighLine() const;
    const Eigen::ArrayXi & getLowLine() const;
    const Eigen::ArrayXi & getBase() const;
    const std::map<ushort,waterItem> & getWaterMap() const;
    EImage toImg() const;

    static const ARGB BlockColor;
    static const ARGB AirColor;
    static const ARGB WaterColor;
    static const ARGB greyColor;

private:
    Eigen::ArrayXi base;
    Eigen::ArrayXi HighLine;
    Eigen::ArrayXi LowLine;
    std::map<ushort,waterItem> waterMap;
};

#endif // HEIGHTLINE_H
