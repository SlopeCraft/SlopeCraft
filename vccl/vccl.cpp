
#include <CLI11.hpp>
#include <thread>

#include "vccl_internal.h"
#include <QCoreApplication>

using std::cout, std::endl;

int main(int argc, char **argv) {
  inputs input;
  CLI::App app;

  const bool is_gpu_accessible = VCL_platform_num() > 0;

  if (is_gpu_accessible) {
    input.prefer_gpu = true;
  }

  app.set_version_flag("--version",
                       std::string("vccl version : ") + SC_VERSION_STR +
                           ", VisualCraftL version : " + VCL_version_string());

  app.add_option("--rp", input.zips, "Resource packs")
      ->check(CLI::ExistingFile);
  app.add_option("--bsl", input.jsons, "Block state list json file.")
      ->check(CLI::ExistingFile);
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
  app.add_option("--mcver", __version, "MC version")
      ->default_val(19)
      ->check(CLI::Range(12, 19, "Avaliable versions"));

  app.add_option("--prefix", input.prefix, "Filename prefix of generate output")
      ->default_val("");

  app.add_flag("--out-image", input.make_converted_image,
               "Generate converted image")
      ->default_val(false);

  std::string algo;
  app.add_option("--algo", algo, "Algorithm for conversion")
      ->default_val("RGB")
      ->check(CLI::IsMember(
          {"RGB", "RGB_Better", "HSV", "Lab94", "Lab00", "XYZ", "gaCvter"}))
      ->expected(1);
  app.add_flag("--dither", input.dither,
               "Use Floyd-Steinberg dithering to improve the result of image "
               "conversion")
      ->default_val(false);
  app.add_flag("--benchmark", input.benchmark, "Display the performance data.")
      ->default_val(false);

  app.add_option("-j", input.num_threads, "CPU threads used to convert images")
      ->check(CLI::PositiveNumber)
      ->default_val(std::thread::hardware_concurrency());

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
               "List all GPU platforms and devices that OpenCL has access to.")
      ->default_val(false);
  app.add_flag("--disable-config", input.disable_config,
               "Disable default option provided by vccl-config.json")
      ->default_val(false);
  app.add_flag("--list-format", input.list_supported_formats,
               "List all supported image formats.")
      ->default_val(false);

  CLI11_PARSE(app, argc, argv);

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

  if (!ok) {
    return __LINE__;
  }
  if (!input.disable_config) {
    config cfg;
    if (!load_config("vccl-config.json", cfg)) {
      cout << "Failed to load config. Skip and continue." << endl;
    } else {
      for (std::string &str : cfg.default_jsons) {
        input.jsons.emplace_back(std::move(str));
      }
      if (input.version == SCL_gameVersion::MC12) {
        for (std::string &str : cfg.default_zips_12) {
          input.zips.emplace_back(std::move(str));
        }
      } else {
        for (std::string &str : cfg.default_zips_latest) {
          input.zips.emplace_back(std::move(str));
        }
      }

      cout << "Default config loaded." << endl;
    }
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
