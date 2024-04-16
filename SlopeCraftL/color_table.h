//
// Created by joseph on 4/15/24.
//

#ifndef SLOPECRAFT_COLOR_TABLE_H
#define SLOPECRAFT_COLOR_TABLE_H

#include "SlopeCraftL.h"
#include "Colorset.h"
#include "simpleBlock.h"
#include <array>
#include <MapImageCvter/MapImageCvter.h>

class color_table_impl : public SlopeCraft::color_table {
 private:
  colorset_allowed_t allowed;
  SCL_mapTypes map_type_;
  SCL_gameVersion mc_version_;
  std::array<simpleBlock, 64> blocks;

 public:
  color_map_ptrs colors() const noexcept override {
    return color_map_ptrs{.r_data = allowed.rgb_data(0),
                          .g_data = allowed.rgb_data(1),
                          .b_data = allowed.rgb_data(2),
                          .map_data = allowed.map_data(),
                          .num_colors = allowed.color_count()};
  }

  SCL_mapTypes map_type() const noexcept override { return this->map_type_; }

  SCL_gameVersion mc_version() const noexcept override {
    return this->mc_version_;
  }

  size_t num_blocks() const noexcept override { return this->blocks.size(); }

  void visit_blocks(void (*fun)(const AbstractBlock *, void *custom_data),
                    void *custom_data) const override {
    for (auto &blk : this->blocks) {
      fun(static_cast<const AbstractBlock *>(&blk), custom_data);
    }
  }

  [[nodiscard]] converted_image *convert_image(
      const_image_reference original_img,
      const convert_option &option) const noexcept override;

  static std::optional<color_table_impl> create(
      const color_table_create_info &args) noexcept;
};

class converted_image_impl : public converted_image {
 public:
  converted_image_impl(converted_image_impl &&) = default;
  explicit converted_image_impl(const colorset_allowed_t &allowed_colorset);
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
};

#endif  // SLOPECRAFT_COLOR_TABLE_H
