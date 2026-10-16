#ifndef SLOPECRAFT_UTILITIES_SCHEM_MUSHROOM_H
#define SLOPECRAFT_UTILITIES_SCHEM_MUSHROOM_H
#include <string>
#include <string_view>
#include <array>
#include <cassert>
#include <optional>
#include <functional>
#include <cstdint>

namespace libSchem::internal {
enum class mushroom_type : uint8_t { red, brown, stem };

enum class direction : uint8_t { up, down, north, south, east, west };

[[nodiscard]] bool is_mushroom(std::string_view blkid) noexcept;

[[nodiscard]] std::optional<mushroom_type> pureid_to_type(
    std::string_view pid) noexcept;

class mushroom_state {
 private:
  std::array<bool, 6> m_is_stoma{false, false, false, false, false, false};

 public:
  mushroom_state() = default;
  explicit mushroom_state(std::array<bool, 6> _stoma);

  [[nodiscard]] inline bool is_stoma(direction dir) const noexcept {
    assert(size_t(dir) < 6);
    return this->m_is_stoma[size_t(dir)];
  }

  inline void set_stoma(direction dir, bool set_to_stoma) noexcept {
    assert(size_t(dir) < 6);
    this->m_is_stoma[size_t(dir)] = set_to_stoma;
  }

  [[nodiscard]] std::string to_block_state_list() const noexcept;

  [[nodiscard]] static std::optional<mushroom_state> from_block_state_list(
      std::string_view bsl) noexcept;

  [[nodiscard]] uint8_t to_compressed_bits() const noexcept;

  [[nodiscard]] inline bool operator==(
      const mushroom_state another) const noexcept {
    return this->to_compressed_bits() == another.to_compressed_bits();
  }
};

struct mushroom_state_hash {
  inline uint64_t operator()(mushroom_state ms) const noexcept {
    return std::hash<uint8_t>()(ms.to_compressed_bits());
  }
};

class mushroom_block {
 private:
  // std::string m_pureid{""};
  mushroom_state m_state;
  mushroom_type m_type;

 public:
  [[nodiscard]] inline mushroom_state& state() noexcept {
    return this->m_state;
  }
  [[nodiscard]] inline mushroom_state state() const noexcept {
    return this->m_state;
  }
  inline void set_state(mushroom_state ms) noexcept { this->m_state = ms; }

  [[nodiscard]] std::string_view pureid() const noexcept;

  /*
inline void set_pureid(std::string_view pid) noexcept {
  this->m_pureid = pid;
}
*/

  [[nodiscard]] inline mushroom_type type() const noexcept {
    return this->m_type;
  }

  inline void set_type(mushroom_type mt) noexcept { this->m_type = mt; }

  [[nodiscard]] std::string id(bool with_namespacename = true) const noexcept;

  [[nodiscard]] static std::optional<mushroom_block> from_block_id(
      std::string_view blikd) noexcept;

  [[nodiscard]] inline bool operator==(
      const mushroom_block another) const noexcept {
    return (this->m_state == another.m_state) &&
           (this->m_type == another.m_type);
  }
};

struct mushroom_block_hash {
  [[nodiscard]] inline uint64_t operator()(mushroom_block mb) const noexcept {
    const uint16_t upper = uint8_t(mb.type());
    const uint16_t lower = mb.state().to_compressed_bits();

    return std::hash<uint16_t>()((upper << 8) | lower);
  }
};
}  // namespace libSchem::internal

#endif  // SLOPECRAFT_UTILITIES_SCHEM_MUSHROOM_H
