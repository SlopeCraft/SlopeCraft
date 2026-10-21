//
// Created by joseph on 4/15/24.
//

#include <format>
#include <boost/uuid/detail/md5.hpp>
#include "SCLDefines.h"
#include "color_table.h"

#include "MCDataVersion.h"
#include "water_item.h"
#include "structure_3D.h"
#include "SCL_translator.h"
#include "utilities/ProcessBlockId/process_block_id.h"
#include "utilities/Schem/mushroom.h"

#include <fstream>

std::optional<color_table_impl> color_table_impl::create(
    const color_table_create_info& args) noexcept {
  color_table_impl result;
  result.mc_version_ = args.mc_version;
  result.map_type_ = args.map_type;
  {
    colorset_allowed_t a;
    a.need_find_side = (args.map_type == mapTypes::Slope);
    result.allowed = std::make_shared<colorset_allowed_t>(std::move(a));
  }

  auto report_err = [&args](errorFlag flag, std::string_view msg) {
    args.ui.report_error(flag, msg.data());
  };

  // block palette
  {
    for (size_t i = 0; i < 64; i++) {
      if (args.blocks[i] == nullptr) {
        result.blocks[i].clear();
        continue;
      }
      args.blocks[i]->copyTo(&result.blocks[i]);
      // fix block id to prevent potential errors
      {
        if (result.blocks[i].id.find(':') == result.blocks[i].id.npos) {
          result.blocks[i].id = "minecraft:" + result.blocks[i].id;
        }

        if (result.blocks[i].idOld.empty()) {
          result.blocks[i].idOld = result.blocks[i].id;
        }

        if (result.blocks[i].idOld.size() > 0 &&
            (result.blocks[i].idOld.find(':') == result.blocks[i].idOld.npos)) {
          result.blocks[i].idOld = "minecraft:" + result.blocks[i].idOld;
        }
      }
    }
  }

  args.ui.report_working_status(workStatus::collectingColors);

  Eigen::ArrayXi baseColorVer(64);  // 基色对应的版本
  baseColorVer.setConstant(static_cast<int>(SCL_gameVersion::FUTURE));
  baseColorVer.segment(0, 52).setConstant(
      static_cast<int>(SCL_gameVersion::ANCIENT));
  baseColorVer.segment(52, 7).setConstant(
      static_cast<int>(SCL_gameVersion::MC16));
  baseColorVer.segment(59, 3).setConstant(
      static_cast<int>(SCL_gameVersion::MC17));

  std::array<bool, 256> m_index;
  for (short index = 0; index < 256; index++) {
    m_index[index] =
        true;  // 默认可以使用这种颜色，逐次判断各个不可以使用的条件

    if (!args.basecolor_allow_LUT[index2baseColor(
            index)]) {  // 在 allowedBaseColor 中被禁止
      m_index[index] = false;
      continue;
    }
    if (index2baseColor(index) == 0) {  // 空气禁用
      m_index[index] = false;
      continue;
    }
    if (static_cast<int>(result.mc_version_) <
        baseColorVer(index2baseColor(index))) {  // 版本低于基色版本
      m_index[index] = false;
      continue;
    }
    if (result.blocks[index2baseColor(index)].id.empty()) {  // 空 id
      m_index[index] = false;
      continue;
    }
    if (result.is_vanilla() &&
        (index2depth(index) >= 3)) {  // 可实装的地图画不允许第四种阴影
      m_index[index] = false;
      continue;
    }
    if (index2baseColor(index) == 12) {                   // 如果是水且非墙面
      if (result.is_flat() && index2depth(index) != 2) {  // 平面且水深不是 1 格
        m_index[index] = false;
        continue;
      }
    } else {
      if (result.is_flat() && index2depth(index) != 1) {  // 平面且阴影不为 1
        m_index[index] = false;
        continue;
      }
    }
  }

  if (!result.allowed->apply_allowed(*SlopeCraft::basic_colorset, m_index)) {
    // std::string msg = std::format(
    //     "Too few usable color(s) : only {}  colors\nAvailable base color(s) :
    //     ", result.allowed->color_count());
    std::string available_base_colors{"["};
    for (int idx = 0; idx < result.allowed->color_count(); idx++) {
      std::format_to(std::back_insert_iterator{available_base_colors}, "{}, ",
                     result.allowed->Map(idx));
    }
    available_base_colors += "]";
    const auto msg =
        SCLTranslator::tr("SlopeCraftL可用颜色过少：仅仅%1种。\n可用的基色：%2")
            .arg(result.allowed->color_count())
            .arg(available_base_colors.c_str())
            .toStdString();

    report_err(errorFlag::USEABLE_COLOR_TOO_FEW, msg);
    return std::nullopt;
  }

  args.ui.report_working_status(workStatus::none);

  return result;
}

