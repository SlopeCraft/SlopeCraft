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

#include "VisualCraftL.h"

#include <filesystem>
#include <iostream>
#include <vector>

#include <CLI11.hpp>

using std::cout, std::endl;

int main(int argc, char **argv) {
  CLI::App app;
  std::vector<std::string> input_files;

  // std::vector<std::string> images;

  // std::string cvted_image_prefix;

  app.add_option("files", input_files, "json and resource packs.")
      ->required()
      ->check(CLI::ExistingFile);
  int __version;
  app.add_option("--version", __version, "MC version.")
      ->default_val(19)
      ->check(CLI::Range(12, int(max_version), "Avaliable versions."));
  int __layers;
  app.add_option("--layers", __layers, "Max layers")
      ->default_val(3)
      ->check(CLI::PositiveNumber);

  std::string __face;
  app.add_option("--face", __face, "Facing direction.")
      ->default_val("up")
      ->check(CLI::IsMember({"up", "down", "north", "south", "east", "west"}));

  // app.add_option("--image", images, "Images to
  // convert")->check(CLI::ExistingFile); app.add_option("--prefix",
  // cvted_image_prefix)->default_val("");

  CLI11_PARSE(app, argc, argv);

  const int max_block_layers = __layers;

  const auto version = SCL_gameVersion(__version);

  bool ok = true;

  const VCL_face_t face = VCL_str_to_face_t(__face.c_str(), &ok);
  if (!ok) {
    return 1;
  }

  VCL_Kernel *kernel = VCL_create_kernel();

  constexpr bool set_resource_by_move = false;

  if (kernel == nullptr) {
    cout << "Failed to create kernel." << endl;
    return 1;
  }

  {
    std::vector<const char *> zip_filenames, json_filenames;

    for (const std::string &i : input_files) {
      std::filesystem::path p(i);

      if (p.extension() == ".zip") {
        zip_filenames.emplace_back(i.c_str());
      }

      if (p.extension() == ".json") {
        json_filenames.emplace_back(i.c_str());
      }
    }

    VCL_block_state_list *bsl = VCL_create_block_state_list(
        json_filenames.size(), json_filenames.data());

    if (bsl == nullptr) {
      cout << "Failed to parse block state list." << endl;
      VCL_destroy_kernel(kernel);
      return 1;
    }

    cout << "Blockstate parsed successfully." << endl;

    // const char *zips[] = {"Vanilla_1_19_2.zip"};

    VCL_resource_pack *rp =
        VCL_create_resource_pack(zip_filenames.size(), zip_filenames.data());

    if (rp == nullptr) {
      cout << "Failed to parse resource pack(s)." << endl;
      VCL_destroy_block_state_list(bsl);
      VCL_destroy_kernel(kernel);
      return 1;
    }

    cout << "Resource pack(s) parsed successfully." << endl;

    // VCL_display_resource_pack(rp);
    // VCL_display_block_state_list(bsl);

    VCL_set_resource_option option;
    option.version = version;
    option.max_block_layers = max_block_layers;
    option.exposed_face = face;

    if constexpr (set_resource_by_move) {
      if (!VCL_set_resource_move(&rp, &bsl, option)) {
        cout << "Failed to set resource pack" << endl;
        VCL_destroy_block_state_list(bsl);
        VCL_destroy_resource_pack(rp);
        VCL_destroy_kernel(kernel);
        return 1;
      }
    } else {
      if (!VCL_set_resource_copy(rp, bsl, option)) {
        cout << "Failed to set resource pack" << endl;
        VCL_destroy_block_state_list(bsl);
        VCL_destroy_resource_pack(rp);
        VCL_destroy_kernel(kernel);
        return 1;
      }
    }

    VCL_destroy_block_state_list(bsl);
    VCL_destroy_resource_pack(rp);
  }

  std::vector<VCL_block *> blocks;
  blocks.resize(VCL_get_blocks_from_block_state_list_match(
      VCL_get_block_state_list(), version, face, nullptr, 0));

  const int block_num = VCL_get_blocks_from_block_state_list_match(
      VCL_get_block_state_list(), version, face, blocks.data(), blocks.size());

  if (block_num != int(blocks.size())) {
    cout << "Impossible error : block_num = " << block_num
         << " but blocks.size() = " << blocks.size() << endl;

    VCL_destroy_kernel(kernel);
    return 1;
  }
  if (!VCL_set_allowed_blocks(blocks.data(), blocks.size())) {
    cout << "VCL_set_allowed_blocks failed." << endl;

    VCL_destroy_kernel(kernel);
    return 1;
  }

  VCL_destroy_kernel(kernel);

  cout << "Success." << endl;

  return 0;
}