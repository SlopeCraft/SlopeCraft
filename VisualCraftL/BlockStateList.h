#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H

#include <bitset>
#include <string>
#include <unordered_map>
#include <utilities/SC_GlobalEnums.h>
#include <vector>

#include <Eigen/Dense>

#include "VisualCraftL.h"

constexpr inline size_t major_version_to_idx(SCL_gameVersion v) noexcept {
  switch (v) {
  case SCL_gameVersion::FUTURE:
    return 31;

  default:
    return size_t(v);
  }
}

class version_set {
private:
  std::bitset<32> set{0};

public:
  version_set() = default;

  version_set(uint64_t val) : set(val) {}

  static version_set all() noexcept {
    version_set ret(~uint32_t(0));
    return ret;
  }

  inline bool match(SCL_gameVersion v) const noexcept {
    return set[major_version_to_idx(v)];
  }

  inline auto operator[](SCL_gameVersion v) noexcept {
    return set[major_version_to_idx(v)];
  }

  inline auto operator[](SCL_gameVersion v) const noexcept {
    return set[major_version_to_idx(v)];
  }

  inline uint64_t to_u32() const noexcept { return set.to_ulong(); }

  inline bool operator==(const version_set &vs) const noexcept {
    return this->to_u32() == vs.to_u32();
  }
};

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

  static_assert(idx_face_up < idx_face_down);
  static_assert(idx_face_down < idx_face_north);
  static_assert(idx_face_north < idx_face_south);
  static_assert(idx_face_south < idx_face_east);
  static_assert(idx_face_east < idx_face_west);

  static constexpr size_t idx_transparent = (size_t)attribute::transparency;
  static constexpr size_t idx_background = (size_t)attribute::background;
  static constexpr size_t id_burnable = (size_t)attribute::burnable;
  static constexpr size_t idx_enderman_pickable =
      (size_t)attribute::enderman_pickable;
  static constexpr size_t idx_is_glowing = (size_t)attribute::is_glowing;
  static constexpr size_t idx_disabled = (size_t)attribute::disabled;

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

  inline const std::string *full_id_ptr() const noexcept {
    return this->full_id_p;
  }

  inline void disable_all_faces() noexcept {
    for (size_t idx = idx_face_up; idx < idx_face_west; idx++) {
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
  bool add(std::string_view filename) noexcept;

  void
  available_block_states(SCL_gameVersion v, VCL_face_t f,
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
};

#endif // SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H