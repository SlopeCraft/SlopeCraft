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
      std::print("System free memory: {}\n", val.free);
      std::print("System total memory: {}\n", val.total);
    } else {
      std::print("Failed to get system memory info:\n{}\n", sys_info.error());
      fail_count++;
    }
  }

  const auto self_info = get_self_memory_info();
  if (self_info) {
    const auto &val = self_info.value();
    std::print("Memory used by this process: \"{}\"\n", val.used);
  } else {
    std::print("Failed to get self memory info:\n{}\n", self_info.error());
    fail_count++;
  }

  return fail_count;
}