#include "stat_memory.h"
#include <iostream>
#include <format>
#include <print>

int main() {
  int fail_count = 0;
  {
    const auto sys_info = get_system_memory_info();
    if (sys_info) {
      const auto &val = sys_info.value();
      std::println("System free memory: {}", val.free);
      std::println("System total memory: {}", val.total);
    } else {
      std::println("Failed to get system memory info:\n{}", sys_info.error());
      fail_count++;
    }
  }

  const auto self_info = get_self_memory_info();
  if (self_info) {
    const auto &val = self_info.value();
    std::println("Memory used by this process: \"{}\"", val.used);
  } else {
    std::println("Failed to get self memory info:\n{}", self_info.error());
    fail_count++;
  }

  return fail_count;
}