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
#ifndef SLOPECRAFT_ENTITY_H
#define SLOPECRAFT_ENTITY_H

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <string_view>

#include <nbt_tags.h>

#include "SC_GlobalEnums.h"
#include "item.h"

namespace SlopeCraft {

class entity {
 public:
  virtual ~entity() = default;
  [[nodiscard]] virtual std::string_view id() const noexcept = 0;
  /// xyz
  [[nodiscard]] virtual std::array<double, 3> position() const noexcept = 0;
  /// [horizontal, vertical]. north = [-180,0], east = [-90,0], south = [0,0],
  /// west = [90,0], north(again) = [180,0].
  [[nodiscard]] virtual std::array<float, 2> rotation() const noexcept = 0;

  [[nodiscard]] virtual nbt::tag_compound dump(
      MCDataVersion::MCDataVersion_t data_version) const;

  [[nodiscard]] virtual std::unique_ptr<entity> clone() const noexcept = 0;
};

enum class hangable_facing_direction : int8_t {
  south = 3,
  west = 4,
  north = 2,
  east = 5,
  top = 1,
  bottom = 0,
};

class hangable : public entity {
 public:
  hangable_facing_direction direction_{hangable_facing_direction::north};
  std::array<int, 3> tile_position_{0, 0, 0};

  [[nodiscard]] std::array<double, 3> position() const noexcept override {
    std::array<double, 3> ret;
    for (size_t i = 0; i < 3; i++) {
      ret[i] = this->tile_position_[i];
    }
    return ret;
  }

  [[nodiscard]] std::array<float, 2> rotation() const noexcept override {
    using hfd = hangable_facing_direction;
    switch (this->direction_) {
      case hfd::north:
        return {-180, 0};
      case hfd::east:
        return {-90, 0};
      case hfd::south:
        return {0, 0};
      case hfd::west:
        return {90, 0};
      case hfd::top:
        return {0, 90};
      case hfd::bottom:
        return {0, -90};
    }
    return {0, 0};
  }

  [[nodiscard]] nbt::tag_compound dump(
      MCDataVersion::MCDataVersion_t data_version) const override;
};

enum class item_frame_variant : uint8_t {
  common,
  glowing,
};

class item_frame : public hangable {
 public:
  /// true to prevent it from dropping if it has no support block, being moved
  /// (e.g. by pistons), taking damage (except from creative players), and
  /// placing an item in it, removing its item, or rotating it.
  bool fixed_{false};
  /// Whether the item frame is invisible. The contained item or map remains
  /// visible.
  bool invisible_{false};

  int8_t item_rotation{0};
  float item_drop_chance{1};
  item_frame_variant variant_{item_frame_variant::common};

  std::unique_ptr<item> item_{nullptr};

  item_frame() = default;

  item_frame(const item_frame& src)
      : hangable{src},
        fixed_{src.fixed_},
        invisible_{src.invisible_},
        item_rotation{src.item_rotation},
        item_drop_chance{src.item_drop_chance},
        variant_{src.variant_},
        item_{src.item_ ? src.item_->clone() : nullptr} {}

  [[nodiscard]] std::string_view id() const noexcept override {
    switch (this->variant_) {
      case item_frame_variant::common:
        return "item_frame";
      case item_frame_variant::glowing:
        return "glow_item_frame";
    }
    return {};
  }

  [[nodiscard]] std::unique_ptr<entity> clone() const noexcept override {
    return std::make_unique<item_frame>(*this);
  }

  [[nodiscard]] nbt::tag_compound dump(
      MCDataVersion::MCDataVersion_t data_version) const override;
};

// not compressed
std::string dump_compound_to_bytes(const nbt::tag_compound& compound);
}  // namespace SlopeCraft

#endif  // SLOPECRAFT_ENTITY_H
