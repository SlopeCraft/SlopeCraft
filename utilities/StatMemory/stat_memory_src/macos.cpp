#include "stat_memory.h"
tl::expected<system_memory_info, std::string>
get_system_memory_info() noexcept {
#warning "TODO: get system memory on macos"
  return tl::make_unexpected("Yet not implemented for macos");
}

[[nodiscard]] tl::expected<self_memory_usage, std::string>
get_self_memory_info() noexcept {
#warning "TODO: get system memory on macos"
  return tl::make_unexpected("Yet not implemented for macos");
}