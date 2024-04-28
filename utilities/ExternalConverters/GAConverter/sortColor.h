/*
 Copyright © 2021-2023  TokiNoBug
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
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SORTCOLOR_H
#define SORTCOLOR_H

#include "GACvterDefines.hpp"
#include "../../SlopeCraftL/SCLDefines.h"

namespace GACvter {

class sortColor {
 public:
  sortColor() noexcept;

  void calculate(ARGB, const colorset_allowed_t& allowed) noexcept;

  inline mapColor_t mapColor(order_t o) const noexcept { return mapCs[o]; }
  /*
  inline float error(order_t o) const {
      return errors[o];
  }
  */

 private:
  std::array<mapColor_t, OrderMax> mapCs;
  // std::array<float,OrderMax> errors;
};

}  // namespace GACvter
#endif  // SORTCOLOR_H
