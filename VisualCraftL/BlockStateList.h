#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H

#include <bitset>
#include <string>
#include <unordered_map>
#include <utilities/SC_GlobalEnums.h>
#include <vector>


constexpr inline size_t major_version_to_idx(SCL_gameVersion v) noexcept {
  switch (v) {
  case SCL_gameVersion::FUTURE:
    return 63;

  default:
    return size_t(v);
  }
}

class version_set {
private:
  std::bitset<64> set{0};

public:
  version_set() = default;

  version_set(uint64_t val) : set(val) {}

  static version_set all() noexcept { return version_set(~uint64_t(0)); }

  inline bool contains(SCL_gameVersion v) const noexcept {
    return set[major_version_to_idx(v)];
  }

  inline auto operator[](SCL_gameVersion v) noexcept {
    return set[major_version_to_idx(v)];
  }

  inline auto operator[](SCL_gameVersion v) const noexcept {
    return set[major_version_to_idx(v)];
  }

  inline uint64_t to_u64() const noexcept { return set.to_ullong(); }

  inline bool operator==(const version_set &vs) const noexcept {
    return this->to_u64() == vs.to_u64();
  }
};

/*
class version_class_hash {
public:
  inline uint64_t operator()(version_set vs) noexcept {
    return std::hash<uint64_t>()(vs.to_u64());
  }
};

*/

class VCL_block_state_list {
private:
  std::unordered_map<std::string, version_set> states;

public:
  bool add(std::string_view filename) noexcept;

  void available_block_states(
      SCL_gameVersion v,
      std::vector<const std::string *> *const str_list) const noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_BLOCKSTATELIST_H