structure_3D* color_table_impl::build(
    const converted_image& cvted, const build_options& option) const noexcept {
  auto opt = structure_3D_impl::create(
      *this, dynamic_cast<const converted_image_impl&>(cvted), option);
  if (opt) {
    return new structure_3D_impl{std::move(opt.value())};
  }
  return nullptr;
}

std::vector<mc_schem::unique_block> color_table_impl::block_id_list(
    bool contain_air) const noexcept {
  std::vector<mc_schem::unique_block> palette;
  palette.reserve(blocks.size() + 1);
  if (contain_air) {
    palette.emplace_back(mc_schem::block::create(mc_schem::common_block::air));
  }
  // Rest elements of array is filled with air blocks or empty. But mc_schem
  // shouldn't encounter duplicates
  for (const auto& blk : blocks) {
    if (blk.id == "air" or blk.id.empty()) {
      break;
    }
    palette.emplace_back(blk.to_mc_schem_block(this->mc_version()));
  }
  return palette;
}

uint64_t color_table_impl::hash() const noexcept {
  boost::uuids::detail::md5 hash;
  SC_HASH_ADD_DATA(hash, this->map_type_)
  SC_HASH_ADD_DATA(hash, this->mc_version_)

  this->allowed->hash_add_data(hash);
  decltype(hash)::digest_type digest;
  hash.get_digest(digest);
  std::array<uint64_t, 2> result;
  memcpy(result.data(), digest, sizeof digest);
  static_assert(sizeof(digest) == sizeof(result));
  return result[0] ^ result[1];
}

static bool is_derived_from(const mc_schem::block& parent,
                            const mc_schem::block& child) {
  const bool is_namespace_ok = [&] {
    // Same namespace, ok
    if (parent.namespace_() == child.namespace_()) {
      return true;
    }
    // Namespace of parent is not declared, ok
    if (parent.namespace_().empty()) {
      return true;
    }
    // Different namespace
    return false;
  }();

  const bool is_pure_id_ok = parent.id() == child.id();

  const bool is_attributes_ok = [&] {
    bool ok = true;
    parent.visit_attributes([&](std::string key, std::string parent_value) {
      const auto child_value = child.get_attribute(key);
      if (not child_value) {
        ok = false;
        return;
      }
      if (child_value.value() != parent_value) {
        ok = false;
        return;
      }
    });
    return ok;
  }();
  return is_namespace_ok and is_pure_id_ok and is_attributes_ok;
}

void color_table_impl::stat_blocks(const structure_3D& s,
                                   size_t buffer[64]) const noexcept {
  const auto& structure = dynamic_cast<const structure_3D_impl&>(s);

  const mc_schem::region* region = structure.schem->get_region(0);
  const auto schem_stat = [&] {
    std::vector<size_t> result;
    result.resize(region->palette_size(), 0);
    region->visit_blocks(
        [&](std::span<const int32_t, 3> pos_xyz [[maybe_unused]],
            uint16_t blkid, auto&, auto) {
          if (blkid >= result.size()) [[unlikely]] {
            return;
          }
          result[blkid]++;
        },
        true);
    return result;
  }();
  assert(schem_stat.size() == structure.palette_length());
  const auto self_blocks = this->block_id_list(false);

  // for (size_t idx_table = 0; idx_table < this->blocks.size(); idx_table++) {
  //   const auto& blk_info = this->blocks[idx_table];
  //   std::string_view full_id = blk_info.idForVersion(this->mc_version());
  //   auto info = blkid::parse_block_id(full_id);
  //   assert(info.has_value());
  //   self_block_info.emplace_back(std::move(info).value());
  // }

  std::fill_n(buffer, 64, 0);
  for (size_t idx_schem = 0; idx_schem < region->palette_size(); idx_schem++) {
    const auto& blk_schem = region->palette(idx_schem);
    if (blk_schem->is_air()) {
      continue;
    }

    for (size_t idx_table = 0; idx_table < self_blocks.size(); idx_table++) {
      assert(idx_table < 64);
      const auto& blk_info_table = self_blocks[idx_table];

      if (is_derived_from(*blk_info_table, *blk_schem)) {
        buffer[idx_table] += schem_stat[idx_schem];
      }
    }
  }
}

