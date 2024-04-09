//
// Created by Joseph on 2024/4/8.
//

#ifndef SLOPECRAFT_LIBPNG_READER_H
#define SLOPECRAFT_LIBPNG_READER_H

#include <span>
#include <tl/expected.hpp>
#include <cstdint>
#include <vector>
#include <string>
#include <tuple>

struct image_info {
  uint32_t rows{0};
  uint32_t cols{0};
};

[[nodiscard]] std::tuple<tl::expected<image_info, std::string>, std::string>
parse_png_into_argb32(std::span<const uint8_t> png_file_in_bytes,
                      std::vector<uint32_t>& pixels_row_major) noexcept;

#endif  // SLOPECRAFT_LIBPNG_READER_H
