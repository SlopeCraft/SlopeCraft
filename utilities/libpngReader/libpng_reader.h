//
// Created by Joseph on 2024/4/8.
//

#ifndef SLOPECRAFT_LIBPNG_READER_H
#define SLOPECRAFT_LIBPNG_READER_H

#include <span>
#include <expected>
#include <cstdint>
#include <vector>
#include <functional>
#include <string>
#include <tuple>

struct image_info {
  uint32_t rows{0};
  uint32_t cols{0};
};

[[nodiscard]] std::tuple<std::expected<image_info, std::string>, std::string>
parse_png_into_argb32(std::span<const uint8_t> png_file_in_bytes,
                      std::vector<uint32_t>& pixels_row_major) noexcept;

[[nodiscard]] std::tuple<std::expected<image_info, std::string>, std::string>
parse_png_into_argb32_flex(
    std::span<const uint8_t> png_file_in_bytes,
    const std::function<uint32_t*(const image_info&)> allocator) noexcept;
#endif  // SLOPECRAFT_LIBPNG_READER_H
