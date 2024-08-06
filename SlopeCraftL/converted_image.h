//
// Created by joseph on 4/17/24.
//

#ifndef SLOPECRAFT_CONVERTED_IMAGE_H
#define SLOPECRAFT_CONVERTED_IMAGE_H
#include <Eigen/Dense>
#include <MapImageCvter/MapImageCvter.h>
#include <filesystem>
#include <tl/expected.hpp>
#include <memory>

#include "SlopeCraftL.h"
#include "SCLDefines.h"
#include "mc_block.h"
#include "Schem/Schem.h"
#include "water_item.h"
#include "string_deliver.h"

class color_table_impl;

class converted_image_impl : public converted_image {
 public:
  converted_image_impl(converted_image_impl &&) = default;
  explicit converted_image_impl(const color_table_impl &table);
  using eimg_row_major =
      Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
  libMapImageCvt::MapImageCvter converter;

 private:
  const SCL_gameVersion game_version;
  const std::shared_ptr<const colorset_allowed_t> colorset;

 public:
  //  [[nodiscard]] uint64_t hash() const noexcept;

  size_t rows() const noexcept final { return this->converter.rows(); }
  size_t cols() const noexcept final { return this->converter.cols(); }

  int map_rows() const noexcept { return ceil(this->rows() / 128.0f); }
  int map_cols() const noexcept { return ceil(this->cols() / 128.0f); }

  void get_original_image(uint32_t *buffer) const noexcept final {
    Eigen::Map<eimg_row_major> buf{buffer, static_cast<int64_t>(this->rows()),
                                   static_cast<int64_t>(this->cols())};
    buf = this->converter.raw_image();
  }
  //  void get_dithered_image(uint32_t *buffer) const noexcept final {}
  void get_converted_image(uint32_t *buffer) const noexcept final {
    this->converter.converted_image(buffer, nullptr, nullptr, false);
  }

  void get_compressed_image(const structure_3D &structure,
                            uint32_t *buffer) const noexcept final;

  bool export_map_data(
      const map_data_file_options &option) const noexcept final;

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

  bool is_converted_from(const color_table &table_) const noexcept final;

  bool get_map_command(
      const map_data_file_give_command_options &option) const final;

  [[nodiscard]] libSchem::Schem assembled_maps(
      const assembled_maps_options &) const noexcept;

  bool export_assembled_maps_litematic(
      const char *filename, const assembled_maps_options &,
      const litematic_options &) const noexcept final;
  virtual bool export_assembled_maps_vanilla_structure(
      const char *filename, const assembled_maps_options &,
      const vanilla_structure_options &) const noexcept final;
};

[[nodiscard]] Eigen::Matrix<int, 3, 2> transform_mat_of(
    SCL_map_facing) noexcept;

[[nodiscard]] uint8_t rotation_of(SCL_map_facing) noexcept;

#endif  // SLOPECRAFT_CONVERTED_IMAGE_H
