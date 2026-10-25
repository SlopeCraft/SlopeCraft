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

#include "item.h"

using namespace SlopeCraft;

nbt::tag_compound item::dump(
    MCDataVersion::MCDataVersion_t data_version) const {
  auto basic_fields = this->dump_basic_fields(data_version);
#warning "In some version it is "tag". At least 1.19.4"
  const std::string key =
      data_version < MCDataVersion::MCDataVersion_t::Java_1_20_5 ? "tag"
                                                                 : "components";
  nbt::tag_compound ret{std::move(basic_fields)};
  {
    auto item_tags = this->dump_tags(data_version);
    if (item_tags) {
      ret.insert(key, std::move(item_tags.value()));
    }
  }
  return ret;
}

nbt::tag_compound item::dump_basic_fields(
    MCDataVersion::MCDataVersion_t data_version) const {
  nbt::tag_compound ret;
  ret.insert("id", nbt::tag_string{std::string{this->id()}});
  if (data_version < MCDataVersion::MCDataVersion_t::Java_1_20_5) {
    ret.insert("Count", nbt::tag_byte{this->count_});
  } else {
    ret.insert("count", nbt::tag_int{this->count_});
  }

  return ret;
}

std::optional<nbt::tag_compound> item::dump_tags(
    MCDataVersion::MCDataVersion_t) const {
  return std::nullopt;
}

std::optional<nbt::tag_compound> filled_map::dump_tags(
    MCDataVersion::MCDataVersion_t data_version) const {
  nbt::tag_compound ret =
      item::dump_tags(data_version).value_or(nbt::tag_compound{});

  if (data_version < MCDataVersion::MCDataVersion_t::Java_1_20_5) {
    ret.insert("map", nbt::tag_int{this->map_id});
  } else {
    ret.insert("minecraft:map_id", this->map_id);
  }
  return ret;
}