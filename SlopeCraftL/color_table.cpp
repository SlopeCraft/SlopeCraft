//
// Created by joseph on 4/15/24.
//

#include "color_table.h"
#include <fmt/format.h>

std::optional<color_table_impl> color_table_impl::create(
    const color_table_create_info &args) noexcept {
  color_table_impl result;
  result.mc_version_ = args.mc_version;
  result.map_type_ = args.map_type;
  result.allowed.need_find_side = (args.map_type == mapTypes::Slope);

  auto report_err = [&args](errorFlag flag, std::string_view msg) {
    if (args.callbacks.report_error) {
      args.callbacks.report_error(args.callbacks.wind, flag, msg.data());
    }
  };

  auto report_woring_status = [&args](workStatues ws) {
    if (args.callbacks.report_working_statue) {
      args.callbacks.report_working_statue(args.callbacks.wind, ws);
    }
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

  report_woring_status(workStatues::collectingColors);

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

  report_woring_status(workStatues::none);

  return result;
}

converted_image_impl::converted_image_impl(
    const colorset_allowed_t &allowed_colorset)
    : converter{*color_set::basic, allowed_colorset} {}

converted_image *color_table_impl::convert_image(
    const_image_reference original_img,
    const convert_option &option) const noexcept {
  converted_image_impl cvted{this->allowed};

  const auto algo = (option.algo == convertAlgo::gaCvter)
                        ? convertAlgo::RGB_Better
                        : option.algo;
  //  auto report_err = [&option](errorFlag flag, std::string_view msg) {
  //    if (option.ui.report_error) {
  //      option.ui.report_error(option.ui.wind, flag, msg.data());
  //    }
  //  };

  auto report_woring_status = [&option](workStatues ws) {
    if (option.ui.report_working_statue) {
      option.ui.report_working_statue(option.ui.wind, ws);
    }
  };

  auto progress_range = [&option](int min, int max, int val) {
    if (option.progress.set_range) {
      option.progress.set_range(option.progress.widget, min, max, val);
    }
  };

  //  auto progress_add = [&option](int delta) {
  //    if (option.progress.add) {
  //      option.progress.add(option.progress.widget, delta);
  //    }
  //  };
  cvted.converter.set_raw_image(original_img.data, original_img.rows,
                                original_img.cols, false);
  {
    heu::GAOption opt;
    opt.crossoverProb = option.ai_cvter_opt.crossoverProb;
    opt.mutateProb = option.ai_cvter_opt.mutationProb;
    opt.maxGenerations = option.ai_cvter_opt.maxGeneration;
    opt.maxFailTimes = option.ai_cvter_opt.maxFailTimes;
    opt.populationSize = option.ai_cvter_opt.popSize;
    // here opt is passed as a const ptr, it won't be changed in this function
    // call
    cvted.converter.convert_image(algo, option.dither, &opt);
  }

  progress_range(0, 4 * cvted.size(), 4 * cvted.size());
  report_woring_status(workStatues::none);

  return new converted_image_impl{std::move(cvted)};
}