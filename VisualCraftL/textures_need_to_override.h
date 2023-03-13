/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

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