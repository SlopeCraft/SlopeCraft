#ifndef SLOPECRAFT_UTILITIES_PROCESS_BLOCK_ID_PROCESS_BLOCK_ID_H
#define SLOPECRAFT_UTILITIES_PROCESS_BLOCK_ID_PROCESS_BLOCK_ID_H

#include <string>
#include <string_view>
#include <utility>
#include <ranges>
#include <vector>
#include <cstdint>

namespace blkid {

struct id_info {
  bool contains_namespace;
  uint32_t num_attributes;
};

using char_range = std::ranges::subrange<const char*>;

bool is_valid_id(std::string_view str) noexcept;

bool process_blk_id(
    std::string_view str, char_range* namespace_name_nullable,
    char_range* pure_id_nullable,
    std::vector<std::pair<char_range, char_range>>* attributes_nullable,
    id_info* info_nullable = nullptr) noexcept;

bool process_blk_id(std::string_view str, std::string* namespace_name_nullable,
                    std::string* pure_id_nullable,
                    std::vector<std::pair<std::string, std::string>>*
                        attributes_nullable) noexcept;

bool process_state_list(
    const char_range range,
    std::vector<std::pair<char_range, char_range>>* attributes_nullable,
    size_t* num_nullable) noexcept;

}  // namespace blkid

#endif  // SLOPECRAFT_UTILITIES_PROCESS_BLOCK_ID_PROCESS_BLOCK_ID_H