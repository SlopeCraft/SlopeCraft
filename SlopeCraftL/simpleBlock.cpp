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

#include "simpleBlock.h"

simpleBlock::simpleBlock() {
  id = "";
  version = 0;
  idOld = "";
  needGlass = false;
  doGlow = false;
  endermanPickable = false;
  burnable = false;

  image.resize(16, 16);
  // wallUseable=true;
}

bool simpleBlock::dealBlockId(const std::string &id, std::string &netBlockId,
                              stringList *proName, stringList *proVal) {
  proName->clear();
  proVal->clear();
  if (id.back() != ']') {
    netBlockId = id;
    return false;
  }
  short ReadBeg = id.find('[');
  // BlockId.indexOf('[');
  short ReadEnd = id.find(']');
  // BlockId.indexOf(']');

  if (ReadBeg <= 0 || ReadEnd <= 0 || ReadEnd <= ReadBeg) {
    std::cerr << "方块 Id 格式出现错误：" << id;
    return false;
  }

  short ProIndex[2] = {-1, -1}, ProValIndex[2] = {-1, -1};

  netBlockId = id.substr(0, ReadBeg);
  // BlockId.mid(0,ReadBeg).toLower();

  for (short read = ReadBeg; read <= ReadEnd; read++) {
    switch (id.at(read)) {
      case '[':  // 代表找到了一个新的属性
        ProIndex[0] = read + 1;
        continue;

      case '=':  // 识别出了属性名，寻找属性值
        ProIndex[1] = read - 1;
        ProValIndex[0] = read + 1;
        continue;

      case ',':  // 代表结束了一个属性，并找到了下一个属性
        ProValIndex[1] = read - 1;
        proName->push_back(
            id.substr(ProIndex[0], ProIndex[1] - ProIndex[0] + 1));
        // BlockId.mid(ProIndex[0],ProIndex[1]-ProIndex[0]+1).toLower());
        proVal->push_back(
            id.substr(ProValIndex[0], ProValIndex[1] - ProValIndex[0] + 1));
        // BlockId.mid(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1).toLower());
        ProIndex[0] = -1;
        ProIndex[1] = -1;
        ProValIndex[0] = -1;
        ProValIndex[1] = -1;

        ProIndex[0] = read + 1;
        continue;
      case ']':
        ProValIndex[1] = read - 1;
        proName->push_back(
            id.substr(ProIndex[0], ProIndex[1] - ProIndex[0] + 1));
        // BlockId.mid(ProIndex[0],ProIndex[1]-ProIndex[0]+1).toLower());
        proVal->push_back(
            id.substr(ProValIndex[0], ProValIndex[1] - ProValIndex[0] + 1));
        // BlockId.mid(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1).toLower());
        continue;
    }
  }
  // qDebug()<<proValue->back()<<'='<<proValue->back();
  return true;
}

size_t BlockList::get_blocks(AbstractBlock **dst, uint8_t *dst_basecolor,
                             size_t capacity_in_elements) noexcept {
  if (dst == nullptr || dst_basecolor == nullptr || capacity_in_elements <= 0) {
    return 0;
  }

  size_t counter = 0;
  for (auto &ptr : this->m_blocks) {
    dst[counter] = ptr.first;
    dst_basecolor[counter] = ptr.second;
    counter++;
    if (counter >= capacity_in_elements) {
      return counter;
    }
  }
  return counter;
}

size_t BlockList::get_blocks(const AbstractBlock **dst, uint8_t *dst_basecolor,
                             size_t capacity_in_elements) const noexcept {
  if (dst == nullptr || dst_basecolor == nullptr || capacity_in_elements <= 0) {
    return 0;
  }

  size_t counter = 0;
  for (auto &ptr : this->m_blocks) {
    dst[counter] = ptr.first;
    dst_basecolor[counter] = ptr.second;
    counter++;
    if (counter >= capacity_in_elements) {
      return counter;
    }
  }
  return counter;
}

void BlockList::clear() noexcept {
  for (auto &ptr : this->m_blocks) {
    delete ptr.first;
  }

  this->m_blocks.clear();
}

BlockList::~BlockList() { this->clear(); }