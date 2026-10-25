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
*/

#include <cstddef>
#include <filesystem>
#include <string>
#include <optional>
#include <set>
#include <cstdint>

// #include <SC_version_buildtime.h>
#include <SlopeCraftL.h>

struct preprocess_option {
  SCL_PureTpPixelSt pure_transparent{SCL_PureTpPixelSt::ReplaceWithBackGround};
  SCL_HalfTpPixelSt half_transparent{SCL_HalfTpPixelSt::ComposeWithBackGround};
  uint32_t background{0xFF'FF'FF'FF};
};

struct export_converted_image_option {};

struct export_assembled_option {
  bool frame_transparent{false};
  bool frame_glowing{false};
  bool frame_fixed{true};
  SCL_map_facing map_facing{SCL_map_facing::wall_north};
  // bool export_as_litematica{true};
};

struct export_map_data_files_option {
  uint32_t begin_index{0};
  bool version_greater_1_20_5{true};
  std::optional<export_assembled_option> assembled_option{std::nullopt};
};

struct export_litematic_option {
  std::vector<std::string> litematica_names{};
  std::vector<std::string> litematica_region_names{};
};

struct export_structure_option {
  bool structure_is_air_void{true};
};

struct export_WESchem_option {
  // std::array<int, 3> WESchem_offset{0, 0, 0};
  // std::array<int, 3> WESchem_WE_offset{0, 0, 0};
  std::vector<std::string> WESchem_names{};
  std::vector<std::string> WESchem_required_mods{};
};

struct export_flat_diagram_option {
  int splitline_row_interval{16};
  int splitline_col_interval{16};
};

struct inputs {
  uint32_t num_threads{0};
  // colors
  SCL_gameVersion version;
  SCL_mapTypes map_type{SCL_mapTypes::Slope};
  SCL_convertAlgo algo{SCL_convertAlgo::RGB_Better};
  bool dither{false};
  std::filesystem::path preset_json{"elegant"};
  std::set<uint8_t> disable_base_colors;
  std::vector<std::filesystem::path> extra_block_lists;
  // images
  std::vector<std::filesystem::path> images;
  preprocess_option preprocess;
  // build options
  bool lossless_compression{false};
  bool lossy_compression{true};
  uint32_t max_allowed_height{256};
  SCL_glassBridgeSettings glass_bridge{SCL_glassBridgeSettings::noBridge};
  uint32_t glass_bridge_interval{5};
  bool fire_proof{false};
  bool enderman_proof{false};
  bool connect_mushroom{true};
  SCL_supportBlockSettings support_block{SCL_supportBlockSettings::none};
  // exports
  std::filesystem::path export_prefix;

  std::optional<export_converted_image_option> converted_image_option{
    std::nullopt};
  std::optional<export_map_data_files_option> map_data_files_option{
    std::nullopt};
  std::optional<export_litematic_option> litematica_option{std::nullopt};
  // structure options
  std::optional<export_structure_option> structure_option{std::nullopt};
  // WE schem options
  std::optional<export_WESchem_option> WESchem_option{std::nullopt};
  // flat diagram options
  std::optional<export_flat_diagram_option> flat_diagram_option{std::nullopt};

  [[nodiscard]] bool need_build() const noexcept;
};

void canonicalize(inputs& inputs, bool build_dir_mode);

void run(const inputs& task, bool build_dir_mode);

[[nodiscard]] std::filesystem::path get_SCL_blocks_dir(bool build_dir_mode);