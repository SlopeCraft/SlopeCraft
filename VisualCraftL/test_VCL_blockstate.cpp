#include "VisualCraftL.h"

#include <filesystem>
#include <iostream>
#include <vector>

using std::cout, std::endl;

int main(int argc, char **argv) {

  VCL_Kernel *kernel = VCL_create_kernel();

  constexpr bool set_resource_by_move = false;

  if (kernel == nullptr) {
    cout << "Failed to create kernel." << endl;
    return 1;
  }

  {

    std::vector<const char *> zip_filenames, json_filenames;

    for (int argidx = 1; argidx < argc; argidx++) {
      std::filesystem::path p(argv[argidx]);

      if (p.extension() == ".zip") {
        zip_filenames.emplace_back(argv[argidx]);
      }

      if (p.extension() == ".json") {
        json_filenames.emplace_back(argv[argidx]);
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

    constexpr int max_block_layers = 3;

    if constexpr (set_resource_by_move) {
      if (!VCL_set_resource_and_version_move(&rp, &bsl, SCL_gameVersion::MC19,
                                             VCL_face_t::face_up,
                                             max_block_layers)) {
        cout << "Failed to set resource pack" << endl;
        VCL_destroy_block_state_list(bsl);
        VCL_destroy_resource_pack(rp);
        VCL_destroy_kernel(kernel);
        return 1;
      }
    } else {

      if (!VCL_set_resource_and_version_copy(rp, bsl, SCL_gameVersion::MC19,
                                             VCL_face_t::face_up,
                                             max_block_layers)) {
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

  VCL_destroy_kernel(kernel);

  cout << "Success." << endl;

  return 0;
}