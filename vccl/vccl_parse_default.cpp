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

#include "vccl_internal.h"
#include <QImage>
#include <QImageReader>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <omp.h>

void cb_progress_range_set(void *, int, int, int) {}
void cb_progress_add(void *, int) {}

using std::cout, std::endl;
#define VCCL_PRIVATE_MACRO_MAKE_CASE(enum_val) \
  if (str == #enum_val) {                      \
    ok = true;                                 \
    return SCL_convertAlgo::enum_val;          \
  }

SCL_convertAlgo str_to_algo(std::string_view str, bool &ok) noexcept {
  ok = false;

  VCCL_PRIVATE_MACRO_MAKE_CASE(RGB);
  VCCL_PRIVATE_MACRO_MAKE_CASE(RGB_Better);
  VCCL_PRIVATE_MACRO_MAKE_CASE(HSV);
  VCCL_PRIVATE_MACRO_MAKE_CASE(Lab94);
  VCCL_PRIVATE_MACRO_MAKE_CASE(Lab00);
  VCCL_PRIVATE_MACRO_MAKE_CASE(XYZ);
  VCCL_PRIVATE_MACRO_MAKE_CASE(gaCvter);

  return {};
}

int list_gpu() {
  const size_t plat_num = VCL_platform_num();
  cout << plat_num << " platforms found on this computer : \n";
  for (size_t pid = 0; pid < plat_num; pid++) {
    VCL_GPU_Platform *plat = VCL_get_platform(pid);
    if (plat == nullptr) {
      cout << "Failed to get platform " << pid << '\n';
      continue;
    }
    cout << "Platform " << pid << " : " << VCL_get_platform_name(plat) << '\n';

    const size_t dev_num = VCL_get_device_num(plat);
    for (size_t did = 0; did < dev_num; did++) {
      VCL_GPU_Device *dp = VCL_get_device(plat, did);
      if (dp == nullptr) {
        cout << "Failed to get device " << did << '\n';
        continue;
      }
      cout << "    Device " << did << " : " << VCL_get_device_name(dp) << '\n';
      VCL_release_device(dp);
    }
    VCL_release_platform(plat);
  }
  return 0;
}

int set_resource(VCL_Kernel *kernel, const inputs &input) noexcept {
  std::vector<const char *> zip_filenames, json_filenames;

  for (const auto &str : input.zips) {
    zip_filenames.emplace_back(str.c_str());
  }
  for (const auto &str : input.jsons) {
    json_filenames.emplace_back(str.c_str());
  }

  VCL_block_state_list *bsl =
      VCL_create_block_state_list(json_filenames.size(), json_filenames.data());

  if (bsl == nullptr) {
    cout << "Failed to parse block state list." << endl;
    VCL_destroy_kernel(kernel);
    return __LINE__;
  }

  VCL_resource_pack *rp =
      VCL_create_resource_pack(zip_filenames.size(), zip_filenames.data());

  if (rp == nullptr) {
    cout << "Failed to parse resource pack(s)." << endl;
    VCL_destroy_block_state_list(bsl);
    VCL_destroy_kernel(kernel);
    return __LINE__;
  }

  if (input.list_blockstates || input.list_models || input.list_textures) {
    VCL_display_resource_pack(rp, input.list_textures, input.list_blockstates,
                              input.list_models);
  }

  VCL_set_resource_option option;
  option.version = input.version;
  option.max_block_layers = input.layers;
  option.exposed_face = input.face;
  option.biome = input.biome;
  option.is_render_quality_fast = !input.leaves_transparent;

  if (!VCL_set_resource_move(&rp, &bsl, option)) {
    cout << "Failed to set resource pack" << endl;
    VCL_destroy_block_state_list(bsl);
    VCL_destroy_resource_pack(rp);
    VCL_destroy_kernel(kernel);
    return __LINE__;
  }

  VCL_destroy_block_state_list(bsl);
  VCL_destroy_resource_pack(rp);
  return 0;
}

