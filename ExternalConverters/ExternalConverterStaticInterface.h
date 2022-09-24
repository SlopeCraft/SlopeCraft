#ifndef EXTERNAL_CONVERTER_STATIC_INTERFACE_H
#define EXTERNAL_CONVERTER_STATIC_INTERFACE_H

#include <Eigen/Dense>

#include "../ColorManip/ColorManip.h"

using EImage = Eigen::ArrayXX<uint32_t>;

using MapList = Eigen::Array<uint8_t, Eigen::Dynamic, 1, Eigen::ColMajor, 256>;
using ColorList = Eigen::Array<float, Eigen::Dynamic, 3, Eigen::ColMajor, 256>;
using TempVectorXf =
    Eigen::Array<float, Eigen::Dynamic, 1, Eigen::ColMajor, 256>;

namespace SlopeCraft {

extern int colorCount4External();

extern Eigen::Map<const Eigen::ArrayXf> BasicRGB4External(int channel);

extern Eigen::Map<const Eigen::ArrayXf> AllowedRGB4External(int channel);

extern Eigen::Map<const Eigen::Array<uint8_t, Eigen::Dynamic, 1>>
AllowedMapList4External();

} // namespace SlopeCraft

#endif // EXTERNAL_CONVERTER_STATIC_INTERFACE_H