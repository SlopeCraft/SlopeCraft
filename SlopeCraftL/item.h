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

#ifndef SLOPECRAFT_ITEM_H
#define SLOPECRAFT_ITEM_H

#include <cstdint>
#include <string>
#include <optional>
#include <memory>
#include <string_view>

#include <nbt_tags.h>

#include "SC_GlobalEnums.h"

namespace SlopeCraft {

class item {
 public:
  int8_t count_{1};

  item() = default;
  virtual ~item() = default;

  [[nodiscard]] virtual std::string_view id() const noexcept = 0;

  [[nodiscard]] virtual std::unique_ptr<item> clone() const noexcept = 0;

  [[nodiscard]] virtual nbt::tag_compound dump(
      MCDataVersion::MCDataVersion_t data_version) const;

 protected:
  virtual nbt::tag_compound dump_basic_fields(
      MCDataVersion::MCDataVersion_t data_version) const;
  /// Extra nbt fields.
  virtual std::optional<nbt::tag_compound> dump_tags(
      MCDataVersion::MCDataVersion_t data_version) const;
};

class filled_map : public item {
 public:
  /// The map number
  int map_id{0};

  [[nodiscard]] std::string_view id() const noexcept override {
    return "minecraft:filled_map";
  }

  [[nodiscard]] std::unique_ptr<item> clone() const noexcept override {
    return std::make_unique<filled_map>(*this);
  }

 protected:
  std::optional<nbt::tag_compound> dump_tags(
      MCDataVersion::MCDataVersion_t data_version) const override;
};

}  // namespace SlopeCraft

#endif  // SLOPECRAFT_ITEM_H
