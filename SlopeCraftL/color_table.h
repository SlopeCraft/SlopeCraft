//
// Created by joseph on 4/15/24.
//

#ifndef SLOPECRAFT_COLOR_TABLE_H
#define SLOPECRAFT_COLOR_TABLE_H

#include <array>
#include <MapImageCvter/MapImageCvter.h>
#include "SlopeCraftL.h"
#include "Colorset.h"
#include "simpleBlock.h"
#include "Schem/Schem.h"
#include "WaterItem.h"

class color_table_impl : public SlopeCraft::color_table {
 public:
  colorset_allowed_t allowed;
  SCL_mapTypes map_type_;
  SCL_gameVersion mc_version_;
  std::array<simpleBlock, 64> blocks;

  color_map_ptrs colors() const noexcept final {
    return color_map_ptrs{.r_data = allowed.rgb_data(0),
                          .g_data = allowed.rgb_data(1),
                          .b_data = allowed.rgb_data(2),
                          .map_data = allowed.map_data(),
                          .num_colors = allowed.color_count()};
  }

  SCL_mapTypes map_type() const noexcept final { return this->map_type_; }

  SCL_gameVersion mc_version() const noexcept final {
    return this->mc_version_;
  }

  size_t num_blocks() const noexcept final { return this->blocks.size(); }

  void visit_blocks(void (*fun)(const AbstractBlock *, void *custom_data),
                    void *custom_data) const final {
    for (auto &blk : this->blocks) {
      fun(static_cast<const AbstractBlock *>(&blk), custom_data);
    }
  }

  [[nodiscard]] converted_image *convert_image(
      const_image_reference original_img,
      const convert_option &option) const noexcept final;

  [[nodiscard]] structure_3D *build(const converted_image &,
                                    const build_options &) const noexcept final;

  [[nodiscard]] static std::optional<color_table_impl> create(
      const color_table_create_info &args) noexcept;

  [[nodiscard]] std::vector<std::string_view> block_id_list() const noexcept;
};

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
    std::unordered_map<TokiPos, waterItem> water_list;
  };
  std::optional<height_maps> height_info(
      const build_options &option) const noexcept;
};

class structure_3D_impl : public structure_3D {
 private:
  libSchem::Schem schem;
  
  Eigen::ArrayXXi map_color;  // map color may be modified by lossy
                              // compression,so we store the modified one

 public:
  size_t shape_x() const noexcept final { return this->schem.x_range(); }
  size_t shape_y() const noexcept final { return this->schem.y_range(); }
  size_t shape_z() const noexcept final { return this->schem.z_range(); }
  size_t palette_length() const noexcept final {
    return this->schem.palette_size();
  }
  void get_palette(const char **buffer_block_id) const noexcept final {
    for (size_t i = 0; i < this->palette_length(); i++) {
      buffer_block_id[i] = this->schem.palette()[i].c_str();
    }
  }

  static std::optional<structure_3D_impl> create(
      const color_table_impl &, const converted_image_impl &cvted,
      const build_options &option) noexcept;
};

#endif  // SLOPECRAFT_COLOR_TABLE_H
