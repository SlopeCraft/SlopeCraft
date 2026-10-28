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

#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream>

#include <omp.h>

#include "DirectionHandler.hpp"
#include "TokiVC.h"
#include "VCL_internal.h"
#include "VisualCraftL.h"

#include <MCDataVersion.h>

std::vector<mc_schem::unique_block> TokiVC::palette_for_mc_schem() {
  std::vector<mc_schem::unique_block> blks;
  blks.reserve(TokiVC::blocks_allowed.size());
  for (size_t i = 0; i < TokiVC::blocks_allowed.size(); i++) {
    blks.emplace_back(nullptr);
  }
  for (const auto& [blkp, idx] : TokiVC::blocks_allowed) {
    const auto& id = blkp->id_for_schem(TokiVC::version);
    blks[idx] = mc_schem::unwrap(mc_schem::block::create(id));
  }
  return blks;
}

void TokiVC::initialize_schematic_with_palette_no_lock(
    std::array<int, 3> shape_xyz) {
  this->schem = mc_schem::schematic::create();
  assert(this->schem->regions_count() == 0);

  auto palette = palette_for_mc_schem();

  this->schem = mc_schem::schematic::create();
  {
    std::vector<const mc_schem::block*> pal_ptrs;
    pal_ptrs.reserve(palette.size());
    for (auto& blkp : palette) {
      assert(blkp != nullptr);
      pal_ptrs.emplace_back(blkp.get());
    }
    auto region = mc_schem::unwrap(
        mc_schem::region::create_with_palette(shape_xyz, pal_ptrs));
    this->schem->append_region(std::move(region));
  }
}

int64_t TokiVC::xyz_size(int64_t* x_dest, int64_t* y_dest,
                         int64_t* z_dest) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to get xyz size before without schem built.");
    return -1;
  }
  auto [x, y, z] = this->schem->shape();

  if (x_dest) *x_dest = x;
  if (y_dest) *y_dest = y;
  if (z_dest) *z_dest = z;
  return static_cast<int64_t>(x) * static_cast<int64_t>(y) *
         static_cast<int64_t>(z);
}

bool TokiVC::build(uint32_t num_threads) noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  try {
    if (this->_step < VCL_Kernel_step::VCL_wait_for_build) {
      VCL_report(VCL_report_type_t::error,
                 "Trying to build before without image converted.");
      return false;
    }

    dir_handler<int64_t> dirh(TokiVC::exposed_face, this->img_cvter.rows(),
                              this->img_cvter.cols(), TokiVC::max_block_layers);

    this->initialize_schematic_with_palette_no_lock(
        {static_cast<int>(dirh.range_xyz()[0]),
         static_cast<int>(dirh.range_xyz()[1]),
         static_cast<int>(dirh.range_xyz()[2])});

    const Eigen::ArrayXX<uint16_t> color_id_mat = this->img_cvter.color_id();
    {
      auto region = this->schem->get_region(0);
      assert(region);

      /// Parallel element operation is safe for dense region, but not for
      /// sparse
      if (not region->is_dense()) [[unlikely]] {
        region->convert_to_dense();
      }
      assert(region->is_dense());
      num_threads = std::max<uint32_t>(
          num_threads > 0 ? num_threads : std::thread::hardware_concurrency(),
          1);

      std::atomic_bool global_ok = true;
      std::vector<std::string> error_msg;
      error_msg.resize(num_threads, {});
#pragma omp parallel for schedule(static) num_threads(num_threads)
      for (int64_t r = 0; r < this->img_cvter.rows(); r++) {
        if (not global_ok) [[unlikely]] {
          continue;
        }

        for (int64_t c = 0; c < this->img_cvter.cols(); c++) {
          auto color_id = color_id_mat(r, c);
          if (color_id >= 0xFFFF)
            continue;  // full transparent pixels, use air instead

          const auto& variant = TokiVC::LUT_basic_color_idx_to_blocks[color_id];

          const VCL_block* const* blockpp = nullptr;
          size_t depth_current = 0;

          if (variant.index() == 0) {
            blockpp = &std::get<0>(variant);
            depth_current = 1;
          } else {
            const std::vector<const VCL_block*>& vec = std::get<1>(variant);
            blockpp = vec.data();
            depth_current = vec.size();
          }

          const bool ok = [&] {
            for (size_t depth = 0; depth < depth_current; depth++) {
              const auto coord = dirh.coordinate_of(r, c, depth);
              const auto range = region->size_xyz();

              for (size_t di = 0; di < 3; di++) {
                if (coord[di] < 0 || coord[di] >= range[di]) [[unlikely]] {
                  error_msg[omp_get_thread_num()] =
                      std::format("coordinate out of range : {}, {}, {}.\n",
                                  coord[0], coord[1], coord[2]);
                  return false;
                }
              }

              const VCL_block* const blkp = blockpp[depth];

              auto it = TokiVC::blocks_allowed.find(blkp);
              if (it == TokiVC::blocks_allowed.end()) [[unlikely]] {
                error_msg[omp_get_thread_num()] = std::format(
                    "Failed to find VCL_block at address {} named {} in "
                    "allowed blocks. This is an internal error.",
                    static_cast<const void*>(blkp),
                    blkp->full_id_ptr()->c_str());
                return false;
              }

              const uint16_t blk_id = it->second;
              try {
                region->set_block({static_cast<int32_t>(coord[0]),
                                   static_cast<int32_t>(coord[1]),
                                   static_cast<int32_t>(coord[2])},
                                  blk_id);
              } catch (const std::exception& e) {
                error_msg[omp_get_thread_num()] =
                    std::format("Failed to set block: {}", e.what());
                return false;
              }
            }
            return true;
          }();

          if (not ok) [[unlikely]] {
            global_ok = false;
            break;
          }
        }
      }

      for (auto& msg : error_msg) {
        if (msg.empty()) continue;
        // logically if found error, global status should be false. But this is
        // not very required
        assert(not global_ok);
        VCL_report(VCL_report_type_t::error, msg.c_str());
        return false;
      }
    }

    {
      auto data_version = static_cast<int32_t>(
          MCDataVersion::suggested_version(TokiVC::version));
      auto md_res = mc_schem::metadata_ir::create(data_version);
      if (not md_res) {
        auto msg =
            std::format("Failed to create metadata for data version {}: {}",
                        data_version, md_res.error()->message());
        VCL_report(VCL_report_type_t::error, msg.c_str());
        return false;
      }
      auto md = std::move(md_res.value());
      md->set_author("VisualCraftL");
      md->set_description(
          "Generated by VisualCraftL "
          "(\"https://github.com/SlopeCraft/SlopeCraft\") and mc_schem "
          "(\"https://github.com/SlopeCraft/mc_schem\"), both are developed by "
          "TokiNoBug.");
      this->schem->set_metadata(std::move(md));
    }

    this->_step = VCL_Kernel_step::VCL_built;

    return true;
  } catch (const std::exception& e) {
    const auto msg = std::format("Exception while building: {}", e.what());
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }
}

