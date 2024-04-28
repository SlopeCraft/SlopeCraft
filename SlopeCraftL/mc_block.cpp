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

#include "mc_block.h"

mc_block::mc_block() {
  //  id = "";
  //  version = 0;
  //  idOld = "";
  //  needGlass = false;
  //  doGlow = false;
  //  endermanPickable = false;
  //  burnable = false;

  image.resize(16, 16);
  // wallUseable=true;
}

size_t block_list::get_blocks(mc_block_interface **dst, uint8_t *dst_basecolor,
                              size_t capacity_in_elements) noexcept {
  if (dst == nullptr || dst_basecolor == nullptr || capacity_in_elements <= 0) {
    return 0;
  }

  size_t counter = 0;
  for (auto &ptr : this->m_blocks) {
    dst[counter] = ptr.first.get();
    dst_basecolor[counter] = ptr.second;
    counter++;
    if (counter >= capacity_in_elements) {
      return counter;
    }
  }
  return counter;
}

size_t block_list::get_blocks(const mc_block_interface **dst,
                              uint8_t *dst_basecolor,
                              size_t capacity_in_elements) const noexcept {
  if (dst == nullptr || dst_basecolor == nullptr || capacity_in_elements <= 0) {
    return 0;
  }

  size_t counter = 0;
  for (auto &ptr : this->m_blocks) {
    dst[counter] = ptr.first.get();
    dst_basecolor[counter] = ptr.second;
    counter++;
    if (counter >= capacity_in_elements) {
      return counter;
    }
  }
  return counter;
}

void block_list::clear() noexcept { this->m_blocks.clear(); }

block_list::~block_list() { this->clear(); }