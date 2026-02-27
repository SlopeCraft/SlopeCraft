#include "stat_memory.h"
std::expected<system_memory_info, std::string>
get_system_memory_info() noexcept {
#warning "TODO: get system memory on macos"
  return std::unexpected("Yet not implemented for macos");
}

[[nodiscard]] std::expected<self_memory_usage, std::string>
get_self_memory_info() noexcept {
#warning "TODO: get system memory on macos"
  return std::unexpected("Yet not implemented for macos");
}