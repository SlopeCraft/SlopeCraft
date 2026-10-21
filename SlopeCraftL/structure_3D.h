//
// Created by joseph on 4/17/24.
//

#ifndef SLOPECRAFT_STRUCTURE_3D_H
#define SLOPECRAFT_STRUCTURE_3D_H

#include <mc_schem.hpp>

#include "SlopeCraftL.h"
#include "converted_image.h"
#include "water_item.h"

class structure_3D_impl : public structure_3D {
 private:
  // A copy of palette block id. This is to serve get_palette function.
  // This violates single-information source rule, if any conflicts with schem,
  // schem is right. This is not good, but don't know how to improve.
  mutable std::vector<std::string> palette_copy;

 public:
  mc_schem::unique_schematic schem{nullptr};
  // map color may be modified by lossy compression,so we store the modified one
  Eigen::ArrayXX<uint8_t> map_color;

  [[nodiscard]] size_t shape_x() const noexcept final {
    return this->schem->shape<size_t>()[0];
  }
  [[nodiscard]] size_t shape_y() const noexcept final {
    return this->schem->shape<size_t>()[1];
  }
  [[nodiscard]] size_t shape_z() const noexcept final {
    return this->schem->shape<size_t>()[2];
  }
  [[nodiscard]] size_t palette_length() const noexcept final {
    return this->schem->get_region(0)->palette_size();
  }
  void get_palette(const char** buffer_block_id) const noexcept final {
    auto region = this->schem->get_region(0);
    assert(region);
    this->palette_copy.resize(region->palette_size(), "");
    for (size_t i = 0; i < region->palette_size(); i++) {
      palette_copy[i] = region->palette(i)->full_id();
      buffer_block_id[i] = palette_copy[i].c_str();
    }
  }

  static std::optional<structure_3D_impl> create(
      const color_table_impl&, const converted_image_impl& cvted,
      const build_options& option) noexcept;

  bool export_litematica(const char* filename,
                         const litematic_options& option) const noexcept final;
  bool export_vanilla_structure(
      const char* filename,
      const vanilla_structure_options& option) const noexcept final;
  bool export_WE_schem(const char* filename,
                       const WE_schem_options& option) const noexcept final;

  bool export_flat_diagram(
      const char* filename, const color_table& table,
      const flag_diagram_options& option) const noexcept final;

  uint64_t block_count() const noexcept final;
};

#endif  // SLOPECRAFT_STRUCTURE_3D_H
