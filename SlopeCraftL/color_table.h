//
// Created by joseph on 4/15/24.
//

#ifndef SLOPECRAFT_COLOR_TABLE_H
#define SLOPECRAFT_COLOR_TABLE_H

#include <array>
#include <filesystem>
#include <tl/expected.hpp>
#include "SlopeCraftL.h"
#include "SCLDefines.h"
#include "mc_block.h"
#include "string_deliver.h"
#include "converted_image.h"

class color_table_impl : public SlopeCraft::color_table {
 public:
  std::shared_ptr<colorset_allowed_t> allowed{new colorset_allowed_t};
  SCL_mapTypes map_type_;
  SCL_gameVersion mc_version_;
  std::array<mc_block, 64> blocks;

  color_map_ptrs colors() const noexcept final {
    return color_map_ptrs{.r_data = allowed->rgb_data(0),
                          .g_data = allowed->rgb_data(1),
                          .b_data = allowed->rgb_data(2),
                          .map_data = allowed->map_data(),
                          .num_colors = allowed->color_count()};
  }

  SCL_mapTypes map_type() const noexcept final { return this->map_type_; }

  SCL_gameVersion mc_version() const noexcept final {
    return this->mc_version_;
  }

  size_t num_blocks() const noexcept final { return this->blocks.size(); }

  void visit_blocks(void (*fun)(const mc_block_interface *, void *custom_data),
                    void *custom_data) const final {
    for (auto &blk : this->blocks) {
      fun(static_cast<const mc_block_interface *>(&blk), custom_data);
    }
  }

  [[nodiscard]] converted_image *convert_image(
      const_image_reference original_img,
      const convert_option &option) const noexcept final;

  [[nodiscard]] static std::optional<color_table_impl> create(
      const color_table_create_info &args) noexcept;

  [[nodiscard]] std::vector<std::string_view> block_id_list(
      bool contain_air) const noexcept;

  [[nodiscard]] const mc_block *find_block_for_index(
      int index, std::string_view block_id) const noexcept;

  [[nodiscard]] uint64_t hash() const noexcept;

  [[nodiscard]] std::filesystem::path self_cache_dir(
      const char *cache_root_dir) const noexcept;

  [[nodiscard]] std::filesystem::path convert_task_cache_filename(
      const_image_reference original_img, const convert_option &option,
      const char *cache_root_dir) const noexcept;

  [[nodiscard]] bool has_convert_cache(
      const_image_reference original_img, const convert_option &option,
      const char *cache_root_dir) const noexcept final;

  [[nodiscard]] bool save_convert_cache(
      const_image_reference original_img, const convert_option &option,
      const converted_image &cvted, const char *cache_root_dir,
      string_deliver *error) const noexcept final {
    auto err =
        this->save_convert_cache(original_img, option, cvted, cache_root_dir);
    write_to_sd(error, err);
    return err.empty();
  }

  [[nodiscard]] std::string save_convert_cache(
      const_image_reference original_img, const convert_option &option,
      const converted_image &, const char *cache_root_dir) const noexcept;

  [[nodiscard]] converted_image *load_convert_cache(
      const_image_reference original_img, const convert_option &option,
      const char *cache_root_dir, string_deliver *error) const noexcept final {
    auto res = this->load_convert_cache(original_img, option, cache_root_dir);
    if (!res) {
      write_to_sd(error, res.error());
      return nullptr;
    }

    write_to_sd(error, "");

    return new converted_image_impl{std::move(res.value())};
  }

  [[nodiscard]] tl::expected<converted_image_impl, std::string>
  load_convert_cache(const_image_reference original_img,
                     const convert_option &option,
                     const char *cache_root_dir) const noexcept;

  [[nodiscard]] structure_3D *build(const converted_image &,
                                    const build_options &) const noexcept final;

  [[nodiscard]] std::filesystem::path build_task_cache_filename(
      const converted_image &, const build_options &,
      const char *cache_root_dir) const noexcept;

  [[nodiscard]] bool save_build_cache(
      const converted_image &, const build_options &, const structure_3D &,
      const char *cache_root_dir, string_deliver *error) const noexcept final;
  [[nodiscard]] bool has_build_cache(
      const converted_image &, const build_options &,
      const char *cache_root_dir) const noexcept final;
  [[nodiscard]] structure_3D *load_build_cache(
      const converted_image &, const build_options &,
      const char *cache_root_dir, string_deliver *error) const noexcept final;

  void stat_blocks(const structure_3D &s,
                   size_t buffer[64]) const noexcept final;

  bool generate_test_schematic(
      const char *filename,
      const test_blocklist_options &option) const noexcept final {
    auto err = this->impl_generate_test_schematic(filename, option);
    write_to_sd(option.err, err);
    return err.empty();
  }

  std::string impl_generate_test_schematic(
      std::string_view filename,
      const test_blocklist_options &option) const noexcept;
};

[[nodiscard]] std::array<uint32_t, 256> LUT_map_color_to_ARGB() noexcept;

//[[nodiscard]] std::string digest_to_string(
//    std::span<const uint64_t> hash) noexcept;

#endif  // SLOPECRAFT_COLOR_TABLE_H
