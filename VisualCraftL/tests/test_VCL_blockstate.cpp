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
      ->check(CLI::Range(12, 19, "Avaliable versions."));
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

    if constexpr (set_resource_by_move) {
      if (!VCL_set_resource_move(
              &rp, &bsl,
              VCL_set_resource_option{version, max_block_layers, face})) {
        cout << "Failed to set resource pack" << endl;
        VCL_destroy_block_state_list(bsl);
        VCL_destroy_resource_pack(rp);
        VCL_destroy_kernel(kernel);
        return 1;
      }
    } else {

      if (!VCL_set_resource_copy(
              rp, bsl,
              VCL_set_resource_option{version, max_block_layers, face})) {
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