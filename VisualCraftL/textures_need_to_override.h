#ifndef SLOPECRAFT_VISUALCRAFTL_TEXTURES_NEED_TO_OVERRIDE_H
#define SLOPECRAFT_VISUALCRAFTL_TEXTURES_NEED_TO_OVERRIDE_H

#include <string_view>

extern const std::string_view VCL_12_grass_texture_names[];
extern const size_t VCL_12_grass_texture_name_size;
extern const std::string_view VCL_12_foliage_texture_names[];
extern const size_t VCL_12_foliage_texture_name_size;

extern const std::string_view VCL_latest_grass_texture_names[];
extern const size_t VCL_latest_grass_texture_name_size;
extern const std::string_view VCL_latest_foliage_texture_names[];
extern const size_t VCL_latest_foliage_texture_name_size;

struct VCL_texture_names_to_override {
  VCL_texture_names_to_override() = delete;
  VCL_texture_names_to_override(bool is_MC12) {
    if (is_MC12) {
      this->grass = VCL_12_grass_texture_names;
      this->num_grass = VCL_12_grass_texture_name_size;
      this->foliage = VCL_12_foliage_texture_names;
      this->num_foliage = VCL_12_foliage_texture_name_size;
    } else {

      this->grass = VCL_latest_grass_texture_names;
      this->num_grass = VCL_latest_grass_texture_name_size;
      this->foliage = VCL_latest_foliage_texture_names;
      this->num_foliage = VCL_latest_foliage_texture_name_size;
    }
  }
  const std::string_view *grass;
  size_t num_grass;
  const std::string_view *foliage;
  size_t num_foliage;
};

#endif // SLOPECRAFT_VISUALCRAFTL_TEXTURES_NEED_TO_OVERRIDE_H