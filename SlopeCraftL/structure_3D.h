//
// Created by joseph on 4/17/24.
//

#ifndef SLOPECRAFT_STRUCTURE_3D_H
#define SLOPECRAFT_STRUCTURE_3D_H

#include "SlopeCraftL.h"
#include "converted_image.h"
#include "Schem/Schem.h"
#include "water_item.h"

class structure_3D_impl : public structure_3D {
 private:
 public:
  libSchem::Schem schem;
  Eigen::ArrayXX<uint8_t>
      map_color;  // map color may be modified by lossy
                  // compression,so we store the modified one

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

  bool export_litematica(const char *filename,
                         const litematic_options &option) const noexcept final;
  bool export_vanilla_structure(
      const char *filename,
      const vanilla_structure_options &option) const noexcept final;
  bool export_WE_schem(const char *filename,
                       const WE_schem_options &option) const noexcept final;

  bool export_flat_diagram(
      const char *filename, const color_table &table,
      const flag_diagram_options &option) const noexcept final;

  [[nodiscard]] std::string save_cache(
      const std::filesystem::path &file) const noexcept;

  [[nodiscard]] static tl::expected<structure_3D_impl, std::string> load_cache(
      const std::filesystem::path &file) noexcept;

  uint64_t block_count() const noexcept final;

  template <class archive>
  void load(archive &ar) {
    ar(this->map_color);
    ar(this->schem);
  };

  template <class archive>
  void save(archive &ar) const {
    ar(this->map_color);
    ar(this->schem);
  }
};

#endif  // SLOPECRAFT_STRUCTURE_3D_H
