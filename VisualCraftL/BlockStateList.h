#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H

#include <bitset>
#include <string>
#include <unordered_map>
#include <utilities/SC_GlobalEnums.h>
#include <vector>

#include <Eigen/Dense>

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

  inline bool contains(SCL_gameVersion v) const noexcept {
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
public:
  VCL_block();
  VCL_block(const std::string *full_id_ptr);

  enum class attribute { transparency = 0 };

  static constexpr size_t idx_transparent = (size_t)attribute::transparency;
  version_set version_info;

  inline const std::string *full_id_ptr() const noexcept {
    return this->full_id_p;
  }

private:
  std::bitset<32> attributes;
  const std::string *full_id_p{nullptr};

  friend class VCL_block_state_list;

public:
  std::string name_ZH{""};
  std::string name_EN{""};

  Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      project_image_on_exposed_face{0, 0};

  inline bool is_transparent() const noexcept {
    return this->attributes[idx_transparent];
  }

  inline void set_transparency(bool ts) noexcept {
    this->attributes[idx_transparent] = ts;
  }
};

class VCL_block_state_list {
private:
  std::unordered_map<std::string, VCL_block> states;

public:
  bool add(std::string_view filename) noexcept;

  void
  available_block_states(SCL_gameVersion v,
                         std::vector<VCL_block *> *const str_list) noexcept;

  void avaliable_block_states_by_transparency(
      SCL_gameVersion v, std::vector<VCL_block *> *const list_non_transparent,
      std::vector<VCL_block *> *const list_transparent) noexcept;

  inline auto &block_states() const noexcept { return this->states; }

  inline VCL_block *block_at(const std::string &str) noexcept {
    auto it = this->states.find(str);

    if (it == this->states.end()) {
      return nullptr;
    }

    return &it->second;
  }
};

#endif // SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H