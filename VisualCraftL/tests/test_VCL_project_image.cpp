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
#include <Eigen/Dense>
#include <VisualCraftL.h>
#include <filesystem>
#include <iostream>
#include <png.h>
#include <unordered_map>

using std::cout, std::endl;

bool rewrite_png(const char *const filename,
                 const Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic,
                                    Eigen::RowMajor> &img) noexcept;

int main(int argc, char **argv) {
  CLI::App app;

  std::vector<std::string> input_files;
  std::vector<const char *> zip_files, json_files;
  SCL_gameVersion version;
  VCL_face_t face;
  std::string __face;

  std::vector<std::string> blockid;

  std::string filename_prefix;
  bool display_bsl = false;
  bool display_model = false;

  {
    app.add_option("files", input_files, "json and resource packs.")
        ->required()
        ->check(CLI::ExistingFile);
    int __version;
    app.add_option("--version", __version, "MC version.")
        ->default_val(19)
        ->check(CLI::Range(12, int(max_version), "Avaliable versions."));
    /*
        int __layers;
        app.add_option("--layers", __layers, "Max layers")
        ->default_val(3)
        ->check(CLI::PositiveNumber);

    */

    app.add_option("--face", __face, "Facing direction.")
        ->default_val("up")
        ->check(
            CLI::IsMember({"up", "down", "north", "south", "east", "west"}));

    app.add_option("--blocks", blockid,
                   "List of block id to render project image.")
        ->expected(1, UINT32_MAX);

    app.add_option("--prefix", filename_prefix)->default_val("./");

    app.add_flag("--display-bsl", display_bsl)->default_val(false);
    app.add_flag("--display-model", display_model)->default_val(false);

    CLI11_PARSE(app, argc, argv);

    for (auto &str : input_files) {
      std::filesystem::path p(str);
      if (p.extension() == ".json") {
        json_files.emplace_back(str.c_str());
        continue;
      }

      if (p.extension() == ".zip") {
        zip_files.emplace_back(str.c_str());
        continue;
      }
    }
    bool ok = true;
    face = VCL_str_to_face_t(__face.data(), &ok);
    if (!ok) {
      cout << "Invalid value for face : " << __face << endl;
      return 1;
    }
    version = SCL_gameVersion(__version);
  }

  VCL_block_state_list *bsl =
      VCL_create_block_state_list(json_files.size(), json_files.data());

  if (bsl == nullptr) {
    cout << "Failed to create bsl" << endl;
    return 2;
  }

  VCL_resource_pack *rp =
      VCL_create_resource_pack(zip_files.size(), zip_files.data());

  if (rp == nullptr) {
    VCL_destroy_block_state_list(bsl);
    cout << "Failed to create resource pack" << endl;
    return 3;
  }
  std::vector<VCL_block *> blocks_to_render;
  {
    std::unordered_map<std::string_view, VCL_block *> block_umap;
    const size_t num_blocks = VCL_get_blocks_from_block_state_list_match(
        bsl, version, face, nullptr, 0);
    block_umap.reserve(num_blocks);

    std::vector<VCL_block *> buffer;
    buffer.resize(num_blocks);

    const size_t num_blocks_2 = VCL_get_blocks_from_block_state_list_match(
        bsl, version, face, buffer.data(), buffer.capacity());

    if (num_blocks_2 != num_blocks) {
      cout << "Logically impossible error : num_blocks_2 != num_blocks" << endl;
      return 4;
    }

    for (VCL_block *bp : buffer) {
      block_umap.emplace(VCL_get_block_id(bp), bp);
    }

    if (display_bsl) {
      cout << num_blocks << " blocks are matched in block state list." << endl;
      size_t i = 0;

      for (const auto &pair : block_umap) {
        cout << "  " << i++ << " : " << pair.first << '\n';
      }
    }

    if (blockid.size() > 0) {
      for (const auto &id_input : blockid) {
        auto it = block_umap.find(id_input);

        if (it == block_umap.end()) {
          cout << "Block named " << id_input
               << " not found in bsl. Check the game version." << endl;
          return 5;
        }

        blocks_to_render.emplace_back(it->second);
      }
    } else {
      for (const auto &pair : block_umap) {
        blocks_to_render.emplace_back(pair.second);
      }
    }
  }

  Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> img(
      16, 16);

  for (size_t idx = 0; idx < blocks_to_render.size(); idx++) {
    VCL_model *md =
        VCL_get_block_model(blocks_to_render[idx], rp, face, nullptr);

    if (md == nullptr) {
      cout << "Failed to find model for block "
           << VCL_get_block_id(blocks_to_render[idx]) << endl;
      return 6;
    }

    if (display_model) {
      VCL_display_model(md);
    }

    if (!VCL_compute_projection_image(md, face, nullptr, nullptr, img.data(),
                                      img.size() * sizeof(uint32_t))) {
      cout << "Failed to export projection image for block "
           << VCL_get_block_id(blocks_to_render[idx]) << endl;
      return 7;
    }
    VCL_destroy_block_model(md);
    std::string filename = filename_prefix + std::to_string(idx) + ".png";
    if (!rewrite_png(filename.data(), img)) {
      cout << "Failed to save image " << filename << " for block "
           << VCL_get_block_id(blocks_to_render[idx]) << endl;
      return 8;
    }
  }

  VCL_destroy_block_state_list(bsl);
  VCL_destroy_resource_pack(rp);

  cout << "Success" << endl;

  return 0;
}

bool rewrite_png(const char *const filename,
                 const Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic,
                                    Eigen::RowMajor> &img) noexcept {
  png_struct *png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png == NULL) {
    return false;
  }

  png_info *info = png_create_info_struct(png);

  if (info == NULL) {
    png_destroy_write_struct(&png, NULL);
    return false;
  }

  FILE *fp = NULL;
  fp = fopen(filename, "wb");

  if (fp == NULL) {
    png_destroy_write_struct(&png, &info);
    return false;
  }

  png_init_io(png, fp);

  png_set_IHDR(png, info, img.cols(), img.rows(), 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_set_bgr(png);
  png_write_info(png, info);

  for (int r = 0; r < img.rows(); r++) {
    const uint8_t *const rowptr = reinterpret_cast<const uint8_t *>(&img(r, 0));
    png_write_row(png, rowptr);
  }

  png_write_end(png, info);

  png_destroy_write_struct(&png, &info);

  fclose(fp);

  return true;
}