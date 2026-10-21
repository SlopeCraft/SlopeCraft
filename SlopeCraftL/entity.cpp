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

#include <algorithm>
#include <cmath>
#include <cassert>
#include <sstream>
#include <span>

#include <io/stream_writer.h>

#include "entity.h"

using namespace SlopeCraft;
using nbt::tag_compound, nbt::tag_string, nbt::tag_float, nbt::tag_double,
    nbt::tag_list, nbt::tag_byte, nbt::tag_int;

tag_compound entity::dump(MCDataVersion::MCDataVersion_t data_version
                          [[maybe_unused]]) const {
  tag_compound ret;

  if (id() not_eq "player") {
    ret.insert("id", tag_string{std::string{id()}});
  }
  {
    const auto [x, y, z] = this->position();
    ret.insert("Pos", tag_list::of<tag_double>({x, y, z}));
  }
  {
    const auto [h, v] = this->rotation();
    ret.insert("Rotation", tag_list::of<tag_float>({h, v}));
  }

  return ret;
}

tag_compound hangable::dump(MCDataVersion::MCDataVersion_t data_version
                            [[maybe_unused]]) const {
  auto ret = entity::dump(data_version);
  ret.insert("Facing", tag_byte{static_cast<int8_t>(this->direction_)});

  const std::array<const char*, 3> keys{"TileX", "TileY", "TileZ"};
  for (size_t dim = 0; dim < 3; ++dim) {
    ret.insert(keys[dim], tag_int{this->tile_position_[dim]});
  }
  return ret;
}

tag_compound item_frame::dump(
    MCDataVersion::MCDataVersion_t data_version) const {
  auto ret = hangable::dump(data_version);
  ret.insert("Fixed", tag_byte{this->fixed_});
  ret.insert("Invisible", tag_byte{this->invisible_});
  ret.insert("ItemDropChance", tag_float{this->item_drop_chance});

  assert(0 <= item_rotation and item_rotation <= 7);
  const auto rot = std::clamp<int8_t>(item_rotation, 0, 7);
  ret.insert("ItemRotation", tag_byte{rot});

  if (this->item_) {
    auto item_compound = this->item_->dump(data_version);
    ret.insert("Item", std::move(item_compound));
  }
  return ret;
}

std::string SlopeCraft::dump_compound_to_bytes(const tag_compound& compound) {
  std::ostringstream oss{std::ios::out | std::ios::binary};
  nbt::io::write_tag("", compound, oss);

  const auto s = oss.str();
  return s;
}