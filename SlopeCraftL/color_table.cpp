//
// Created by joseph on 4/15/24.
//

#include <fmt/format.h>
#include "color_table.h"
#include "WaterItem.h"
#include "height_line.h"
#include "structure_3D.h"

std::optional<color_table_impl> color_table_impl::create(
    const color_table_create_info &args) noexcept {
  color_table_impl result;
  result.mc_version_ = args.mc_version;
  result.map_type_ = args.map_type;
  result.allowed.need_find_side = (args.map_type == mapTypes::Slope);

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
  baseColorVer.setConstant((int)SCL_gameVersion::FUTURE);
  baseColorVer.segment(0, 52).setConstant((int)SCL_gameVersion::ANCIENT);
  baseColorVer.segment(52, 7).setConstant((int)SCL_gameVersion::MC16);
  baseColorVer.segment(59, 3).setConstant((int)SCL_gameVersion::MC17);

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
    if ((int)result.mc_version_ <
        baseColorVer(index2baseColor(index))) {  // 版本低于基色版本
      m_index[index] = false;
      continue;
    }
    if (result.blocks[index2baseColor(index)].id.empty()) {  // 空 id
      m_index[index] = false;
      continue;
    }
    /*
    if ((mapType == mapTypes::Wall) &&
        !blockPalette[index2baseColor(index)]
             .wallUseable) { //墙面像素画且当前方块不适合墙面

      m_index[index] = false;
      continue;
    }*/
    if (result.is_vanilla() &&
        (index2depth(index) >= 3)) {  // 可实装的地图画不允许第四种阴影
      m_index[index] = false;
      continue;
    }
    if (index2baseColor(index) == 12) {  // 如果是水且非墙面
      if (result.is_flat() && index2depth(index) != 2) {  // 平板且水深不是 1 格
        m_index[index] = false;
        continue;
      }
    } else {
      if (result.is_flat() && index2depth(index) != 1) {  // 平板且阴影不为 1
        m_index[index] = false;
        continue;
      }
    }
  }

  if (!result.allowed.apply_allowed(*color_set::basic, m_index)) {
    std::string msg = fmt::format(
        "Too few usable color(s) : only {}  colors\nAvaliable base color(s) : ",
        result.allowed.color_count());

    for (int idx = 0; idx < result.allowed.color_count(); idx++) {
      msg += std::to_string(result.allowed.Map(idx)) + ", ";
    }

    report_err(errorFlag::USEABLE_COLOR_TOO_FEW, msg);
    return std::nullopt;
  }

  args.ui.report_working_status(workStatus::none);

  return result;
}

structure_3D *color_table_impl::build(
    const converted_image &cvted, const build_options &option) const noexcept {
  auto opt = structure_3D_impl::create(
      *this, dynamic_cast<const converted_image_impl &>(cvted), option);
  if (opt) {
    return new structure_3D_impl{std::move(opt.value())};
  }
  return nullptr;
}

std::vector<std::string_view> color_table_impl::block_id_list() const noexcept {
  std::vector<std::string_view> dest;
  dest.reserve(this->blocks.size());
  for (auto &blk : this->blocks) {
    dest.emplace_back(blk.id);
  }
  return dest;
}