std::string color_table_impl::impl_generate_test_schematic(
    const std::string& filename,
    const test_blocklist_options& option) const noexcept {
  if (!filename.ends_with(".nbt")) {
    return SCLTranslator::tr("文件名必须以 *.nbt 结尾").toStdString();
    // return "File name should end with \".nbt\"";
  }

  try {
    std::vector<const mc_block*> realSrc;
    std::vector<uint8_t> realBaseColor;
    realSrc.clear();
    realBaseColor.clear();
    for (size_t idx = 0; idx < option.block_count; idx++) {
      if (option.block_ptrs[idx]->getVersion() > this->mc_version_) {
        continue;
      }
      realSrc.emplace_back(
          static_cast<const mc_block*>(option.block_ptrs[idx]));
      realBaseColor.emplace_back(option.basecolors[idx]);
    }

    std::vector<std::vector<int>> block_counter;
    block_counter.resize(64);

    for (uint32_t idx = 0; idx < realSrc.size(); idx++) {
      block_counter[realBaseColor[idx]].push_back(idx);
    }

    mc_schem::unique_region region;
    {
      std::vector<mc_schem::unique_block> pal;
      std::vector<const mc_schem::block*> pal_view;
      pal.reserve(realSrc.size() + 1);
      pal.emplace_back(mc_schem::block::create(mc_schem::common_block::air));
      for (auto i : realSrc) {
        auto blk = i->to_mc_schem_block(this->mc_version());
        pal.emplace_back(std::move(blk));
      }

      pal_view.reserve(realSrc.size() + 1);
      for (auto& blk : pal) {
        pal_view.emplace_back(blk.get());
      }
      auto region_res =
          mc_schem::region::create_with_palette({1, 1, 1}, pal_view);
      if (not region_res) {
        const auto msg = SCLTranslator::tr("SlopeCraftL内部错误：%1")
                             .arg(region_res.error()->message().c_str());
        return msg.toStdString();
      }
      region = std::move(*region_res);
    }

    int xSize = 0;
    constexpr int zSize = 64, ySize = 2;
    for (const auto& it : block_counter) {
      xSize = std::max(size_t(xSize), it.size());
    }
    region->reshape({xSize + 1, ySize, zSize});
    region->fill_with(*region->palette(0));  // fill with air

    for (uint8_t base = 0; base < 64; base++) {
      for (uint32_t idx = 0; idx < block_counter[base].size(); idx++) {
        const int32_t xPos = idx;
        const int32_t yPos = 0;
        const int32_t zPos = base;

        region->set_block({xPos, yPos, zPos}, block_counter[base][idx] + 1);
      }
      region->set_block(
          {static_cast<int32_t>(block_counter[base].size()), 1, base},
          1);  // glass block
    }

    auto schem = mc_schem::schematic::create();
    schem->metadata()->set_mc_data_version(static_cast<int32_t>(
        MCDataVersion::suggested_version(this->mc_version_)));

    schem->append_region(std::move(region));

    std::ofstream ofs{filename, std::ios::binary};
    mc_schem::vanilla_structure_save_option vsso{};
    auto result = schem->save_vanilla_structure(ofs, vsso);
    if (not result) {
      const auto msg = SCLTranslator::tr("导出结构方块文件%1失败，：%2")
                           .arg(filename.c_str())
                           .arg(result.error()->message().c_str());
      return msg.toStdString();
    }
  } catch (const std::exception& e) {
    const auto msg = SCLTranslator::tr("导出结构方块文件%1失败，：%2")
                         .arg(filename.c_str())
                         .arg(e.what());
    return msg.toStdString();
  }

  return {};
}

std::expected<color_table_searching_index, std::string>
color_table_impl::build_indexer() const noexcept {
  color_table_searching_index indexer;
  for (const auto& blk : this->blocks) {
    auto info_opt = blk.detail_info(this->mc_version());
    if (not info_opt) {
      return std::unexpected{
        SCLTranslator::tr("遇到无效方块id \"%1\"")
            .arg(blk.idForVersion(this->mc_version()))
            .toStdString(),
      };
    }
    indexer.block_LUT.emplace(info_opt.value(), &blk);
  }

  return indexer;
}

const mc_block* color_table_searching_index::find(
    const std::string& id) const noexcept {
  using namespace blkid;

  std::string namespace_, pure_id;
  {
    auto blk = mc_schem::block::create(id.c_str());
    if (not blk) {
      return nullptr;
    }
    namespace_ = (*blk)->namespace_();
    pure_id = (*blk)->id();
  }

  const block_detail_info detail{
    .id_namespace{namespace_},
    .pure_id{pure_id},
  };
  auto it = this->block_LUT.find(detail);
  if (it == this->block_LUT.end()) {
    return nullptr;
  }
  return it->second;
}

std::array<uint32_t, 256> LUT_map_color_to_ARGB() noexcept {
  const auto& basic = *SlopeCraft::basic_colorset;
  std::array<uint32_t, 256> ret;
  ret.fill(0);
  for (size_t idx = 0; idx < 256; idx++) {
    const auto map_color = index2mapColor(idx);
    if (index2baseColor(idx) == 0) {
      ret[map_color] = 0x00000000;
      continue;
    }
    ret[map_color] =
        RGB2ARGB(basic.RGB(idx, 0), basic.RGB(idx, 1), basic.RGB(idx, 2));
  }
  return ret;
}