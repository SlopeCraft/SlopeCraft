#ifndef SLOPECRAFT_VCCL_INTERNAL_H
#define SLOPECRAFT_VCCL_INTERNAL_H

#include <VisualCraftL.h>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

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
  bool benchmark{false};
  bool prefer_gpu{false};
  bool show_gpu{false};
  uint8_t platform_idx{0};
  uint8_t device_idx{0};
  bool list_gpu{false};
  bool list_supported_formats{false};
  bool disable_config{false};
};

int run(const inputs &input) noexcept;
int set_resource(VCL_Kernel *kernel, const inputs &input) noexcept;
int set_allowed(VCL_block_state_list *bsl, const inputs &input) noexcept;

SCL_convertAlgo str_to_algo(std::string_view str, bool &ok) noexcept;

int list_gpu();

struct config {
  std::vector<std::string> default_zips_12;
  std::vector<std::string> default_zips_latest;
  std::vector<std::string> default_jsons;
};

bool load_config(std::string_view filename, config &cfg) noexcept;

void list_supported_formats() noexcept;

#endif // #ifndef SLOPECRAFT_VCCL_INTERNAL_H