int set_allowed(VCL_block_state_list *bsl, const inputs &input) noexcept {
  std::vector<VCL_block *> blocks;

  blocks.resize(VCL_get_blocks_from_block_state_list_match(
      bsl, input.version, input.face, nullptr, 0));

  const int num = VCL_get_blocks_from_block_state_list_match(
      bsl, input.version, input.face, blocks.data(), blocks.size());

  if (int(blocks.size()) != num) {
    return __LINE__;
  }

  if (!VCL_set_allowed_blocks(blocks.data(), blocks.size())) {
    cout << "Failed to set allowed blocks." << endl;
    return __LINE__;
  }

  return 0;
}

void list_supported_formats() noexcept {
  auto fmts = QImageReader::supportedImageFormats();
  cout << "Supported image formats : ";
  for (auto &qba : fmts) {
    cout << qba.data() << ", ";
  }
  cout << endl;
}

int run(const inputs &input) noexcept {
  double wt = 0;

  if (input.zips.size() <= 0 || input.jsons.size() <= 0) {
    cout << "No zips or jsons provided. exit." << endl;
    return __LINE__;
  }

  VCL_Kernel *kernel = VCL_create_kernel();
  if (kernel == nullptr) {
    cout << "Failed to create kernel." << endl;
    return __LINE__;
  }

  // cout << "algo = " << (char)input.algo << endl;

  kernel->set_prefer_gpu(input.prefer_gpu);
  if (input.prefer_gpu) {
    bool ok;
    while (true) {
      auto plat = VCL_get_platform(input.platform_idx);
      if (plat == nullptr) {
        ok = false;
        break;
      }

      auto dev = VCL_get_device(plat, input.device_idx);

      if (dev == nullptr) {
        VCL_release_platform(plat);
        ok = false;
        break;
      }

      ok = kernel->set_gpu_resource(plat, dev);

      VCL_release_device(dev);
      VCL_release_platform(plat);
      break;
    }
    if (!ok || !kernel->have_gpu_resource()) {
      cout << "Failed to set gpu resource for kernel. Platform and device may "
              "be invalid."
           << endl;
      return __LINE__;
    }

    if (input.show_gpu) {
      kernel->show_gpu_name();
    }
  }

  omp_set_num_threads(input.num_threads);

  kernel->set_ui(nullptr, cb_progress_range_set, cb_progress_add);

  {
    wt = omp_get_wtime();
    const int ret = set_resource(kernel, input);
    if (ret != 0) {
      return ret;
    }
    wt = omp_get_wtime() - wt;

    if (input.benchmark) {
      fmt::print(
          "Parsing resource pack and block state list in {} miliseconds.\n",
          wt * 1000);
    }
  }

  {
    const int ret = set_allowed(VCL_get_block_state_list(), input);
    if (ret != 0) {
      return ret;
    }
  }

  if (input.show_color_num) {
    fmt::println("{} colors avaliable.", VCL_get_allowed_colors(nullptr, 0));
  }

  if (input.export_test_lite) {
    std::string filename = fmt::format("{}test_all_blocks_mc={}.litematic",
                                       input.prefix, int(input.version));
    wt = omp_get_wtime();

    if (!VCL_export_test_litematic(filename.c_str())) {
      fmt::print("Failed to export test litematic \"{}\"\n", filename);
      return __LINE__;
    }

    wt = omp_get_wtime() - wt;

    if (input.benchmark) {
      fmt::print("Exported \"{}\" in {} seconds.\n", filename, wt);
    }
  }

  for (const auto &img_filename : input.images) {
    const std::string pure_filename_no_extension =
        std::filesystem::path(img_filename)
            .filename()
            .replace_extension("")
            .string();
    if (!input.need_to_read()) {
      continue;
    }

    QImage img(QString::fromLocal8Bit(img_filename.c_str()));

    if (img.isNull()) {
      fmt::print("Failed to open image {}\n", img_filename);
      VCL_destroy_kernel(kernel);
      return __LINE__;
    }

    img = img.convertToFormat(QImage::Format::Format_ARGB32);

    if (img.isNull()) {
      return __LINE__;
    }
    if (!kernel->set_image(img.height(), img.width(),
                           (const uint32_t *)img.scanLine(0), true)) {
      cout << "Failed to set raw image to kernel." << endl;
      return __LINE__;
    }

    if (!input.need_to_convert()) {
      continue;
    }

    wt = omp_get_wtime();
    if (!kernel->convert(input.algo, input.dither)) {
      cout << "Failed to convert image." << endl;
      return __LINE__;
    }
    wt = omp_get_wtime() - wt;

    if (input.benchmark) {
      fmt::print("Converted {} pixels in {} seconds.\n",
                 img.height() * img.width(), wt);
    }

    if (input.make_converted_image) {
      std::string dst_name_str(input.prefix);
      dst_name_str += pure_filename_no_extension + "_converted.png";

      memset(img.scanLine(0), 0, img.height() * img.width() * sizeof(uint32_t));

      kernel->converted_image((uint32_t *)img.scanLine(0), nullptr, nullptr,
                              true);

      const bool ok = img.save(QString::fromLocal8Bit(dst_name_str.c_str()));

      if (!ok) {
        fmt::print("Failed to save image {}\n", dst_name_str);
        return __LINE__;
      }
      // cout << dst_path << endl;
    }

    if (input.make_flat_diagram) {
      double wtime[3];
      for (uint8_t layer = 0; layer < input.layers; layer++) {
        std::string dst_name_str(input.prefix);
        dst_name_str += pure_filename_no_extension;
        dst_name_str += "_flagdiagram_layer=";
        dst_name_str += std::to_string(layer);
        dst_name_str += ".png";

        VCL_Kernel::flag_diagram_option option;
        option.row_start = 0;
        option.row_end = kernel->rows();
        option.split_line_row_margin = input.flat_diagram_splitline_margin_row;
        option.split_line_col_margin = input.flat_diagram_splitline_margin_col;
        wtime[layer] = omp_get_wtime();
        if (!kernel->export_flag_diagram(dst_name_str.c_str(), option, layer)) {
          fmt::print("Failed to export flat diagram {}\n", dst_name_str);
          return __LINE__;
        }
        wtime[layer] = omp_get_wtime() - wtime[layer];
      }

      if (input.benchmark) {
        fmt::print("Export flatdiagram containing {} images in ", input.layers);
        for (int i = 0; i < input.layers; i++) {
          cout << wtime[i] << ", ";
        }
        cout << " seconds.\n";
      }
    }

    if (!input.need_to_build()) {
      continue;
    }

    wt = omp_get_wtime();
    if (!kernel->build()) {
      cout << "Failed to build " << endl;
      return __LINE__;
    }
    wt = omp_get_wtime() - wt;

    if (input.benchmark) {
      fmt::print("Built {} blocks in {} seconds.\n", kernel->xyz_size(), wt);
    }

    if (input.make_litematic) {
      const std::string filename =
          input.prefix + pure_filename_no_extension + ".litematic";

      wt = omp_get_wtime();
      const bool success = kernel->export_litematic(
          filename.c_str(), "Genereated by VCCL", "VCCL is part of SlopeCraft");
      wt = omp_get_wtime() - wt;

      if (!success) {
        fmt::println("Failed to export {}.", filename);
        return __LINE__;
      }

      if (input.benchmark) {
        fmt::println("Export litematic with {} blocks in {} seconds.",
                     kernel->xyz_size(), wt);
      }
    }

    if (input.make_schematic) {
      const std::string filename =
          input.prefix + pure_filename_no_extension + ".schem";

      wt = omp_get_wtime();
      const bool success = kernel->export_WESchem(
          filename.data(), {0, 0, 0}, {0, 0, 0}, "Genereated by VCCL");
      wt = omp_get_wtime() - wt;

      if (!success) {
        fmt::println("Failed to export {}.", filename);
        return __LINE__;
      }

      if (input.benchmark) {
        fmt::print("Export WE schem with {} blocks in {} seconds.\n",
                   kernel->xyz_size(), wt);
      }
    }

    if (input.make_structure) {
      const std::string filename =
          input.prefix + pure_filename_no_extension + ".nbt";

      wt = omp_get_wtime();
      const bool success = kernel->export_structure(
          filename.c_str(), input.structure_is_air_void);
      wt = omp_get_wtime() - wt;

      if (!success) {
        fmt::println("Failed to export {}.", filename);
        return __LINE__;
      }

      if (input.benchmark) {
        fmt::print(
            "Export vanilla structure file with {} blocks in {} seconds.\n",
            kernel->xyz_size(), wt);
      }
    }
  }
  VCL_destroy_kernel(kernel);
  cout << "success." << endl;

  return 0;
}