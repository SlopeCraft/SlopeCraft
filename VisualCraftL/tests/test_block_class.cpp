/*
 Copyright © 2021-2026  TokiNoBug
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

#include "VisualCraftL.h"

#include <iostream>

int main(int, char **) {
  const char *arr[] = {"wood",
                       "planks",
                       "leaves",
                       "mushroom",
                       "slab",
                       "wool",
                       "concrete",
                       "terracotta",
                       "glazed_terracotta",
                       "concrete_powder",
                       "shulker_box",
                       "glass",
                       "redstone",
                       "stone",
                       "ore",
                       "clay",
                       "natural",
                       "crafted",
                       "desert",
                       "nether",
                       "the_end",
                       "ocean",
                       "creative_only",
                       "others"};

  static_assert(sizeof(arr) / sizeof(const char *) ==
                (int)VCL_block_class_t::others + 1);

  bool ok = true;
  for (int i = 0; i <= (int)VCL_block_class_t::others; i++) {
    const VCL_block_class_t ret = VCL_string_to_block_class(arr[i], &ok);

    if (!ok || int(ret) != i) {
      std::cout << "Failed at " << arr[i] << " : ok = " << ok
                << ", ret = " << int(i) << std::endl;
      return 1;
    }
  }

  return 0;
}