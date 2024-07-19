#include "stat_memory.h"
#include <Windows.h>
#include <Psapi.h>

#ifdef max
#undef max
#endif

#include <cmath>

tl::expected<system_memory_info, std::string>
get_system_memory_info() noexcept {
  MEMORYSTATUS ms;
  GlobalMemoryStatus(&ms);

  return system_memory_info{
      .total = ms.dwTotalPhys,
      .free = ms.dwAvailPhys,
  };
}

[[nodiscard]] tl::expected<self_memory_usage, std::string>
get_self_memory_info() noexcept {
  HANDLE handle = GetCurrentProcess();
  if (handle == nullptr) {
    return tl::make_unexpected(
        "win32 api failed, GetCurrentProcess returned nullptr instead of a "
        "handle to current process.");
  }
  PROCESS_MEMORY_COUNTERS pmc;
  GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
  self_memory_usage result{.used =
                               std::max(pmc.PagefileUsage, pmc.WorkingSetSize)};
  return result;
}