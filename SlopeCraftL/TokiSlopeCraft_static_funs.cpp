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

#include "TokiSlopeCraft.h"
#include <SC_version_buildtime.h>

const char *TokiSlopeCraft::getSCLVersion() { return SC_VERSION_STR; }

void TokiSlopeCraft::getColorMapPtrs(const float **const r,
                                     const float **const g,
                                     const float **const b,
                                     const unsigned char **m, int *rows) {
  if (r != nullptr) *r = TokiSlopeCraft::Allowed.rgb_data(0);
  if (g != nullptr) *g = TokiSlopeCraft::Allowed.rgb_data(1);
  if (b != nullptr) *b = TokiSlopeCraft::Allowed.rgb_data(2);

  if (m != nullptr) *m = TokiSlopeCraft::Allowed.map_data();
  if (rows != nullptr) *rows = TokiSlopeCraft::Allowed.color_count();
}

const float *TokiSlopeCraft::getBasicColorMapPtrs() {
  return TokiSlopeCraft::Basic.RGB_mat().data();
}

uint64_t TokiSlopeCraft::mcVersion2VersionNumber(SCL_gameVersion g) {
  switch (g) {
    case SCL_gameVersion::ANCIENT:
      return 114514;
    case SCL_gameVersion::MC12:
      return 1631;
    case SCL_gameVersion::MC13:
      return 1976;
    case SCL_gameVersion::MC14:
      return 2230;
    case SCL_gameVersion::MC15:
      return 2230;
    case SCL_gameVersion::MC16:
      return 2586;
    case SCL_gameVersion::MC17:
      return 2730;
    case SCL_gameVersion::MC18:
      return 2865;
    case SCL_gameVersion::MC19:
      return 3105;  // 1.19.0
    default:
      return 1919810;
  }
}

#include <ProcessBlockId/process_block_id.h>
#include <utilities/Schem/mushroom.h>

const simpleBlock *TokiSlopeCraft::find_block_for_idx(
    int idx, std::string_view blkid) noexcept {
  if (idx < 0) {
    return nullptr;
  }

  if (idx < (int)TokiSlopeCraft::blockPalette.size()) {
    return &TokiSlopeCraft::blockPalette[idx];
  }

  // the block must be mushroom
  namespace lsi = libSchem::internal;
  using lsi::mushroom_type;

  blkid::char_range pure_id_range;
  // invalid block id
  if (!blkid::process_blk_id(blkid, nullptr, &pure_id_range, nullptr)) {
    return nullptr;
  }

  std::string_view pure_id{pure_id_range.begin(), pure_id_range.end()};

  auto mush_type_opt = lsi::pureid_to_type(pure_id);
  if (!mush_type_opt.has_value()) {
    return nullptr;
  }

  uint8_t expected_basecolor = 0;
  switch (mush_type_opt.value()) {
    case mushroom_type::red:
      expected_basecolor = 28;
      break;
    case mushroom_type::brown:
      expected_basecolor = 10;
      break;
    case mushroom_type::stem:
      expected_basecolor = 3;
      break;
  }

  const simpleBlock *blkp = find_block_for_idx(expected_basecolor, {});

  if (blkp == nullptr) {
    return nullptr;
  }

  if (lsi::pureid_to_type(blkp->id) != mush_type_opt) {
    return nullptr;
  }

  return blkp;
}