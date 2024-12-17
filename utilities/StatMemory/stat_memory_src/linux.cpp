#include "stat_memory.h"
#include <string>
#include <string_view>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <fmt/format.h>
#include <functional>

#include <ranges>
#include <unistd.h>

#include <unistd.h>

tl::expected<std::map<std::string, int64_t>, std::string> parse_linux_file(
    const char* filename,
    const std::function<bool(std::string_view)>& skip_this_line = {}) noexcept {
  std::map<std::string, int64_t> fields;
  std::string mem_info;
  {
    FILE* fd = fopen(filename, "r");
    if (fd == nullptr) {
      return tl::make_unexpected(
          fmt::format("Failed to read \"{}\", fopen returned NULL", filename));
    }
    std::array<char, 4096> buf;
    while (true) {
      const size_t bytes = fread(buf.data(), 1, buf.size(), fd);
      mem_info.append(buf.data(), bytes);
      if (bytes < buf.size()) {  // end of file
        break;
      }
    }
    fclose(fd);
  }
  for (auto line : std::views::split(mem_info, '\n')) {
    std::string_view line_sv{line.data(), line.size()};
    if (skip_this_line and skip_this_line(line_sv)) {
      // if skip_this_line is not null, and this line is considered to be
      // skipped
      continue;
    }

    if (line_sv.empty()) {
      continue;
    }
    const size_t idx_of_colon = line_sv.find_first_of(':');
    if (idx_of_colon == line_sv.npos) {  // Failed to parse this line, skip it.
      continue;
      //      return tl::make_unexpected(
      //          fmt::format("Failed to parse \"{}\" from {}", line_sv,
      //          filename));
    }

    std::string_view key{line_sv.begin(), idx_of_colon};

    std::string_view value_str{line_sv.begin() + idx_of_colon + 1,
                               line_sv.end()};
    std::string number_str{value_str.begin(), value_str.end()};
    uint64_t amplifier = 1;
    for (char& c : number_str) {  // to lower
      if (c >= 'A' and c <= 'Z') {
        c = std::tolower(c);
      }
    }
    if (number_str.ends_with("kb")) {
      amplifier = 1000;
      for (int i = 0; i < 2; i++) number_str.pop_back();
    }
    if (number_str.ends_with("kib")) {
      amplifier = 1024;
      for (int i = 0; i < 3; i++) number_str.pop_back();
    }
    if (number_str.ends_with("mb")) {
      amplifier = 1000 * 1000;
      for (int i = 0; i < 2; i++) number_str.pop_back();
    }
    if (number_str.ends_with("mib")) {
      amplifier = 1024 * 1024;
      for (int i = 0; i < 3; i++) number_str.pop_back();
    }
    if (number_str.ends_with("gb")) {
      amplifier = 1000 * 1000 * 1000;
      for (int i = 0; i < 2; i++) number_str.pop_back();
    }
    if (number_str.ends_with("gib")) {
      amplifier = 1024 * 1024 * 1024;
      for (int i = 0; i < 3; i++) number_str.pop_back();
    }

    const int64_t number = std::atoll(number_str.c_str()) * amplifier;

    fields.emplace(key, number);
  }

  return fields;
}

tl::expected<system_memory_info, std::string>
get_system_memory_info() noexcept {
  auto fileds = parse_linux_file("/proc/meminfo", [](std::string_view line) {
    if (line.starts_with("MemTotal")) return false;
    if (line.starts_with("MemFree")) return false;
    return true;
  });
  if (not fileds) {
    return tl::make_unexpected(std::move(fileds).error());
  }
  auto& value = fileds.value();
  system_memory_info ret;
  auto it = value.find("MemTotal");
  if (it != value.end()) {
    ret.total = it->second;
  } else {
    return tl::make_unexpected(
        "Failed to parse field \"MemTotal\" from /proc/meminfo");
  }
  it = value.find("MemFree");
  if (it not_eq value.end()) {
    ret.free = it->second;
  } else {
    return tl::make_unexpected(
        "Failed to parse field \"MemFree\" from /proc/meminfo");
  }
  return ret;
}

[[nodiscard]] tl::expected<self_memory_usage, std::string>
get_self_memory_info() noexcept {
  const int64_t pid = getpid();
  const std::string file = fmt::format("/proc/{}/status", pid);
  auto fields = parse_linux_file(file.c_str(), [](std::string_view line) {
    return not line.starts_with("VmSize");
  });
  if (not fields) {
    return tl::make_unexpected(std::move(fields.error()));
  }

  auto& val = fields.value();
  auto it = val.find("VmSize");
  if (it not_eq val.end()) {
    const int64_t used_memory = it->second;
    if (used_memory < 0) {
      return tl::make_unexpected(fmt::format(
          "VmSize from {} is negative(the value is {})", file, used_memory));
    }
    return self_memory_usage{uint64_t(used_memory)};
  }

  return tl::make_unexpected(
      fmt::format("Failed to parse field VmSize from {}", file));
}
