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

#ifndef COLORSET_H
#define COLORSET_H
#include "../ColorManip/newColorSet.hpp"
#include "SCLDefines.h"

// using namespace Eigen;
/*
class ConstColorSet {
public:
  explicit ConstColorSet() = default;
  explicit ConstColorSet(const float *rgbSrc);

  Eigen::Array<float, 256, 3> _RGB;
  Eigen::Array<float, 256, 3> HSV;
  Eigen::Array<float, 256, 3> Lab;
  Eigen::Array<float, 256, 3> XYZ;
  Eigen::Array<uint8_t, 256, 1> Map;
};

class ColorSet {
public:
  explicit ColorSet();
  explicit ColorSet(int ColorNum);
  ColorList _RGB;
  ColorList HSV;
  ColorList Lab;
  ColorList XYZ;
  MapList Map;
  bool ApplyAllowed(const ConstColorSet &standard, bool *MIndex);
  uint16_t colorCount() const;
};

*/

using colorset_allowed_t = colorset_new<false, true, 256>;
using colorset_basic_t = colorset_new<true, true, 256>;

namespace SlopeCraft {
extern int colorCount4External();
// extern const Eigen::Array<float, 256, 3> &BasicRGB4External;
extern Eigen::Map<const Eigen::ArrayXf> BasicRGB4External(int channel);

extern Eigen::Map<const Eigen::ArrayXf> AllowedRGB4External(int channel);
// extern const ColorList &Allowed4External;
// extern Eigen::Map<const Eigen::Array<float, Dynamic, 3>> Allowed4External();
// extern const MapList &AllowedMapList4External;
extern Eigen::Map<const Eigen::Array<uint8_t, Dynamic, 1>>
AllowedMapList4External();
} // namespace SlopeCraft

// bool readFromFile(const char *FileName, Eigen::ArrayXXf &M);
// bool readFromTokiColor(const char *, Eigen::ArrayXXf &, const std::string &);

#endif // COLORSET_H
