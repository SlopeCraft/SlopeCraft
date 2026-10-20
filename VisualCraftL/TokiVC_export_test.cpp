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

#include <fstream>
#include <map>

#include <magic_enum/magic_enum.hpp>
#include "TokiVC.h"
#include "VCL_internal.h"
#include "VisualCraftL.h"

#include <MCDataVersion.h>

const VCL_block* find_first_mark_block(
    const std::unordered_map<const VCL_block*, uint16_t>&
        blocks_allowed) noexcept {
  const VCL_block* p = nullptr;
  for (const auto& pair : blocks_allowed) {
    if (pair.first->is_air()) {
      continue;
    }
    if (pair.first->is_disabled()) {
      continue;
    }

    if (VCL_compare_block(p, pair.first)) {
      p = pair.first;
    }
  }
  return p;
}

bool TokiVC::export_test_litematic_no_lock(const char* filename) noexcept {
  if (!TokiVC_internal::is_basic_color_set_ready ||
      !TokiVC_internal::is_allowed_color_set_ready) {
    VCL_report(
        VCL_report_type_t::error,
        "Trying to export testing litematic before allowed blocks are set.");
    return false;
  }

  auto region = [&] -> mc_schem::unique_region {
    const auto palette = palette_for_mc_schem();
    std::vector<const mc_schem::block*> pal_view;
    pal_view.reserve(palette.size());
    for (auto& blk : palette) {
      assert(blk.get());
      pal_view.emplace_back(blk.get());
    }
    auto region_res =
        mc_schem::region::create_with_palette({1, 1, 1}, pal_view);
    if (not region_res) {
      mc_schem::unique_error err{std::move(region_res.error())};
      const auto msg = std::format("Failed to create region with palette:  {}",
                                   err->message());
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return nullptr;
    }
    return std::move(*region_res);
  }();
  if (region == nullptr) {
    return false;
  }

  // divide blocks by class
  std::map<
      VCL_block_class_t,
      std::vector<typename decltype(TokiVC::blocks_allowed)::const_iterator>>
      blk_class;

  for (auto cls : magic_enum::enum_values<VCL_block_class_t>()) {
    blk_class[cls] = {};
    blk_class[cls].reserve(TokiVC::blocks_allowed.size() /
                           magic_enum::enum_values<VCL_block_class_t>().size());
  }

  for (auto it = TokiVC::blocks_allowed.begin();
       it != TokiVC::blocks_allowed.end(); ++it) {
    blk_class.at(it->first->block_class).emplace_back(it);
  }

  size_t max_cols = 0;

  for (auto& pair : blk_class) {
    std::sort(pair.second.begin(), pair.second.end(),
              [](typename decltype(TokiVC::blocks_allowed)::const_iterator A,
                 typename decltype(TokiVC::blocks_allowed)::const_iterator B)
                  -> bool { return A->second < B->second; });
    max_cols = std::max(max_cols, pair.second.size());
  }
  max_cols++;

  auto block_class_arr = magic_enum::enum_values<VCL_block_class_t>();
  std::sort(block_class_arr.begin(), block_class_arr.end());

  const size_t rows = block_class_arr.size();

  const int32_t x_range = max_cols;
  const int32_t z_range = rows;
  const int32_t y_range = 2;

  region->reshape({x_range, y_range, z_range});

  const VCL_block* const marker = find_first_mark_block(TokiVC::blocks_allowed);

  for (size_t idx_class = 0; idx_class < block_class_arr.size(); idx_class++) {
    const auto& vec = blk_class.at(block_class_arr[idx_class]);
    const int32_t z_pos = idx_class;
    size_t idx = 0;
    for (; idx < vec.size(); idx++) {
      const int32_t x_pos = idx;

      region->set_block({x_pos, 0, z_pos}, vec[idx]->second);
    }

    if (marker != nullptr) {
      region->set_block({static_cast<int32_t>(vec.size()), 1, z_pos},
                        TokiVC::blocks_allowed.at(marker));
    }
  }
  // setup metadata
  const auto lite_name = std::format("Testing litematic for 1.{}",
                                     static_cast<int>(TokiVC::version));
  region->set_name(lite_name);
  auto schem = mc_schem::schematic::create();
  {
    const auto data_version =
        static_cast<int32_t>(MCDataVersion::suggested_version(TokiVC::version));
    auto md = mc_schem::metadata_ir::create(data_version);
    if (not md) {
      VCL_report(
          VCL_report_type_t::error,
          std::format("mc_schem_doesn't support data version {}", data_version)
              .c_str());
      return false;
    }

    (*md)->set_name(lite_name);
    (*md)->set_author("VisualCraftL");
    (*md)->set_description("This litematic is generated by VisualCraft.");
    schem->set_metadata(std::move(*md));
  }

  schem->append_region(std::move(region));

  mc_schem::litematica_save_option option;
  std::ofstream ofs{filename, std::ios::binary};
  auto res = schem->save_litematica(ofs, option);
  if (not res) {
    auto msg = std::format("Failed to export {} because {}", filename,
                           res.error()->message());
    VCL_report(VCL_report_type_t::warning, msg.c_str(), true);
    return false;
  }
  return true;
}
