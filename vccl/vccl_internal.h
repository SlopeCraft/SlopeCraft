#ifndef SLOPECRAFT_VCCL_INTERNAL_H
#define SLOPECRAFT_VCCL_INTERNAL_H

#include <VisualCraftL.h>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

struct inputs {

  // resource
  std::vector<std::string> zips;
  std::vector<std::string> jsons;

  // colors
  SCL_gameVersion version;
  uint8_t layers;
  VCL_face_t face;
  SCL_convertAlgo algo;
  bool dither{false};
  bool show_color_num{false};
  //  images
  std::vector<std::string> images;

  // exports
  std::string prefix;
  bool make_converted_image{false};
  bool make_litematic{false};
  bool make_schematic{false};
  bool make_structure{false};
  bool structure_is_air_void{true};

  inline bool need_to_read() const noexcept { return true; }

  inline bool need_to_convert() const noexcept {
    return make_converted_image || need_to_build();
  }
  inline bool need_to_build() const noexcept {
    return make_litematic || make_schematic || make_structure;
  }
  // compute
  uint16_t num_threads;
  bool benchmark{false};

  // gpu
  bool prefer_gpu{false};
  bool show_gpu{false};
  uint8_t platform_idx{0};
  uint8_t device_idx{0};
  bool list_gpu{false};

  // others
  bool disable_config{false};
  bool list_supported_formats{false};
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