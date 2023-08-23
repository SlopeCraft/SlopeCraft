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

#include "DirectionHandler.hpp"
#include "TokiVC.h"
#include "VCL_internal.h"
#include "VisualCraftL.h"
#include <chrono>
#include <vector>

void TokiVC::fill_schem_blocklist_no_lock() noexcept {
  std::vector<const char *> blk_ids;
  // fill with nullptr
  blk_ids.resize(TokiVC::blocks_allowed.size());
  for (auto &p : blk_ids) {
    p = nullptr;
  }
  for (const auto &pair : TokiVC::blocks_allowed) {
    blk_ids[pair.second] = pair.first->full_id_ptr()->c_str();
  }
  this->schem.set_block_id(blk_ids.data(), blk_ids.size());
}

int64_t TokiVC::xyz_size(int64_t *x, int64_t *y, int64_t *z) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to get xyz size before without schem built.");
    return -1;
  }

  if (x != nullptr) *x = this->schem.x_range();
  if (y != nullptr) *y = this->schem.y_range();
  if (z != nullptr) *z = this->schem.z_range();
  return this->schem.size();
}

bool TokiVC::build() noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_wait_for_build) {
    VCL_report(VCL_report_type_t::error,
               "Trying to build before without image converted.");
    return false;
  }

  dir_handler<int64_t> dirh(TokiVC::exposed_face, this->img_cvter.rows(),
                            this->img_cvter.cols(), TokiVC::max_block_layers);

  this->schem.resize(dirh.range_xyz()[0], dirh.range_xyz()[1],
                     dirh.range_xyz()[2]);

  this->fill_schem_blocklist_no_lock();

  this->schem.fill(0);

  std::atomic_bool ret = true;

  const Eigen::ArrayXX<uint16_t> color_id_mat = this->img_cvter.color_id();
  // #warning resume omp here
#pragma omp parallel for schedule(static)
  for (int64_t r = 0; r < this->img_cvter.rows(); r++) {
    for (int64_t c = 0; c < this->img_cvter.cols(); c++) {
      auto color_id = color_id_mat(r, c);
      if (color_id >= 0xFFFF)
        continue;  // full transparent pixels, use air instead

      const auto &variant = TokiVC::LUT_basic_color_idx_to_blocks[color_id];

      const VCL_block *const *blockpp = nullptr;
      size_t depth_current = 0;

      if (variant.index() == 0) {
        blockpp = &std::get<0>(variant);
        depth_current = 1;
      } else {
        const std::vector<const VCL_block *> &vec = std::get<1>(variant);
        blockpp = vec.data();
        depth_current = vec.size();
      }

      for (size_t depth = 0; depth < depth_current; depth++) {
        const auto coord = dirh.coordinate_of(r, c, depth);
        const int64_t range[3] = {this->schem.x_range(), this->schem.y_range(),
                                  this->schem.z_range()};
        for (size_t di = 0; di < 3; di++) {
          if (coord[di] < 0 || coord[di] >= range[di]) {
            std::string msg =
                fmt::format("coordinate out of range : {}, {}, {}.\n", coord[0],
                            coord[1], coord[2]);
            VCL_report(VCL_report_type_t::error, msg.c_str());
          }
        }

        const VCL_block *const blkp = blockpp[depth];

        auto it = TokiVC::blocks_allowed.find(blkp);
        if (it == TokiVC::blocks_allowed.end()) {
          std::string msg = fmt::format(
              "Failed to find VCL_block at address {} named {} in "
              "allowed blocks. This is an internal error.",
              (const void *)blkp, blkp->full_id_ptr()->c_str());
          VCL_report(VCL_report_type_t::error, msg.c_str());
          ret = false;
        }

        const uint16_t blk_id = it->second;

        this->schem(coord[0], coord[1], coord[2]) = blk_id;
      }
    }
  }

  this->schem.set_MC_major_version_number(TokiVC::version);
  this->schem.set_MC_version_number(
      MCDataVersion::suggested_version(TokiVC::version));

  this->_step = VCL_Kernel_step::VCL_built;

  return ret;
}

bool TokiVC::export_litematic(const char *localEncoding_filename,
                              const char *utf8_litename,
                              const char *utf8_regionname) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export litematic without built.");
    return false;
  }

  libSchem::litematic_info info;
  info.author_utf8 = "VisualCraftL";
  info.destricption_utf8 =
      "Generated by VisualCraftL, which is developed by TokiNoBug. See "
      "\"https://github.com/ToKiNoBug/SlopeCraft\" for detailed information.";
  info.litename_utf8 = utf8_litename;
  info.regionname_utf8 = utf8_regionname;
  info.time_created = std::chrono::duration_cast<std::chrono::seconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
  info.time_modified = info.time_created;

  SCL_errorFlag flag;
  std::string detail;

  const bool ok = this->schem.export_litematic(localEncoding_filename, info,
                                               &flag, &detail);

  if (!ok) {
    std::string err = fmt::format(
        "VisualCraftL failed to export a litematic. Error "
        "number(SCSL_errorFlag) = {}, detail : {}",
        int(flag), detail);
    VCL_report(VCL_report_type_t::error, err.c_str());
    return false;
  }
  return true;
}

bool TokiVC::export_structure(const char *localEncoding_TargetName,
                              bool is_air_structure_void) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export structure without built.");
    return false;
  }

  SCL_errorFlag flag;
  std::string detail;
  const bool ok = this->schem.export_structure(
      localEncoding_TargetName, is_air_structure_void, &flag, &detail);

  if (!ok) {
    std::string err = fmt::format(
        "VisualCraftL failed to export a structure. Error "
        "number(SCSL_errorFlag) = {}, detail : {}",
        int(flag), detail);
    VCL_report(VCL_report_type_t::error, err.c_str());
    return false;
  }
  return true;
}

bool TokiVC::export_WESchem(const char *localEncoding_fileName,
                            const int (&offset)[3], const int (&weOffset)[3],
                            const char *utf8_Name,
                            const char *const *const utf8_requiredMods,
                            const int requiredModsCount) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (this->_step < VCL_Kernel_step::VCL_built) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export structure without built.");
    return false;
  }

  libSchem::WorldEditSchem_info info;

  info.schem_name_utf8 = utf8_Name;
  for (size_t d = 0; d < 3; d++) {
    info.offset[d] = offset[d];
    info.WE_offset[d] = weOffset[d];
  }

  info.required_mods_utf8.resize(requiredModsCount);
  for (int i = 0; i < requiredModsCount; i++) {
    info.required_mods_utf8[i] = utf8_requiredMods[i];
  }

  SCL_errorFlag flag;
  std::string detail;

  const bool ok =
      this->schem.export_WESchem(localEncoding_fileName, info, &flag, &detail);
  if (!ok) {
    std::string err = fmt::format(
        "VisualCraftL failed to export a WorldEdit schem. Error "
        "number(SCSL_errorFlag) = {}, detail : {}",
        int(flag), detail);
    VCL_report(VCL_report_type_t::error, err.c_str());
    return false;
  }
  return true;
}