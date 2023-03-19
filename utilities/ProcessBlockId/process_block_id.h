#ifndef SLOPECRAFT_UTILITIES_PROCESS_BLOCK_ID_PROCESS_BLOCK_ID_H
#define SLOPECRAFT_UTILITIES_PROCESS_BLOCK_ID_PROCESS_BLOCK_ID_H

#include <string_view>
#include <utility>
#include <ranges>
#include <vector>

namespace blkid {

using char_range = std::ranges::subrange<const char*>;

bool is_valid_id(std::string_view str) noexcept;

bool process_blk_id(
    std::string_view str, char_range* namespace_name, char_range* pure_id,
    std::vector<std::pair<char_range, char_range>>* attributes) noexcept;

bool process_blk_id(
    std::string_view str, std::string* namespace_name, std::string* pure_id,
    std::vector<std::pair<std::string, std::string>>* attributes) noexcept;

}  // namespace blkid

#endif  // SLOPECRAFT_UTILITIES_PROCESS_BLOCK_ID_PROCESS_BLOCK_ID_H