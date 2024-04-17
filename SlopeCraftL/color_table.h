//
// Created by joseph on 4/15/24.
//

#ifndef SLOPECRAFT_COLOR_TABLE_H
#define SLOPECRAFT_COLOR_TABLE_H

#include <array>
#include "SlopeCraftL.h"
#include "Colorset.h"
#include "mc_block.h"
#include "converted_image.h"

class color_table_impl : public SlopeCraft::color_table {
 public:
  colorset_allowed_t allowed;
  SCL_mapTypes map_type_;
  SCL_gameVersion mc_version_;
  std::array<mc_block, 64> blocks;

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

#endif  // SLOPECRAFT_COLOR_TABLE_H
