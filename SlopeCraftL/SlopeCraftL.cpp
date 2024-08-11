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

#include <fmt/format.h>
#include <json.hpp>
#include <tl/expected.hpp>
#include <zip.h>
#include <libpng_reader.h>

#include "SlopeCraftL.h"
#include "mc_block.h"
#include "blocklist.h"
#include "string_deliver.h"
#include "color_table.h"

using namespace SlopeCraft;

extern "C" {

SCL_EXPORT const float *SCL_get_rgb_basic_colorset_source() {
  return SlopeCraft::RGBBasicSource;
}

SCL_EXPORT mc_block_interface *SCL_create_block() { return new mc_block; }
SCL_EXPORT void SCL_destroy_block(mc_block_interface *b) { delete b; }

SCL_EXPORT block_list_interface *SCL_create_block_list(
    const char *zip_filename, const block_list_create_info &option) {
  auto [res, warnings] = create_block_list_from_file(zip_filename);

  SlopeCraft::write_to_sd(option.warnings, warnings);
  if (not res) {
    SlopeCraft::write_to_sd(option.error, res.error());
    return nullptr;
  }

  auto bl = new block_list{std::move(res.value())};
  assert(bl not_eq nullptr);
  return bl;
}

SCL_EXPORT block_list_interface *SCL_create_block_list_from_buffer(
    const void *buffer, size_t buffer_bytes,
    const block_list_create_info &option) {
  if (buffer == nullptr or buffer_bytes <= 0) {
    SlopeCraft::write_to_sd(
        option.error,
        "SCL_create_block_list_from_buffer met invalid value, either buffer is "
        "nullptr or buffer size is 0");
    return nullptr;
  }

  auto [res, warnings] = create_block_list_from_buffer(
      {reinterpret_cast<const uint8_t *>(buffer), buffer_bytes});
  SlopeCraft::write_to_sd(option.warnings, warnings);
  if (not res) {
    SlopeCraft::write_to_sd(option.error, res.error());
    return nullptr;
  }

  auto bl = new block_list{std::move(res.value())};
  assert(bl not_eq nullptr);
  return bl;
}

SCL_EXPORT void SCL_destroy_block_list(block_list_interface *bli) {
  delete bli;
}

SCL_EXPORT GA_converter_option *SCL_createAiCvterOpt() {
  return new GA_converter_option;
}
void SCL_EXPORT SCL_destroyAiCvterOpt(GA_converter_option *a) { delete a; }

void SCL_EXPORT SCL_setPopSize(GA_converter_option *a, unsigned int p) {
  a->popSize = p;
}
void SCL_EXPORT SCL_setMaxGeneration(GA_converter_option *a, unsigned int p) {
  a->maxGeneration = p;
}
void SCL_EXPORT SCL_setMaxFailTimes(GA_converter_option *a, unsigned int p) {
  a->maxFailTimes = p;
}
void SCL_EXPORT SCL_setCrossoverProb(GA_converter_option *a, double p) {
  a->crossoverProb = p;
}
void SCL_EXPORT SCL_setMutationProb(GA_converter_option *a, double p) {
  a->mutationProb = p;
}

unsigned int SCL_EXPORT SCL_getPopSize(const GA_converter_option *a) {
  return a->popSize;
}
unsigned int SCL_EXPORT SCL_getMaxGeneration(const GA_converter_option *a) {
  return a->maxGeneration;
}
unsigned int SCL_EXPORT SCL_getMaxFailTimes(const GA_converter_option *a) {
  return a->maxFailTimes;
}
double SCL_EXPORT SCL_getCrossoverProb(const GA_converter_option *a) {
  return a->crossoverProb;
}
double SCL_EXPORT SCL_getMutationProb(const GA_converter_option *a) {
  return a->mutationProb;
}

// SCL_EXPORT void SCL_getColorMapPtrs(const float **const rdata,
//                                     const float **const gdata,
//                                     const float **const bdata,
//                                     const uint8_t **mapdata, int *num) {
//   TokiSlopeCraft::getColorMapPtrs(rdata, gdata, bdata, mapdata, num);
// }

SCL_EXPORT const char *SCL_getSCLVersion() { return SC_VERSION_STR; }

SCL_EXPORT SCL_gameVersion SCL_basecolor_version(uint8_t basecolor) {
  if (basecolor <= 51) {
    return SCL_gameVersion::ANCIENT;
  }

  if (basecolor <= 58) {
    return SCL_gameVersion::MC16;
  }

  if (basecolor <= 61) {
    return SCL_gameVersion::MC17;
  }
  return SCL_gameVersion::FUTURE;
}

SCL_EXPORT uint8_t SCL_maxBaseColor() { return 61; }

SCL_EXPORT color_table *SCL_create_color_table(
    const color_table_create_info &args) {
  auto opt = color_table_impl::create(args);
  if (opt) {
    return new color_table_impl{std::move(opt.value())};
  }
  return nullptr;
}

SCL_EXPORT void SCL_destroy_color_table(color_table *c) { delete c; }

SCL_EXPORT void SCL_destroy_converted_image(converted_image *c) { delete c; }
SCL_EXPORT void SCL_destroy_structure_3D(structure_3D *s) { delete s; }

SCL_EXPORT void SCL_get_base_color_ARGB32(uint32_t dest[64]) {
  for (int bc = 0; bc < 64; bc++) {
    const int row = bc + 128;
    const std::array<float, 3> rgb_f32{
        SlopeCraft::basic_colorset->RGB(row, 0),
        SlopeCraft::basic_colorset->RGB(row, 1),
        SlopeCraft::basic_colorset->RGB(row, 2),
    };
    std::array<uint8_t, 3> rgb_u8;
    for (int i = 0; i < 3; ++i) {
      assert(rgb_f32[i] >= 0);
      assert(rgb_f32[i] <= 1.0);
      rgb_u8[i] = rgb_f32[i] * 255;
    }

    dest[bc] = ARGB32(rgb_u8[0], rgb_u8[1], rgb_u8[2]);
  }
}

// SCL_EXPORT int SCL_getBlockPalette(const mc_block_interface **blkpp,
//                                    size_t capacity) {
//   return TokiSlopeCraft::getBlockPalette(blkpp, capacity);
// }
}

#include <ExternalConverters/ExternalConverterStaticInterface.h>
namespace SlopeCraft {
Eigen::Map<const Eigen::ArrayXf> BasicRGB4External(int channel) {
  return Eigen::Map<const Eigen::ArrayXf>(
      &SlopeCraft::basic_colorset->RGB_mat()(0, channel),
      SlopeCraft::basic_colorset->color_count());
}
}  // namespace SlopeCraft