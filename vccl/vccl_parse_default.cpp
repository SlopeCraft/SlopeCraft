#include "vccl_internal.h"
#include <QImage>
#include <filesystem>
#include <fstream>
#include <json.hpp>
#include <omp.h>

void cb_progress_range_set(void *, int, int, int) {}
void cb_progress_add(void *, int) {}

using std::cout, std::endl;
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
  VCL_set_resource_option option;
  option.version = input.version;
  option.max_block_layers = input.layers;
  option.exposed_face = input.face;

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

bool load_config(std::string_view filename, config &cfg) noexcept {

  using njson = nlohmann::json;
  njson jo;
  try {
    std::ifstream ofs(filename.data());
    if (!ofs) {
      cout << "Failed to open default config file " << filename << endl;
      return false;
    }

    ofs >> jo;
    {
      njson &rp12 = jo.at("default_resource_pack_12");
      cfg.default_zips_12.clear();
      for (size_t i = 0; i < rp12.size(); i++) {
        cfg.default_zips_12.emplace_back(rp12[i].get<std::string>());
      }
    }
    {
      njson &rp_latest = jo.at("default_resource_pack_latest");
      cfg.default_zips_latest.clear();
      for (size_t i = 0; i < rp_latest.size(); i++) {
        cfg.default_zips_latest.emplace_back(rp_latest[i].get<std::string>());
      }
    }
    {
      njson &bsl = jo.at("default_block_state_list");
      cfg.default_jsons.clear();
      for (size_t i = 0; i < bsl.size(); i++) {
        cfg.default_jsons.emplace_back(bsl[i].get<std::string>());
      }
    }

  } catch (std::exception &e) {
    cout << "Failed to parse default config file(\"" << filename
         << "\'), detail : " << e.what() << endl;
    return false;
  }
  return true;
}

int run(const inputs &input) noexcept {

  if (input.zips.size() <= 0 || input.jsons.size() <= 0) {
    cout << "No zips or jsons provided. exit." << endl;
    return __LINE__;
  }

  VCL_Kernel *kernel = VCL_create_kernel();
  if (kernel == nullptr) {
    cout << "Failed to create kernel." << endl;
    return __LINE__;
  }

  cout << "algo = " << (char)input.algo << endl;

  kernel->set_prefer_gpu(input.prefer_gpu);
  if (input.prefer_gpu) {
    const bool ok =
        kernel->set_gpu_resource(input.platform_idx, input.device_idx);
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

    double wt = omp_get_wtime();

    if (!kernel->convert(input.algo, input.dither)) {
      cout << "Failed to convert" << endl;
      return __LINE__;
    }
    wt = omp_get_wtime() - wt;

    if (input.benchmark) {
      cout << "Converting " << img.height() * img.width() << " pixels in " << wt
           << " seconds." << endl;
    }

    if (input.make_converted_image) {
      std::string dst_name_str(input.prefix);
      dst_name_str += std::filesystem::path(img_filename).filename().string();

      memset(img.scanLine(0), 0, img.height() * img.width() * sizeof(uint32_t));

      kernel->converted_image((uint32_t *)img.scanLine(0), nullptr, nullptr,
                              true);

      const bool ok = img.save(QString::fromLocal8Bit(dst_name_str.c_str()));

      if (!ok) {
        cout << "Failed to save image " << dst_name_str << endl;
        return __LINE__;
      }
      // cout << dst_path << endl;
    }
  }

  VCL_destroy_kernel(kernel);

  cout << "success." << endl;

  return 0;
}
