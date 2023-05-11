#include "mushroom.h"
#include <fmt/format.h>
#include <process_block_id.h>
#include <magic_enum.hpp>
#include <unordered_map>
namespace lsi = libSchem::internal;

#include "Schem.h"

std::optional<lsi::mushroom_type> lsi::pureid_to_type(
    std::string_view pid) noexcept {
  if (pid == "brown_mushroom_block") {
    return mushroom_type::brown;
  }
  if (pid == "red_mushroom_block") {
    return mushroom_type::red;
  }
  if (pid == "mushroom_stem") {
    return mushroom_type::stem;
  }
  return std::nullopt;
}

bool lsi::is_mushroom(std::string_view blkid) noexcept {
  blkid::char_range pure_name;
  if (!blkid::process_blk_id(blkid, nullptr, &pure_name, nullptr)) {
    // invalid block id
    return false;
  }

  return pureid_to_type({pure_name.begin(), pure_name.end()}).has_value();
}

lsi::mushroom_state::mushroom_state(std::array<bool, 6> _stoma)
    : m_is_stoma{_stoma} {}

std::string lsi::mushroom_state::to_block_state_list() const noexcept {
  return fmt::format("up={},down={},north={},south={},east={},west={}",
                     m_is_stoma[0], m_is_stoma[1], m_is_stoma[2], m_is_stoma[3],
                     m_is_stoma[4], m_is_stoma[5]);
}

uint8_t lsi::mushroom_state::to_compressed_bits() const noexcept {
  uint8_t ret{0};
  for (bool val : this->m_is_stoma) {
    ret |= uint8_t(val);
    ret = ret << 1;
  }

  return ret;
}

std::optional<lsi::mushroom_state> private_fun_bsl_to_mushroom_state(
    const std::vector<std::pair<blkid::char_range, blkid::char_range>>
        &bs) noexcept {
  using namespace lsi;
  std::array<bool, 6> stoma;
  stoma.fill(0);
  std::array<int, 6> set_times;
  set_times.fill(0);

  for (const auto &pair : bs) {
    std::string_view key_str{pair.first.begin(), pair.first.end()};
    auto key = magic_enum::enum_cast<direction>(key_str);
    if (!key.has_value()) {
      // invalid key
      return std::nullopt;
    }

    const auto dir = key.value();
    std::string_view value_str{pair.second.begin(), pair.second.end()};

    bool val;
    bool is_val_ok{false};
    if (value_str == "true") {
      val = true;
      is_val_ok = true;
    }
    if (value_str == "false") {
      val = false;
      is_val_ok = true;
    }

    if (!is_val_ok) {
      return std::nullopt;
    }

    stoma[size_t(dir)] = val;
    set_times[size_t(dir)]++;
  }

  for (auto times : set_times) {
    if (times > 1) {
      return std::nullopt;
    }
  }

  return mushroom_state{stoma};
}

std::optional<lsi::mushroom_state> lsi::mushroom_state::from_block_state_list(
    std::string_view bsl) noexcept {
  using namespace blkid;
  std::vector<std::pair<char_range, char_range>> bs;

  if (!blkid::process_state_list({&*bsl.begin(), &*bsl.end()}, &bs, nullptr)) {
    // invalid blockstatelist
    return std::nullopt;
  }

  return private_fun_bsl_to_mushroom_state(bs);
}

std::string_view lsi::mushroom_block::pureid() const noexcept {
  switch (this->m_type) {
    case mushroom_type::brown:
      return "brown_mushroom_block";
    case mushroom_type::red:
      return "red_mushroom_block";
    case mushroom_type::stem:
      return "mushroom_stem";
  }

  return "Invalid enum value";
}

std::string lsi::mushroom_block::id(bool with_namespacename) const noexcept {
  std::string_view nsn = (with_namespacename ? "minecraft::" : "");
  return fmt::format("{}{}[{}]", nsn, this->pureid(),
                     m_state.to_block_state_list());
}

