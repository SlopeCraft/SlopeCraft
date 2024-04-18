//
// Created by joseph on 4/17/24.
//

#ifndef SLOPECRAFT_CONVERTED_IMAGE_H
#define SLOPECRAFT_CONVERTED_IMAGE_H
#include "SlopeCraftL.h"
#include "Colorset.h"
#include "mc_block.h"
#include "Schem/Schem.h"
#include "WaterItem.h"
#include <Eigen/Dense>
#include <MapImageCvter/MapImageCvter.h>
#include <filesystem>
#include <tl/expected.hpp>

class color_table_impl;

class converted_image_impl : public converted_image {
 public:
  converted_image_impl(converted_image_impl &&) = default;
  explicit converted_image_impl(const color_table_impl &table);
  using eimg_row_major =
      Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
  libMapImageCvt::MapImageCvter converter;

  size_t rows() const noexcept final { return this->converter.rows(); }
  size_t cols() const noexcept final { return this->converter.cols(); }

  void get_original_image(uint32_t *buffer) const noexcept final {
    Eigen::Map<eimg_row_major> buf{buffer, static_cast<int64_t>(this->rows()),
                                   static_cast<int64_t>(this->cols())};
    buf = this->converter.raw_image();
  }
  //  void get_dithered_image(uint32_t *buffer) const noexcept final {}
  void get_converted_image(uint32_t *buffer) const noexcept final {
    this->converter.converted_image(buffer, nullptr, nullptr, false);
  }

  struct height_maps {
    Eigen::ArrayXXi map_color;
    Eigen::ArrayXXi base;
    Eigen::ArrayXXi high_map;
    Eigen::ArrayXXi low_map;
    std::unordered_map<rc_pos, water_y_range> water_list;
  };
  std::optional<height_maps> height_info(
      const build_options &option) const noexcept;

  [[nodiscard]] static uint64_t convert_task_hash(
      const_image_reference original_img,
      const convert_option &option) noexcept;

  std::string save_cache(const std::filesystem::path &file) const noexcept;

  [[nodiscard]] static tl::expected<converted_image_impl, std::string>
  load_cache(const color_table_impl &table,
             const std::filesystem::path &file) noexcept;
};
#endif  // SLOPECRAFT_CONVERTED_IMAGE_H
