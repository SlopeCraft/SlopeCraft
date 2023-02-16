#include <CLI11.hpp>
#include <QImage>
#include <VisualCraftL.h>
#include <iostream>
#include <omp.h>
#include <string>
#include <thread>
#include <vector>

using std::cout, std::endl;

struct inputs {
  std::vector<std::string> zips;
  std::vector<std::string> jsons;
  std::vector<std::string> images;
  std::string prefix;
  int layers;
  int num_threads;
  SCL_gameVersion version;
  VCL_face_t face;
  bool make_converted_image{false};
  SCL_convertAlgo algo;
  bool dither{false};
};

int run(const inputs &input) noexcept;
int set_resource(VCL_Kernel *kernel, const inputs &input) noexcept;
int set_allowed(VCL_block_state_list *bsl, const inputs &input) noexcept;

SCL_convertAlgo str_to_algo(std::string_view str, bool &ok) noexcept;

void cb_progress_range_set(void *, int, int, int) {}
void cb_progress_add(void *, int) {}

int main(int argc, char **argv) {
  inputs input;
  CLI::App app;
  app.add_option("--rp", input.zips, "Resource packs")
      ->check(CLI::ExistingFile)
      ->required();
  app.add_option("--bsl", input.jsons, "Block state list json file.")
      ->check(CLI::ExistingFile)
      ->required();
  app.add_option("--img", input.images, "Images to convert")
      ->check(CLI::ExistingFile);

  app.add_option("--layers", input.layers, "Max layers")
      ->default_val(3)
      ->check(CLI::PositiveNumber);

  std::string __face;
  app.add_option("--face", __face, "Facing direction.")
      ->default_val("up")
      ->check(CLI::IsMember({"up", "down", "north", "south", "east", "west"}));

  int __version;
  app.add_option("--version", __version, "MC version.")
      ->default_val(19)
      ->check(CLI::Range(12, 19, "Avaliable versions."));

  app.add_option("--prefix", input.prefix, "Prefix to generate output")
      ->check(CLI::ExistingDirectory)
      ->required();

  app.add_flag("--out-image", input.make_converted_image,
               "Generate converted image")
      ->default_val(false);

  std::string algo;
  app.add_option("--algo", algo, "Algo for conversion")
      ->default_val("RGB_Better")
      ->check(CLI::IsMember(
          {"RGB", "RGB_Better", "HSV", "Lab94", "Lab00", "XYZ", "gaCvter"}))
      ->expected(1);
  app.add_flag("--dither", input.dither)->default_val(false);

  app.add_option("-j", input.num_threads)
      ->check(CLI::PositiveNumber)
      ->default_val(std::thread::hardware_concurrency());

  CLI11_PARSE(app, argc, argv);

  input.version = SCL_gameVersion(__version);

  bool ok = true;

  input.face = VCL_str_to_face_t(__face.c_str(), &ok);
  if (!ok) {
    return __LINE__;
  }

  input.algo = str_to_algo(algo, ok);

  if (!ok) {
    return __LINE__;
  }

  return run(input);
}

#define VCCL_PRIVATE_MACRO_MAKE_CASE(enum_val)                                 \
  if (str == #enum_val) {                                                      \
    ok = true;                                                                 \
    return SCL_convertAlgo::enum_val;                                          \
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

  if (!VCL_set_resource_move(
          &rp, &bsl,
          VCL_set_resource_option{input.version, input.layers, input.face})) {
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

  if (blocks.size() != num) {
    return __LINE__;
  }

  if (!VCL_set_allowed_blocks(blocks.data(), blocks.size())) {
    cout << "Failed to set allowed blocks." << endl;
    return __LINE__;
  }

  return 0;
}

int run(const inputs &input) noexcept {
  VCL_Kernel *kernel = VCL_create_kernel();
  if (kernel == nullptr) {
    cout << "Failed to create kernel." << endl;
    return __LINE__;
  }

  cout << "algo = " << input.algo << endl;

  omp_set_num_teams(input.num_threads);

  kernel->set_ui(nullptr, cb_progress_range_set, cb_progress_add);

  {
    const int ret = set_resource(kernel, input);
    if (ret != 0) {
      return ret;
    }
  }

  {
    const int ret = set_allowed(VCL_get_block_state_list(), input);
    if (ret != 0) {
      return ret;
    }
  }

  for (const auto &img_filename : input.images) {
    QImage img(img_filename.c_str());

    if (img.isNull()) {
      cout << "Failed to open image " << img_filename << endl;
      VCL_destroy_kernel(kernel);
      return __LINE__;
    }

    img = img.convertToFormat(QImage::Format::Format_ARGB32);

    if (img.isNull()) {
      return __LINE__;
    }
    if (!kernel->set_image(img.height(), img.width(),
                           (const uint32_t *)img.scanLine(0), true)) {
      cout << "Failed to set raw image." << endl;
      return __LINE__;
    }

    if (!kernel->convert(input.algo, input.dither)) {
      cout << "Failed to convert" << endl;
      return __LINE__;
    }
  }

  VCL_destroy_kernel(kernel);
  return 0;
}