std::optional<lsi::mushroom_block> lsi::mushroom_block::from_block_id(
    std::string_view blikd) noexcept {
  using namespace blkid;

  char_range pure_id;
  std::vector<std::pair<char_range, char_range>> bs;
  if (!process_blk_id(blikd, nullptr, &pure_id, &bs)) {
    return std::nullopt;
  }

  auto type = pureid_to_type({pure_id.begin(), pure_id.end()});
  if (!type.has_value()) {
    return std::nullopt;
  }

  auto state = private_fun_bsl_to_mushroom_state(bs);
  if (!state.has_value()) {
    return std::nullopt;
  }

  mushroom_block ret;
  ret.m_state = state.value();
  ret.m_type = type.value();
  return ret;
}

struct mushroom_map {
  using ele_t = libSchem::Schem::ele_t;
  std::unordered_map<libSchem::internal::mushroom_block, ele_t,
                     libSchem::internal::mushroom_block_hash>
      blk_to_ele;
  std::unordered_map<ele_t, libSchem::internal::mushroom_block> ele_to_blk;
  std::vector<std::string> *palette{nullptr};

  inline bool is_mushroom(ele_t e) const noexcept {
    return this->ele_to_blk.contains(e);
  }

  inline ele_t operator[](libSchem::internal::mushroom_block mb) noexcept {
    auto it = this->blk_to_ele.find(mb);
    if (it != this->blk_to_ele.end()) {
      return it->second;
    }
    const ele_t new_idx = this->palette->size();
    this->palette->emplace_back(mb.id(true));

    this->blk_to_ele.emplace(mb, new_idx);
    this->ele_to_blk.emplace(new_idx, mb);
    return new_idx;
  }

  inline libSchem::internal::mushroom_block at(ele_t idx) const noexcept {
    assert(this->is_mushroom(idx));
    return this->ele_to_blk.at(idx);
  }

  inline void emplace(ele_t ele,
                      libSchem::internal::mushroom_block blk) noexcept {
    this->blk_to_ele.emplace(blk, ele);
    this->ele_to_blk.emplace(ele, blk);
  }
};

void libSchem::Schem::process_mushroom_states_fast() noexcept {
  using libSchem::internal::direction;
  using libSchem::internal::mushroom_block;
  mushroom_map mmap;
  mmap.blk_to_ele.reserve(64 * 3);
  mmap.ele_to_blk.reserve(64 * 3);
  mmap.palette = &this->block_id_list;

  for (ele_t i = 0; i < this->block_id_list.size(); i++) {
    const auto &id = this->block_id_list[i];

    auto mb = mushroom_block::from_block_id(id);
    if (!mb.has_value()) {
      continue;
    }

    mmap.ele_to_blk.emplace(i, mb.value());
    mmap.blk_to_ele.emplace(mb.value(), i);
  }

  auto &self = *this;

  for (int64_t y = 0; y < this->y_range(); y++) {
    for (int64_t z = 0; z < this->z_range(); z++) {
      for (int64_t x = 0; x < this->x_range(); x++) {
        // skip if not a mushroom
        if (!mmap.is_mushroom(self(x, y, z))) {
          continue;
        }

        mushroom_block cur_blk = mmap.at(self(x, y, z));

        if ((x > 0) && mmap.is_mushroom(self(x - 1, y, z))) {
          cur_blk.state().set_stoma(direction::west, true);
        }
        if ((x < this->x_range() - 1) && mmap.is_mushroom(self(x + 1, y, z))) {
          cur_blk.state().set_stoma(direction::east, true);
        }

        if ((y > 0) && mmap.is_mushroom(self(x, y - 1, z))) {
          cur_blk.state().set_stoma(direction::down, true);
        }
        if ((y < this->y_range() - 1) && mmap.is_mushroom(self(x, y + 1, z))) {
          cur_blk.state().set_stoma(direction::up, true);
        }

        if ((z > 0) && mmap.is_mushroom(self(x, y, z - 1))) {
          cur_blk.state().set_stoma(direction::north, true);
        }
        if ((z < this->z_range() - 1) && mmap.is_mushroom(self(x, y, z + 1))) {
          cur_blk.state().set_stoma(direction::south, true);
        }

        // this step append a new block id when necessary, it's not read only
        self(x, y, z) = mmap[cur_blk];
      }
    }
  }
}