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

#include <CLI11.hpp>
#include <thread>

#include "vccl_internal.h"
#include <QCoreApplication>
#include <VCLConfigLoader.h>
#include <magic_enum.hpp>

#include <SC_version_buildtime.h>
#include <fmt/format.h>

using std::cout, std::endl;

bool validate_input(const inputs &input) noexcept;

int main(int argc, char **argv) {
  inputs input;
  CLI::App app;

  const bool is_gpu_accessible = VCL_platform_num() > 0;

  if (is_gpu_accessible) {
    input.prefer_gpu = true;
  }

  app.set_version_flag("--version,-v", SC_VERSION_STR);
  bool show_config{false};
  app.add_flag("--show-config,--sc", show_config,
               "Show buildtime configuration and exit.")
      ->default_val(false);

  // resource
  app.add_option("--resource-pack,--rp", input.zips, "Resource packs")
      ->check(CLI::ExistingFile);
  app.add_option("--block-state-list,--bsl", input.jsons,
                 "Block state list json files")
      ->check(CLI::ExistingFile);

  // colors
  int __version;
  app.add_option("--mcver", __version, "MC version")
      ->default_val(19)
      ->check(CLI::Range(12, 21, "Avaliable versions"));

  app.add_option("--layers,--layer", input.layers, "Max layers")
      ->default_val(1)
      ->check(CLI::Range(1, 3, "Avaliable depth"));
  std::string __face;
  app.add_option("--face", __face, "Facing direction")
      ->default_val("up")
      ->check(CLI::IsMember({"up", "down", "north", "south", "east", "west"}));
  std::string algo;
  app.add_option("--algo", algo, "Algorithm for conversion")
      ->default_val("RGB")
      ->check(
          CLI::IsMember({"RGB", "RGB_Better", "HSV", "Lab94", "Lab00", "XYZ"}))
      ->expected(1);
  std::string biome;
  app.add_option("--biome", biome, "The biome where a pixel art is placed.")
      ->default_val("the_void")
      ->check(CLI::IsMember(magic_enum::enum_names<VCL_biome_t>()))
      ->expected(1);
  app.add_flag("--leaves-transparent,--ltp", input.leaves_transparent)
      ->default_val(false);

  app.add_flag("--dither", input.dither,
               "Use Floyd-Steinberg dithering to improve the result of image "
               "conversion")
      ->default_val(false);
  app.add_flag("--show-num-color,--snc", input.show_color_num,
               "Show the number of colors")
      ->default_val(false);

  //  images
  app.add_option("--src-img,--simg,--img", input.images, "Images to convert")
      ->check(CLI::ExistingFile);

  // exports
  app.add_option("--prefix", input.prefix, "Filename prefix of generate output")
      ->default_val("./");
  app.add_flag("--out-image,--oimg", input.make_converted_image,
               "Generate converted image")
      ->default_val(false);
  app.add_flag("--out-flag-diagram,--ofd", input.make_flat_diagram,
               "Generated flat diagram")
      ->default_val(false);
  app.add_option("--flat-diagram-splitline-margin-row,--fdslmr,--fdsmr",
                 input.flat_diagram_splitline_margin_row,
                 "Row margin of split line in flat diagram. Non positive "
                 "values indicates that no splitline is drawn.")
      ->default_val(16);
  app.add_option("--flat-diagram-splitline-margin-col,--fdslmc,--fdsmc",
                 input.flat_diagram_splitline_margin_col,
                 "Col margin of split line in flat diagram. Non negative "
                 "values indicates that no splitline is drawn.")
      ->default_val(16);

  app.add_flag("--litematic,--lite", input.make_litematic,
               "Export .litematic files for litematica mod")
      ->default_val(false);
  app.add_flag("--schematic,--schem", input.make_schematic,
               "Export .schem for World Edit mod")
      ->default_val(false);
  app.add_flag("--structure,--nbt", input.make_structure,
               "Export .nbt file for vanilla strcuture block")
      ->default_val(false);
  app.add_flag("--nbt-air-void,--nav", input.structure_is_air_void,
               "Represent air as structure void in vanilla structure")
      ->default_val(true);

  // compute
  app.add_flag("--benchmark", input.benchmark, "Display the performance data")
      ->default_val(false);

  app.add_option("--threads,-j", input.num_threads,
                 "CPU threads used to convert images")
      ->check(CLI::PositiveNumber)
      ->default_val(std::thread::hardware_concurrency());

  // gpu
  app.add_flag("--gpu", input.prefer_gpu, "Use gpu as much as possible")
      ->default_val(is_gpu_accessible);

  app.add_option("--platform", input.platform_idx, "The number of GPU platform")
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);
  app.add_option("--device", input.device_idx,
                 "The number of GPU device on assigned platform")
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);
  app.add_flag("--show-gpu", input.show_gpu)->default_val(false);
  app.add_flag("--list-gpu", input.list_gpu,
               "List all avaliable GPU platforms and devices and exit")
      ->default_val(false);

  // others
  app.add_flag("--disable-config", input.disable_config,
               "Disable default option provided by vccl-config.json")
      ->default_val(false);
  app.add_flag("--list-image-formats,--list-formats,--lif",
               input.list_supported_formats,
               "List all supported image formats and exit")
      ->default_val(false);
  app.add_flag("--list-blockstates,--list-blockstate,--lbs",
               input.list_blockstates,
               "List all blocks jsons in the resource pack.")
      ->default_val(false);
  app.add_flag("--list-models,--list-model,--lmd", input.list_models,
               "List all block models in the resource pack.")
      ->default_val(false);
  app.add_flag("--list-textures,--list-texture", input.list_textures,
               "List all textures in the resource pack.")
      ->default_val(false);
  app.add_flag("--export-test-litematic,--export-test-lite,--etl",
               input.export_test_lite,
               "Export a testing lite that contains all avaliable blocks.")
      ->default_val(false);

  CLI11_PARSE(app, argc, argv);

  if (show_config) {
    fmt::println("Version : {}", SC_VERSION_STR);
    fmt::println("Build type : {}", CMAKE_BUILD_TYPE);
    fmt::println("GPU API : {}", SC_GPU_API);
    fmt::println("Vectorize : {}", SC_VECTORIZE);
    fmt::println("Gprof : {}", SC_GPROF);
    return 0;
  }

  if (input.list_gpu) {
    return list_gpu();
  }

  input.version = SCL_gameVersion(__version);

  bool ok = true;

  input.face = VCL_str_to_face_t(__face.c_str(), &ok);
  if (!ok) {
    return __LINE__;
  }

  input.algo = str_to_algo(algo, ok);

  {
    auto temp = magic_enum::enum_cast<VCL_biome_t>(biome);
    ok = temp.has_value();
    if (ok) {
      input.biome = temp.value();
    }
  }

  if (!ok) {
    return __LINE__;
  }
  if (!input.disable_config) {
    VCL_config cfg;
    if (!load_config("vccl-config.json", cfg)) {
      cout << "Failed to load config. Skip and continue" << endl;
    } else {
      for (std::string &str : cfg.default_jsons) {
        input.jsons.emplace_back(std::move(str));
      }

      input.zips.emplace_back(cfg.default_zips.at(input.version));

      cout << "Default config loaded" << endl;
    }
  }

  if (!validate_input(input)) {
    return __LINE__;
  }

  QCoreApplication qapp(argc, argv);
  if (input.list_supported_formats) {
    list_supported_formats();
    qapp.quit();
    return 0;
  }

  const int ret = run(input);
  qapp.quit();

  return ret;
}

bool validate_input(const inputs &input) noexcept {
  if (input.make_schematic && input.version <= SCL_gameVersion::MC12) {
    cout << "Invalid input : .schem can not be exported within 1.12" << endl;
    return false;
  }
  return true;
}