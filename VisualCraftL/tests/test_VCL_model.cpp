#include <CLI11.hpp>
#include <Eigen/Dense>
#include <VisualCraftL.h>
#include <filesystem>
#include <iostream>
#include <unordered_map>

using std::cout, std::endl;

int main(int argc, char **argv) {

  CLI::App app;

  std::vector<std::string> input_files;
  std::vector<const char *> zip_files;
  // SCL_gameVersion version;
  //  VCL_face_t face;
  //  std::string __face;

  std::vector<std::string> model_names;

  // std::string filename_prefix;
  //  bool display_bsl = false;
  bool display_model = false;

  {
    app.add_option("resource pack", input_files, "Resource packs.")
        ->required()
        ->check(CLI::ExistingFile);
    /*
int __version;
app.add_option("--version", __version, "MC version.")
    ->default_val(19)
    ->check(CLI::Range(12, 19, "Avaliable versions."));

    */
    /*
        int __layers;
        app.add_option("--layers", __layers, "Max layers")
        ->default_val(3)
        ->check(CLI::PositiveNumber);

    */
    /*
    app.add_option("--face", __face, "Facing direction.")
        ->default_val("up")
        ->check(
            CLI::IsMember({"up", "down", "north", "south", "east", "west"}));
    app.add_option("--prefix", filename_prefix)->default_val("./");

    */

    app.add_option("--models", model_names, "List of model name to display.")
        ->expected(1, UINT32_MAX);

    // app.add_flag("--display-bsl", display_bsl)->default_val(false);
    app.add_flag("--display-model", display_model)->default_val(false);

    CLI11_PARSE(app, argc, argv);

    for (auto &str : input_files) {
      std::filesystem::path p(str);
      if (p.extension() == ".zip") {
        zip_files.emplace_back(str.c_str());
        continue;
      }
    }
    /*
    bool ok = true;
    face = VCL_str_to_face_t(__face.data(), &ok);
    if (!ok) {
      cout << "Invalid value for face : " << __face << endl;
      return 1;
    }
    version = SCL_gameVersion(__version);
    */
  }

  VCL_resource_pack *rp =
      VCL_create_resource_pack(zip_files.size(), zip_files.data());

  if (rp == nullptr) {
    cout << "Failed to create resource pack" << endl;
    return 3;
  }

  for (const auto &md_name : model_names) {
    VCL_model *mdp = VCL_get_block_model_by_name(rp, md_name.c_str());

    if (mdp == nullptr) {
      cout << "Model named \"" << md_name << "\" not found. Skip." << endl;
      continue;
    }

    VCL_display_model(mdp);

    VCL_destroy_block_model(mdp);
  }

  VCL_destroy_resource_pack(rp);

  cout << "Success" << endl;

  return 0;
}
