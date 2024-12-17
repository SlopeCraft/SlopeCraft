//
// Created by Joseph on 2024/8/6.
//

#ifndef SLOPECRAFT_ITEM_H
#define SLOPECRAFT_ITEM_H

#include <cstdint>
#include <cstddef>
#include <string_view>
#include <string>
#include <memory>

#include <tl/expected.hpp>
#include <MCDataVersion.h>

namespace NBT {
template <bool is_nbt_compressed>
class NBTWriter;
}

namespace libSchem {
class item {
 public:
  virtual ~item() = default;

  virtual std::string_view id() const noexcept = 0;

  int8_t count_{1};
  virtual tl::expected<size_t, std::string> dump(
      NBT::NBTWriter<true>&,
      MCDataVersion::MCDataVersion_t data_version) const noexcept;

  [[nodiscard]] virtual std::unique_ptr<item> clone() const noexcept = 0;

 protected:
  virtual tl::expected<size_t, std::string> dump_basic_fields(
      NBT::NBTWriter<true>&,
      MCDataVersion::MCDataVersion_t data_version) const noexcept;
  virtual tl::expected<size_t, std::string> dump_tags(
      NBT::NBTWriter<true>&,
      MCDataVersion::MCDataVersion_t data_version) const noexcept;
};

class filled_map : public item {
 public:
  /// The map number
  int map_id{0};

  std::string_view id() const noexcept override {
    return "minecraft:filled_map";
  }
  std::unique_ptr<item> clone() const noexcept override {
    return std::make_unique<filled_map>(*this);
  }

 protected:
  tl::expected<size_t, std::string> dump_tags(
      NBT::NBTWriter<true>&,
      MCDataVersion::MCDataVersion_t data_version) const noexcept override;
};
}  // namespace libSchem

#endif  // SLOPECRAFT_ITEM_H
