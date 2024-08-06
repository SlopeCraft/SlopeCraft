//
// Created by Joseph on 2024/8/6.
//

#include "item.h"
#include <NBTWriter/NBTWriter.h>

tl::expected<size_t, std::string> libSchem::item::dump(
    NBT::NBTWriter<true> &dest,
    MCDataVersion::MCDataVersion_t data_version) const noexcept {
  auto res = this->dump_basic_fields(dest, data_version);
  if (not res) {
    return res;
  }
  size_t bytes = res.value();
  const char *key = nullptr;
  if (data_version < MCDataVersion::MCDataVersion_t::Java_1_20_5) {
    key = "tags";
  } else {
    key = "components";
  }
  {
    bytes += dest.writeCompound(key);
    auto res_tags = this->dump_tags(dest, data_version);
    if (not res_tags) {
      dest.endCompound();
      return res_tags;
    }
    bytes += res_tags.value();
    bytes += dest.endCompound();
  }
  return bytes;
}

tl::expected<size_t, std::string> libSchem::item::dump_basic_fields(
    NBT::NBTWriter<true> &dest,
    MCDataVersion::MCDataVersion_t data_version) const noexcept {
  if (not dest.isInCompound()) {
    return tl::make_unexpected(
        "You should dump item fields into a compound, but the NBT write is not "
        "writing a compound.");
  }

  size_t bytes = 0;
  bytes += dest.writeString("id", this->id().data());
  if (data_version < MCDataVersion::MCDataVersion_t::Java_1_20_5) {
    bytes += dest.writeByte("Count", this->count_);
  } else {
    bytes += dest.writeInt("count", this->count_);
  }
  return bytes;
}

tl::expected<size_t, std::string> libSchem::item::dump_tags(
    NBT::NBTWriter<true> &dest,
    MCDataVersion::MCDataVersion_t data_version) const noexcept {
  if (not dest.isInCompound()) {
    return tl::make_unexpected("You should dump item tags into a compound.");
  }
  return 0;
}

tl::expected<size_t, std::string> libSchem::filled_map::dump_tags(
    NBT::NBTWriter<true> &dest,
    MCDataVersion::MCDataVersion_t data_version) const noexcept {
  auto res = item::dump_tags(dest, data_version);
  if (not res) {
    return res;
  }
  size_t bytes = res.value();
  if (data_version < MCDataVersion::MCDataVersion_t::Java_1_20_5) {
    bytes += dest.writeInt("map", this->map_id);
  } else {
    bytes += dest.writeInt("minecraft:map_id", this->map_id);
  }
  return bytes;
}