bool TokiVC::export_litematic(const char* localEncoding_filename,
                              const char* utf8_litename,
                              const char* utf8_regionname) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export litematic without built.");
    return false;
  }

  mc_schem::litematica_save_option option{};

  const auto old_region_name = this->schem->get_region(0)->name();
  this->schem->get_region(0)->set_name(utf8_regionname);

  const auto old_lite_name = this->schem->metadata()->name();
  this->schem->metadata()->set_name(utf8_litename);

  auto restore_metadata = [&] {
    this->schem->get_region(0)->set_name(old_region_name);
    this->schem->metadata()->set_name(old_lite_name);
  };

  std::ofstream ofs{localEncoding_filename, std::ios::binary};
  auto result = this->schem->save_litematica(ofs, option);
  if (not result) {
    const auto msg =
        std::format("VisualCraftL failed to export a litematic. Detail: {}",
                    std::move(result.error())->message());
    VCL_report(VCL_report_type_t::error, msg.c_str());
    restore_metadata();
    return false;
  }
  restore_metadata();
  return true;
}

bool TokiVC::export_structure(const char* localEncoding_TargetName,
                              bool is_air_structure_void) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export structure without built.");
    return false;
  }
  mc_schem::vanilla_structure_save_option option{
    .keep_air = not is_air_structure_void,
  };
  std::ofstream ofs{localEncoding_TargetName, std::ios::binary};
  auto result = this->schem->save_vanilla_structure(ofs, option);
  if (not result) {
    const auto msg = std::format(
        "VisualCraftL failed to export a vanilla structure. Detail: {}",
        result.error()->message());
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }
  return true;
}

// schematic format seems not have required mods. So these are suspended
bool TokiVC::export_WESchem(const char* localEncoding_fileName,
                            const int (&offset)[3], const int (&weOffset)[3],
                            const char* utf8_Name,
                            const char* const* const utf8_requiredMods
                            [[maybe_unused]],
                            const int requiredModsCount
                            [[maybe_unused]]) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export structure without built.");
    return false;
  }
  mc_schem::world_edit13_save_option option{
    .background_block = mc_schem::common_block::air,
  };

  const auto old_name = this->schem->metadata()->name();
  this->schem->metadata()->set_name(utf8_Name);
  const auto old_offset = this->schem->metadata()->schem_offset();
  this->schem->metadata()->set_schem_offset(offset);
  const auto old_we_offset = this->schem->metadata()->schem_we_offset();
  this->schem->metadata()->set_schem_we_offset(weOffset);
  auto restore_metadata = [&] {
    auto md = this->schem->metadata();
    md->set_name(old_name);
    md->set_schem_offset(old_offset);
    if (old_we_offset) {
      md->set_schem_we_offset(old_we_offset.value());
    } else {
      md->set_schem_we_offset();
    }
  };

  std::ofstream ofs{localEncoding_fileName, std::ios::binary};
  auto result = this->schem->save_world_edit13(ofs, option);
  if (not result) {
    const auto msg = std::format(
        "VisualCraftL failed to export a WorldEdit schem. Detail: {}",
        result.error()->message());
    VCL_report(VCL_report_type_t::error, msg.c_str());
    restore_metadata();
    return false;
  }
  restore_metadata();
  return true;
}