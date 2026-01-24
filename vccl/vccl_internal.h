/*
 Copyright © 2021-2026  TokiNoBug
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

#ifndef SLOPECRAFT_VCCL_INTERNAL_H
#define SLOPECRAFT_VCCL_INTERNAL_H

#include <VisualCraftL.h>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

struct inputs {
  // resource
  std::vector<std::string> zips;
  std::vector<std::string> jsons;

  // colors
  SCL_gameVersion version;
  uint8_t layers{1};
  VCL_face_t face;
  SCL_convertAlgo algo;
  bool dither{false};
  bool show_color_num{false};
  VCL_biome_t biome{VCL_biome_t::the_void};
  bool leaves_transparent{false};
  //  images
  std::vector<std::string> images;

  // exports
  std::string prefix;
  bool make_converted_image{false};
  bool make_flat_diagram{false};
  int flat_diagram_splitline_margin_row{16};
  int flat_diagram_splitline_margin_col{16};
  bool make_litematic{false};
  bool make_schematic{false};
  bool make_structure{false};
  bool structure_is_air_void{true};

  inline bool need_to_read() const noexcept { return true; }

  inline bool need_to_convert() const noexcept {
    return this->make_converted_image || this->make_flat_diagram ||
           this->need_to_build();
  }
  inline bool need_to_build() const noexcept {
    return this->make_litematic || this->make_schematic || this->make_structure;
  }
  // compute
  uint16_t num_threads;
  bool benchmark{false};

  // gpu
  bool prefer_gpu{false};
  bool show_gpu{false};
  uint8_t platform_idx{0};
  uint8_t device_idx{0};
  bool list_gpu{false};

  // others
  bool disable_config{false};
  bool list_supported_formats{false};
  bool list_blockstates{false};
  bool list_models{false};
  bool list_textures{false};
  bool export_test_lite{false};
};

int run(const inputs &input) noexcept;
int set_resource(VCL_Kernel *kernel, const inputs &input) noexcept;
int set_allowed(VCL_block_state_list *bsl, const inputs &input) noexcept;

SCL_convertAlgo str_to_algo(std::string_view str, bool &ok) noexcept;

int list_gpu();

void list_supported_formats() noexcept;

#endif  // #ifndef SLOPECRAFT_VCCL_INTERNAL_H