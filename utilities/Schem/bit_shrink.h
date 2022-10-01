/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SCHEM_BITSHRINK_H
#define SCHEM_BITSHRINK_H

#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>

/**
 * \brief Bitwise compression for litematica
 *
 * \param src The 3D array in y-z-x order
 * \param src_count Number of elements
 * \param block_types Number of block types, used to compute how many bits to
 * store a element
 * \param dest destination
 */
void shrink_bits(const uint16_t *const src, const size_t src_count,
                 const int block_types,
                 std::vector<uint64_t> *const dest) noexcept;

inline auto to_pure_block_id(std::string_view id) noexcept {
  const size_t first_of_left_branket = id.find_first_of('[');
  if (first_of_left_branket == id.npos) {
    return id.substr();
  } else {
    return id.substr(0, first_of_left_branket);
  }
}

bool process_block_id(
    const std::string_view id, std::string *const pure_id,
    std::vector<std::pair<std::string, std::string>> *const traits);

void shrink_bytes_weSchem(const uint16_t *src, const size_t src_count,
                          const int palette_max,
                          std::vector<uint8_t> *const dest) noexcept;

class __mushroom_sides {

private:
  uint8_t val{0b111111};

public:
  __mushroom_sides() = default;
  __mushroom_sides(uint8_t _) : val(_) {}

  enum class side_t : uint8_t { up, down, north, south, east, west };

  inline uint8_t u6() const noexcept { return val & 0b111111; }

  inline void set_u6(uint8_t v) noexcept { val = v & 0b111111; }

  inline void set_full() noexcept { val = 0b111111; }

  template <side_t side> struct proxy_ref {
  private:
    uint8_t *const data;

  public:
    proxy_ref(uint8_t *__data) : data(__data) {}
    proxy_ref(__mushroom_sides *thisptr) : data(&thisptr->val) {}
    static constexpr uint8_t mask = 1U << uint8_t(side);
    static constexpr uint8_t invmask = ~mask;
    inline operator bool() const noexcept { return *data & mask; }

    inline bool operator=(const bool val) noexcept {
      if (val) {
        *data |= mask;
      } else {
        *data &= invmask;
      }
      return val;
    }
  };

  inline auto operator++(int) noexcept {
    this->val++;
    return *this;
  }

  inline auto up() noexcept { return proxy_ref<side_t::up>(this); }
  inline bool up() const noexcept {
    return proxy_ref<side_t::up>::mask & this->val;
  }
  inline auto down() noexcept { return proxy_ref<side_t::down>(this); }
  inline bool down() const noexcept {
    return proxy_ref<side_t::down>::mask & this->val;
  }
  inline auto north() noexcept { return proxy_ref<side_t::north>(this); }
  inline bool north() const noexcept {
    return proxy_ref<side_t::north>::mask & this->val;
  }
  inline auto south() noexcept { return proxy_ref<side_t::south>(this); }
  inline bool south() const noexcept {
    return proxy_ref<side_t::south>::mask & this->val;
  }
  inline auto east() noexcept { return proxy_ref<side_t::east>(this); }
  inline bool east() const noexcept {
    return proxy_ref<side_t::east>::mask & this->val;
  }
  inline auto west() noexcept { return proxy_ref<side_t::west>(this); }
  inline bool west() const noexcept {
    return proxy_ref<side_t::west>::mask & this->val;
  }

  std::string to_blockid(std::string_view pure_id) const noexcept {
    std::string val;
    val.reserve(512);
    val = pure_id.data();
    val.push_back('[');

    for (uint8_t dir_v = 0; dir_v < 6; dir_v++) {
      const side_t dir = side_t(dir_v);
      const char *dir_value = nullptr;
      switch (dir) {
      case side_t::up:
        val += "up=";
        dir_value = up() ? "true" : "false";
        break;
      case side_t::down:
        val += "down=";
        dir_value = down() ? "true" : "false";
        break;
      case side_t::north:
        val += "north=";
        dir_value = north() ? "true" : "false";
        break;
      case side_t::south:
        val += "south=";
        dir_value = south() ? "true" : "false";
        break;
      case side_t::east:
        val += "east=";
        dir_value = east() ? "true" : "false";
        break;
      case side_t::west:
        val += "west=";
        dir_value = west() ? "true" : "false";
        break;
      }
      val += dir_value;
      val.push_back(',');
    }
    val.back() = ']';
    return val;
  }

  static __mushroom_sides from_block_id(std::string_view blockid) noexcept {
    std::string pure_id;
    pure_id.resize(128);
    std::vector<std::pair<std::string, std::string>> traits;
    traits.resize(6);

    __mushroom_sides result;

    if (!process_block_id(blockid, &pure_id, &traits)) {
      exit(114);
      return __mushroom_sides();
    }

    for (const auto &t : traits) {
      if (t.first == "up") {
        result.up() = !(t.second == "false");
        continue;
      }
      if (t.first == "down") {
        result.down() = !(t.second == "false");
        continue;
      }
      if (t.first == "north") {
        result.north() = !(t.second == "false");
        continue;
      }
      if (t.first == "west") {
        result.west() = !(t.second == "false");
        continue;
      }
      if (t.first == "south") {
        result.south() = !(t.second == "false");
        continue;
      }
      if (t.first == "east") {
        result.east() = !(t.second == "false");
        continue;
      }
    }

    return result;
  }
};

#endif // SCHEM_BITSHRINK_H