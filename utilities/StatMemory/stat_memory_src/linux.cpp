#include "stat_memory.h"
tl::expected<system_memory_info, std::string>
get_system_memory_info() noexcept {
  return tl::make_unexpected("Yet not implemented for Linux");
}

[[nodiscard]] tl::expected<self_memory_usage, std::string>
get_self_memory_info() noexcept {
  return tl::make_unexpected("Yet not implemented for Linux");
}