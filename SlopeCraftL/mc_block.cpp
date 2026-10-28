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

#include "mc_block.h"
#include "blocklist.h"
#include "SCL_translator.h"

#include <magic_enum/magic_enum.hpp>

mc_block::mc_block() { image.resize(16, 16); }

std::optional<block_detail_info> mc_block::detail_info(
    SCL_gameVersion current_version) const {
  auto blk = this->to_mc_schem_block(current_version);

  if (not blk) {
    return std::nullopt;
  }
  return block_detail_info{.id_namespace{blk->namespace_()},
                           .pure_id{blk->id()}};
}

void mc_block_interface::clear() noexcept {
  setBurnable(false);
  setDoGlow(false);
  setEndermanPickable(false);
  setId("minecraft:air");
  setIdOld("");
  setNeedGlass(false);
  setVersion(SCL_gameVersion::ANCIENT);
  setNameZH("");
  setNameEN("");
  for (auto ver : magic_enum::enum_values<SCL_gameVersion>()) {
    if (ver >= SCL_gameVersion::MIN_VALID and
        ver <= SCL_gameVersion::MAX_VALID) {
      this->setNeedStone(ver, false);
    }
  }
}

size_t block_list::get_blocks(mc_block_interface** dst, uint8_t* dst_basecolor,
                              size_t capacity_in_elements) noexcept {
  if (capacity_in_elements <= 0) {
    return 0;
  }
  if (dst == nullptr and dst_basecolor == nullptr) {
    return 0;
  }

  size_t counter = 0;
  for (auto& ptr : this->m_blocks) {
    if (dst) dst[counter] = ptr.first.get();

    if (dst_basecolor) dst_basecolor[counter] = ptr.second;

    counter++;
    if (counter >= capacity_in_elements) {
      return counter;
    }
  }
  return counter;
}

size_t block_list::get_blocks(const mc_block_interface** dst,
                              uint8_t* dst_basecolor,
                              size_t capacity_in_elements) const noexcept {
  if (capacity_in_elements <= 0) {
    return 0;
  }
  if (dst == nullptr and dst_basecolor == nullptr) {
    return 0;
  }

  size_t counter = 0;
  for (auto& ptr : this->m_blocks) {
    if (dst) dst[counter] = ptr.first.get();
    if (dst_basecolor) dst_basecolor[counter] = ptr.second;
    counter++;
    if (counter >= capacity_in_elements) {
      return counter;
    }
  }
  return counter;
}

void block_list::clear() noexcept { this->m_blocks.clear(); }

block_list::~block_list() { this->clear(); }

mc_schem::unique_block mc_block::to_mc_schem_block(SCL_gameVersion ver) const {
  auto full_id = this->idForVersion(ver);
  auto blk_res = mc_schem::block::create(full_id);
  if (not blk_res) {
    const auto msg =
        SCLTranslator::tr(
            "内部错误：方块id\"%1\"通过了检查，但是再次检查出错：%2")
            .arg(full_id)
            .arg(magic_enum::enum_name(blk_res.error()).data());
    throw std::runtime_error{msg.toStdString().c_str()};
  }

  return std::move(blk_res.value());
}