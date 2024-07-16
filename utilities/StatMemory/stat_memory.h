//
// Created by Joseph on 2024/7/16.
//

#ifndef SLOPECRAFT_STAT_MEMORY_H
#define SLOPECRAFT_STAT_MEMORY_H

#include <cstdint>
#include <string>
#include <tl/expected.hpp>

// All units in byte
struct system_memory_info {
  uint64_t total;
  uint64_t free;
};

[[nodiscard]] tl::expected<system_memory_info, std::string>
get_system_memory_info() noexcept;

// All units in byte
struct self_memory_usage {
  uint64_t used;
};

[[nodiscard]] tl::expected<self_memory_usage, std::string>
get_self_memory_info() noexcept;

#endif  // SLOPECRAFT_STAT_MEMORY_H
