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

#include "newTokiColor.hpp"

#include "colorset_maptical.hpp"
/*
std::array<uint8_t, 4> newTokiColorBase<true>::DepthCount = {64, 64, 64, 64};
bool newTokiColorBase<true>::needFindSide = false;
*/

std::array<uint8_t, 4> newtokicolor_base_maptical::DepthCount = {64, 64, 64,
                                                                 64};
bool newtokicolor_base_maptical::needFindSide = false;
/*
SCL_convertAlgo newTokiColorBase<true>::convertAlgo =
    SCL_convertAlgo::RGB_Better;
    */
