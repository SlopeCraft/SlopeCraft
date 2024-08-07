//
// Created by Joseph on 2024/8/6.
//

#include <NBTWriter/NBTWriter.h>
#include <fmt/format.h>

#include "entity.h"

tl::expected<size_t, std::string> libSchem::entity::dump(
    NBT::NBTWriter<true> &destination,
    MCDataVersion::MCDataVersion_t data_version) const noexcept {
  if (not destination.isInCompound()) {
    return tl::make_unexpected(
        "You should write entities fields into a compound, but the NBTWriter "
        "is not writing a compound.");
  }
  size_t bytes = 0;

  if (this->id() not_eq "player") {
    bytes += destination.writeString("id", this->id().data());
  }
  {
    destination.writeListHead("Pos", NBT::tagType::Double, 3);
    for (double cord : this->position()) {
      bytes += destination.writeDouble("", cord);
    }
    assert(destination.isInCompound());
  }
  {
    bytes += destination.writeListHead("Rotation", NBT::tagType::Float, 2);
    for (float rot : this->rotation()) {
      bytes += destination.writeFloat("", rot);
    }
    assert(destination.isInCompound());
  }

  return bytes;
}

tl::expected<size_t, std::string> libSchem::hangable::dump(
    NBT::NBTWriter<true> &destination,
    MCDataVersion::MCDataVersion_t data_version) const noexcept {
  auto res = entity::dump(destination, data_version);
  if (not res) {
    return res;
  }
  size_t bytes = res.value();
  bytes +=
      destination.writeByte("Facing", static_cast<int8_t>(this->direction_));

  const std::array<const char *, 3> keys{"TileX", "TileY", "TileZ"};
  for (size_t dim = 0; dim < 3; dim++) {
    bytes += destination.writeInt(keys[dim], this->tile_position_[dim]);
  }
  return bytes;
}

tl::expected<size_t, std::string> libSchem::item_frame::dump(
    NBT::NBTWriter<true> &destination,
    MCDataVersion::MCDataVersion_t data_version) const noexcept {
  auto res = hangable::dump(destination, data_version);
  if (not res) {
    return res;
  }
  size_t bytes = res.value();
  bytes += destination.writeByte("Fixed", this->fixed_);
  bytes += destination.writeByte("Invisible", this->invisible_);
  bytes += destination.writeFloat("ItemDropChance", this->item_drop_chance);
  if (this->item_rotation < 0 or this->item_rotation > 7) {
    return tl::make_unexpected(
        fmt::format("Invalid item rotation {}, expected in range [0,7]",
                    int(this->item_rotation)));
  }
  bytes += destination.writeByte("ItemRotation", this->item_rotation);

  if (this->item_ not_eq nullptr) {
    bytes += destination.writeCompound("Item");
    auto res_item = this->item_->dump(destination, data_version);
    if (not res_item) {
      return tl::make_unexpected(
          fmt::format("Failed to dump item fields: {}", res_item.error()));
    }
    bytes += res_item.value();
    bytes += destination.endCompound();
  }
  return bytes;
}