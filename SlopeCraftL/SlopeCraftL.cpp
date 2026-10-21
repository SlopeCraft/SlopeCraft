/*
 Copyright © 2021-2026  TokiNoBug
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

#include <format>
#include <json.hpp>
#include <expected>
#include <zip.h>
#include <libpng_reader.h>

#include "SlopeCraftL.h"
#include "mc_block.h"
#include "blocklist.h"
#include "string_deliver.h"
#include "color_table.h"
#include "SCL_translator.h"

using namespace SlopeCraft;

extern "C" {

SCL_EXPORT const float* SCL_get_rgb_basic_colorset_source() {
  return RGBBasicSource;
}

SCL_EXPORT mc_block_interface* SCL_create_block() { return new mc_block; }
SCL_EXPORT void SCL_destroy_block(mc_block_interface* b) { delete b; }

SCL_EXPORT block_list_interface* SCL_create_block_list(
    const char* zip_filename, const block_list_create_info& option) {
  auto [res, warnings] = create_block_list_from_file(zip_filename);

  write_to_sd(option.warnings, warnings);
  if (not res) {
    write_to_sd(option.error, res.error());
    return nullptr;
  }

  auto bl = new block_list{std::move(res.value())};
  assert(bl not_eq nullptr);
  return bl;
}

SCL_EXPORT block_list_interface* SCL_create_block_list_from_buffer(
    const void* buffer, size_t buffer_bytes,
    const block_list_create_info& option) {
  if (buffer == nullptr or buffer_bytes <= 0) {
    write_to_sd(
        option.error, SCLTranslator::tr("函数SCL_create_block_list_from_"
                                        "buffer遇到无效的输入，缓冲区为空")
                          .toStdString()
        // "SCL_create_block_list_from_buffer met invalid value, either buffer
        // is nullptr or buffer size is 0"
    );
    return nullptr;
  }

  auto [res, warnings] = create_block_list_from_buffer(
      {reinterpret_cast<const uint8_t*>(buffer), buffer_bytes});
  SlopeCraft::write_to_sd(option.warnings, warnings);
  if (not res) {
    SlopeCraft::write_to_sd(option.error, res.error());
    return nullptr;
  }

  auto bl = new block_list{std::move(res.value())};
  assert(bl not_eq nullptr);
  return bl;
}

SCL_EXPORT void SCL_destroy_block_list(block_list_interface* bli) {
  delete bli;
}

SCL_EXPORT const char* SCL_getSCLVersion() { return SC_VERSION_STR; }

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

SCL_EXPORT SCL_gameVersion SCL_maxAvailableVersion() {
  return SCL_gameVersion::MAX_VALID;
}

SCL_EXPORT color_table* SCL_create_color_table(
    const color_table_create_info& args) {
  auto opt = color_table_impl::create(args);
  if (opt) {
    return new color_table_impl{std::move(opt.value())};
  }
  return nullptr;
}

SCL_EXPORT void SCL_destroy_color_table(color_table* c) { delete c; }

SCL_EXPORT void SCL_destroy_converted_image(converted_image* c) { delete c; }
SCL_EXPORT void SCL_destroy_structure_3D(structure_3D* s) { delete s; }

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

SCL_EXPORT void SCL_get_map_count(size_t image_rows, size_t image_cols,
                                  size_t& map_rows, size_t& map_cols) {
  map_rows = ceil(image_rows / 128.0);
  map_cols = ceil(image_cols / 128.0);
}
}

namespace SlopeCraft {
Eigen::Map<const Eigen::ArrayXf> BasicRGB4External(int channel) {
  return Eigen::Map<const Eigen::ArrayXf>(
      &SlopeCraft::basic_colorset->RGB_mat()(0, channel),
      SlopeCraft::basic_colorset->color_count());
}
}  // namespace SlopeCraft