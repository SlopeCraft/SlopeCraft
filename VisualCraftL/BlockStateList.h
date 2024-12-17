/*
 Copyright © 2021-2023  TokiNoBug
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

#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H

#include <bitset>
#include <string>
#include <unordered_map>
#include <utilities/SC_GlobalEnums.h>
#include <vector>
#include <functional>
#include <Eigen/Dense>

#include "version_set.hpp"
#include "VisualCraftL.h"

class VCL_block_state_list;

class VCL_block {
  friend class VCL_block_state_list;

 public:
  VCL_block();
  VCL_block(const std::string *full_id_ptr);

  using attribute = ::VCL_block_attribute_t;

  static constexpr size_t idx_face_up = (size_t)attribute::face_up;
  static constexpr size_t idx_face_down = (size_t)attribute::face_down;
  static constexpr size_t idx_face_north = (size_t)attribute::face_north;
  static constexpr size_t idx_face_south = (size_t)attribute::face_south;
  static constexpr size_t idx_face_east = (size_t)attribute::face_east;
  static constexpr size_t idx_face_west = (size_t)attribute::face_west;

  static_assert(idx_face_up == 0);
  static_assert(idx_face_up < idx_face_down);
  static_assert(idx_face_down < idx_face_north);
  static_assert(idx_face_north < idx_face_south);
  static_assert(idx_face_south < idx_face_east);
  static_assert(idx_face_east < idx_face_west);
  static_assert(idx_face_west == 5);

  static constexpr size_t idx_transparent = (size_t)attribute::transparency;
  static constexpr size_t idx_background = (size_t)attribute::background;
  static constexpr size_t id_burnable = (size_t)attribute::burnable;
  static constexpr size_t idx_enderman_pickable =
      (size_t)attribute::enderman_pickable;
  static constexpr size_t idx_is_glowing = (size_t)attribute::is_glowing;
  static constexpr size_t idx_disabled = (size_t)attribute::disabled;
  static constexpr size_t idx_is_air = (size_t)attribute::is_air;
  static constexpr size_t idx_is_grass = (size_t)attribute::is_grass;
  static constexpr size_t idx_is_foliage = (size_t)attribute::is_foliage;

  inline bool get_attribute(attribute a) const noexcept {
    return this->attributes[size_t(a)];
  }

  inline void set_attribute(attribute a, bool val) noexcept {
    this->attributes[size_t(a)] = val;
  }

  inline bool is_transparent() const noexcept {
    return this->attributes[idx_transparent];
  }

  inline void set_transparency(bool ts) noexcept {
    this->attributes[idx_transparent] = ts;
  }

  inline bool is_face_available(VCL_face_t f) const noexcept {
    return this->attributes[size_t(f)];
  }

  inline void set_face_avaliablity(VCL_face_t f, bool val) noexcept {
    this->attributes[size_t(f)] = val;
  }

  inline bool is_background() const noexcept {
    return this->attributes[idx_background];
  }

  inline void set_is_background(bool val) noexcept {
    this->attributes[idx_background] = val;
  }

  inline bool is_disabled() const noexcept {
    return this->attributes[idx_disabled];
  }

  inline void set_disabled(bool disable) noexcept {
    this->attributes[idx_disabled] = disable;
  }

  inline bool is_air() const noexcept { return this->attributes[idx_is_air]; }

  inline const std::string *full_id_ptr() const noexcept {
    return this->full_id_p;
  }

  inline void disable_all_faces() noexcept {
    for (size_t idx = idx_face_up; idx <= idx_face_west; idx++) {
      this->attributes[idx] = false;
    }
  }

  inline bool match(SCL_gameVersion v, VCL_face_t f) const noexcept {
    return this->version_info.match(v) && this->is_face_available(f) &&
           !this->is_disabled();
  }

  inline const std::string &id_for_schem(SCL_gameVersion v) const noexcept {
    for (const auto &pair : this->id_replace_list) {
      if (pair.first == v) {
        return pair.second;
      }
    }
    return *this->full_id_p;
  }

 private:
  void initialize_attributes() noexcept;
  // members
 public:
  version_set version_info;

 private:
  std::bitset<32> attributes;

 public:
  VCL_block_class_t block_class{VCL_block_class_t::others};

 private:
  const std::string *full_id_p{nullptr};

 public:
  Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      project_image_on_exposed_face{0, 0};
  std::string name_ZH{""};
  std::string name_EN{""};
  std::vector<std::pair<SCL_gameVersion, std::string>> id_replace_list{};
};

class VCL_block_state_list {
 private:
  std::unordered_map<std::string, VCL_block> states;

 public:
  using is_allowed_callback_t = std::function<bool(const VCL_block *)>;
  bool add(std::string_view filename) noexcept;

  void available_block_states(
      SCL_gameVersion v, VCL_face_t f,
      std::vector<VCL_block *> *const str_list) noexcept;

  void avaliable_block_states_by_transparency(
      SCL_gameVersion v, VCL_face_t f,
      std::vector<VCL_block *> *const list_non_transparent,
      std::vector<VCL_block *> *const list_transparent) noexcept;

  inline auto &block_states() const noexcept { return this->states; }
  inline auto &block_states() noexcept { return this->states; }

  inline VCL_block *block_at(const std::string &str) noexcept {
    auto it = this->states.find(str);

    if (it == this->states.end()) {
      return nullptr;
    }

    return &it->second;
  }

  void update_foliages(bool is_foliage_transparent) noexcept;
};

VCL_block_class_t string_to_block_class(std::string_view str,
                                        bool *ok = nullptr) noexcept;

#endif  // SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H