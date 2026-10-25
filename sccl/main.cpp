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
#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <string>
#include <map>
#include <optional>
#include <iostream>
#include <format>

#include <QDir>
#include <QTranslator>
#include <QCoreApplication>

#include <CLI/CLI.hpp>
#include <magic_enum/magic_enum.hpp>

#include <SC_version_buildtime.h>
#include <SlopeCraftL.h>
#include "sccl_internal.h"

#include <thread>

template <class E>
[[nodiscard]] std::map<std::string, E> enum_mapping(
    const std::set<E> excludes = {}) noexcept {
  std::map<std::string, E> ret;
  for (auto [val, key] : magic_enum::enum_entries<E>()) {
    if (excludes.contains(val)) {
      continue;
    }
    ret.emplace(std::string(key), val);
  }
  return ret;
}

int main(int argc, char** argv) {
  QCoreApplication qapp{argc, argv};

  inputs input;
  CLI::App app;
  app.set_version_flag("--version,-v", SC_VERSION_STR);
  app.set_help_all_flag("--help-all", "Show options for all subcommands");
  bool show_config{false};
  app.add_flag("--show-config,--sc", show_config,
               "Show build-time configuration and exit.")
      ->default_val(false);
  bool build_dir_mode{false};
  app.add_flag("--build-dir-mode", build_dir_mode,
               "Read blocks and config files from local dir, only useful for "
               "development")
      ->default_val(false);
  app.add_option(
         "-j,--parallel,--threads", input.num_threads,
         "Number of threads for convert and build. 0 for hardware concurrency")
      ->check(CLI::NonNegativeNumber)
      ->default_val(0);
  // colors
  const std::string group_color{"Color"};
  const auto SCL_exclude_versions = {SCL_gameVersion::ANCIENT,
                                     SCL_gameVersion::FUTURE};
  app.add_option("--mcver", input.version, "MC version")
      ->transform(CLI::CheckedTransformer{
        enum_mapping<SCL_gameVersion>(SCL_exclude_versions)})
      ->required()
      ->group(group_color);
  app.add_option("--map-type", input.map_type, "Map type string")
      ->transform(CLI::CheckedTransformer{enum_mapping<SCL_mapTypes>()})
      ->default_val(magic_enum::enum_name(input.map_type))
      ->group(group_color);
  app.add_option("--algo", input.algo, "Image converting algorithm")
      ->transform(CLI::CheckedTransformer{enum_mapping<SCL_convertAlgo>()})
      ->default_val(magic_enum::enum_name(input.algo))
      ->group(group_color);
  app.add_flag("--dither", input.dither, "Use dithering for image conversion")
      ->default_val(input.dither)
      ->group(group_color);
  app.add_option("--preset", input.preset_json, "Preset json for block list")
      ->default_val(input.preset_json.string())
      ->group(group_color);
  app.add_option("--disable-basecolor", input.disable_base_colors,
                 "Disable some base color")
      ->check(CLI::Range{uint8_t{0}, SlopeCraft::SCL_maxBaseColor(),
                         "Valid base color"})
      ->group(group_color);
  app.add_option("--add-blocklist", input.extra_block_lists,
                 "Add extra block lists apart from FixedBlocks.zip and "
                 "CustomBlocks.zip")
      ->check(CLI::ExistingFile)
      ->group(group_color);
  // images
  const std::string group_image{"Image"};
  app.add_option("--image", input.images, "Image files")
      ->required()
      ->check(CLI::ExistingFile)
      ->group(group_image);
  app.add_option("--preprocess-pure-transparent",
                 input.preprocess.pure_transparent,
                 "Strategy for pure transparent pixels")
      ->transform(CLI::CheckedTransformer{enum_mapping<SCL_PureTpPixelSt>()})
      ->default_val(magic_enum::enum_name(input.preprocess.pure_transparent))
      ->group(group_image);
  app.add_option("--preprocess-half-transparent",
                 input.preprocess.half_transparent,
                 "Strategy for half transparent pixels")
      ->transform(CLI::CheckedTransformer{enum_mapping<SCL_HalfTpPixelSt>()})
      ->default_val(magic_enum::enum_name(input.preprocess.half_transparent))
      ->group(group_image);
  app.add_option("--preprocess-background-color", input.preprocess.background,
                 "Background color in ARGB")
      ->default_val(input.preprocess.background)
      ->group(group_image);
  // build options
  const std::string group_build{"3D structure"};
  app.add_flag(
         "--lossless-compression", input.lossless_compression,
         "Compress maximum height without changing colors (breaks continuity)")
      ->default_val(input.lossless_compression)
      ->group(group_build);
  app.add_flag("--lossy-compression", input.lossy_compression,
               "Compress maximum height by modifying pixels (works good with "
               "lossless compression)")
      ->default_val(input.lossy_compression)
      ->group(group_build);
  app.add_option("--max-height", input.max_allowed_height,
                 "Maximum height for 3D structure")
      ->default_val(input.max_allowed_height)
      ->group(group_build);
  app.add_option("--glass-bridge", input.glass_bridge,
                 "Connect blocks horizontally with glass")
      ->default_val(input.glass_bridge)
      ->transform(
          CLI::CheckedTransformer{enum_mapping<SCL_glassBridgeSettings>()})
      ->group(group_build);
  app.add_option("--glass-bridge-interval", input.glass_bridge_interval,
                 "Vertical spacing for each glass bridge layers")
      ->default_val(input.glass_bridge_interval)
      ->check(CLI::NonNegativeNumber)
      ->group(group_build);
  app.add_flag("--fire-proof", input.fire_proof,
               "Wrap burnable blocks with glass")
      ->default_val(input.fire_proof)
      ->group(group_build);
  app.add_flag("--enderman-proof", input.enderman_proof,
               "Wrap enderman-pickable blocks with glass")
      ->default_val(input.enderman_proof)
      ->group(group_build);
  app.add_flag("--connect-mushroom", input.connect_mushroom,
               "Fix block-state of adjacent mushroom blocks")
      ->default_val(input.connect_mushroom)
      ->group(group_build);
  app.add_option(
         "--support-block", input.support_block,
         "Add support blocks under color-showing blocks to help construction")
      ->default_val(input.support_block)
      ->transform(
          CLI::CheckedTransformer{enum_mapping<SCL_supportBlockSettings>()})
      ->group(group_build);

  // exports
  app.add_option("--export-prefix", input.export_prefix,
                 "Directory to export litematic/schematic/nbt/images")
      ->required()
      ->check(CLI::ExistingDirectory)
      ->group("Export");
  //////////////////////////////////////////////////////////////////////////////
  export_converted_image_option ecio{};
  auto cmd_converted_image =
      app.add_subcommand("export-converted-image", "Exported converted image");
  //////////////////////////////////////////////////////////////////////////////
  export_map_data_files_option emdfo{};
  auto cmd_data_files =
      app.add_subcommand("export-map-data-file", "Export map data files");
  cmd_data_files
      ->add_option("--begin-index", emdfo.begin_index, "Begin index of maps")
      ->default_val(emdfo.begin_index)
      ->check(CLI::NonNegativeNumber);
  cmd_data_files
      ->add_flag("--version-greater-than-MC-1-20-5",
                 emdfo.version_greater_1_20_5,
                 "Whether version >= 1.20.5. Only valid for 1.20.x")
      ->default_val(emdfo.version_greater_1_20_5);
  ////////////////////////////////////////
  export_assembled_option eao{};
  auto cmd_assembled = cmd_data_files->add_subcommand(
      "export-assembled", "Export schematic containing assembled maps");
  cmd_assembled
      ->add_flag("--frame-transparent", eao.frame_transparent,
                 "Whether item frame is invisible")
      ->default_val(eao.frame_transparent);
  cmd_assembled
      ->add_flag("--frame-glowing", eao.frame_glowing,
                 "Whether item frame is glowing")
      ->default_val(eao.frame_glowing);
  cmd_assembled
      ->add_flag("--frame-fixed", eao.frame_fixed,
                 "Whether frame is not breakable")
      ->default_val(eao.frame_fixed);
  cmd_assembled->add_option("--facing", eao.map_facing, "Map facing")
      ->default_val(eao.map_facing)
      ->transform(CLI::CheckedTransformer{enum_mapping<SCL_map_facing>()});
  //////////////////////////////////////////////////////////////////////////////
  export_litematic_option elo{};
  auto cmd_litematica =
      app.add_subcommand("export-litematica", "Export as .litematic file");
  cmd_litematica->add_option(
      "--names", elo.litematica_names,
      "Names of each litematica (not filename, like meta data).");
  cmd_litematica->add_option("--region-names", elo.litematica_region_names,
                             "Names of each litematica main region");

  //////////////////////////////////////////////////////////////////////////////
  export_structure_option eso{};
  auto cmd_structure = app.add_subcommand("export-structure",
                                          "Export as vanilla structure (.nbt)");
  cmd_structure
      ->add_flag("--is-air-void", eso.structure_is_air_void,
                 "Replace air with structure void")
      ->default_val(input);

  //////////////////////////////////////////////////////////////////////////////
  export_WESchem_option eWEo{};
  auto cmd_weschem =
      app.add_subcommand("export-WEschem",
                         "Export as WorldEdit schematic (*.schem for 1.13+, "
                         "*.schematic for 1.12)");
  cmd_weschem->add_option("--names", eWEo.WESchem_names,
                          "Names of WE schematics (not filename)");
  cmd_weschem->add_option("--required-mods", eWEo.WESchem_required_mods,
                          "Required mods for WE schematic");
  //////////////////////////////////////////////////////////////////////////////
  export_flat_diagram_option efdo{};
  auto cmd_flagdiagram =
      app.add_subcommand("export-flat-diagram", "Export as flat diagram");
  cmd_flagdiagram
      ->add_option("--splitline-row-interval", efdo.splitline_row_interval,
                   "Row interval of split line by block. 0 means no split line")
      ->default_val(efdo.splitline_row_interval);
  cmd_flagdiagram
      ->add_option("--splitline-col-interval", efdo.splitline_col_interval,
                   "Col interval of split line by block. 0 means no split line")
      ->default_val(efdo.splitline_col_interval);

  //////////////////////////////////////////////////////////////////////////////
  CLI11_PARSE(app, argc, argv);

  if (cmd_converted_image->count() > 0) {
    input.converted_image_option = std::move(ecio);
  }

  if (cmd_data_files->count() > 0) {
    if (cmd_assembled->count() > 0) {
      emdfo.assembled_option = std::move(eao);
    }
    input.map_data_files_option = std::move(emdfo);
  }

  if (cmd_litematica->count() > 0) {
    input.litematica_option = std::move(elo);
  }

  if (cmd_structure->count() > 0) {
    input.structure_option = std::move(eso);
  }

  if (cmd_weschem->count() > 0) {
    input.WESchem_option = std::move(eWEo);
  }

  if (cmd_flagdiagram->count() > 0) {
    input.flat_diagram_option = std::move(efdo);
  }

  try {
    QDir dir{":/i18n"};
    const auto translate_files =
        dir.entryInfoList({"*_en_US.qm"}, QDir::Filter::NoFilter);
    std::vector<std::unique_ptr<QTranslator>> translators;
    for (auto& file : translate_files) {
      std::unique_ptr<QTranslator> ptr{new QTranslator{&qapp}};
      if (not ptr->load(file.filePath())) {
        std::cerr << "Failed to load translate file "
                  << file.filePath().toStdString() << std::endl;
        continue;
      }
      translators.emplace_back(std::move(ptr));
    }
    canonicalize(input, build_dir_mode);
    run(input, build_dir_mode);
  } catch (const std::exception& e) {
    std::cout.flush();
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}