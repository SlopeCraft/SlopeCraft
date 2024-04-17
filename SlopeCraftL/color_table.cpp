//
// Created by joseph on 4/15/24.
//

#include "color_table.h"
#include "WaterItem.h"
#include "HeightLine.h"
#include "lossyCompressor.h"
#include "PrimGlassBuilder.h"
#include <fmt/format.h>

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

converted_image_impl::converted_image_impl(const color_table_impl &table)
    : converter{*color_set::basic, table.allowed} {}

converted_image *color_table_impl::convert_image(
    const_image_reference original_img,
    const convert_option &option) const noexcept {
  converted_image_impl cvted{*this};

  const auto algo = (option.algo == convertAlgo::gaCvter)
                        ? convertAlgo::RGB_Better
                        : option.algo;
  cvted.converter.set_raw_image(original_img.data, original_img.rows,
                                original_img.cols, false);
  {
    heu::GAOption opt;
    opt.crossoverProb = option.ai_cvter_opt.crossoverProb;
    opt.mutateProb = option.ai_cvter_opt.mutationProb;
    opt.maxGenerations = option.ai_cvter_opt.maxGeneration;
    opt.maxFailTimes = option.ai_cvter_opt.maxFailTimes;
    opt.populationSize = option.ai_cvter_opt.popSize;

    cvted.converter.convert_image(algo, option.dither, &opt);
  }

  option.progress.set_range(0, 4 * cvted.size(), 4 * cvted.size());
  option.ui.report_working_status(workStatus::none);

  return new converted_image_impl{std::move(cvted)};
}

std::optional<converted_image_impl::height_maps>
converted_image_impl::height_info(const build_options &option) const noexcept {
  //
  //  std::unordered_map<rc_pos, water_y_range> water_list;

  Eigen::ArrayXXi map_color = this->converter.mapcolor_matrix().cast<int>();

  const bool allow_lossless_compress =
      int(option.compressMethod) & int(SCL_compressSettings::NaturalOnly);

  if (((map_color - 4 * (map_color / 4)) >= 3).any()) {
    std::string msg =
        "Fatal error : SlopeCraftL found map color with depth 3 in a "
        "vanilla map.\n Map contents (map color matrix in col-major) :\n[";
    for (int c = 0; c < map_color.cols(); c++) {
      for (int r = 0; r < map_color.rows(); r++) {
        fmt::format_to(std::back_insert_iterator{msg}, "{},", map_color(r, c));
      }
      msg += ";\n";
    }
    msg += "];\n";
    option.ui.report_error(errorFlag::DEPTH_3_IN_VANILLA_MAP, msg.c_str());
    return std::nullopt;
  }

  Eigen::ArrayXXi base, high_map, low_map;
  base.setZero(this->rows(), this->cols());
  high_map.setZero(this->rows(), this->cols());
  low_map.setZero(this->rows(), this->cols());
  std::unordered_map<rc_pos, water_y_range> water_list;

  LossyCompressor compressor;
  for (int64_t c = 0; c < map_color.cols(); c++) {
    // cerr << "Coloumn " << c << '\n';
    HeightLine HL;
    // getTokiColorPtr(c,&src[0]);
    HL.make(map_color.col(c), allow_lossless_compress);

    if (HL.maxHeight() > option.maxAllowedHeight &&
        (option.compressMethod == compressSettings::ForcedOnly ||
         option.compressMethod == compressSettings::Both)) {
      std::vector<const TokiColor *> ptr(map_color.rows());

      this->converter.col_TokiColor_ptrs(c, ptr.data());
      // getTokiColorPtr(c, &ptr[0]);

      compressor.setSource(HL.getBase(), &ptr[0]);
      bool success =
          compressor.compress(option.maxAllowedHeight, allow_lossless_compress);
      if (!success) {
        option.ui.report_error(
            SCL_errorFlag::LOSSYCOMPRESS_FAILED,
            fmt::format("Failed to compress the 3D structure at column {}.", c)
                .data());
        return std::nullopt;
      }
      Eigen::ArrayXi temp;
      HL.make(&ptr[0], compressor.getResult(), allow_lossless_compress, &temp);
      map_color.col(c) = temp;
    }
    base.col(c) = HL.getBase();
    high_map.col(c) = HL.getHighLine();
    low_map.col(c) = HL.getLowLine();

    auto hl_water_list = HL.getWaterMap();
    water_list.reserve(water_list.size() + hl_water_list.size());
    for (const auto &[r, water_item] : hl_water_list) {
      water_list.emplace(
          rc_pos{static_cast<int32_t>(r), static_cast<int32_t>(c)}, water_item);
    }

    option.main_progressbar.add(4 * this->size());
  }

  return height_maps{.map_color = map_color,
                     .base = base,
                     .high_map = high_map,
                     .low_map = low_map,
                     .water_list = water_list};
}

std::optional<structure_3D_impl> structure_3D_impl::create(
    const color_table_impl &table, const converted_image_impl &cvted,
    const build_options &option) noexcept {
  if (option.maxAllowedHeight < 14) {
    option.ui.report_error(
        errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14,
        fmt::format("Max allowed height should be >= 14, but found {}",
                    option.maxAllowedHeight)
            .c_str());
    return std::nullopt;
  }
  structure_3D_impl ret;
  // set up basic infos
  {
    ret.schem.set_MC_major_version_number(table.mc_version_);
    ret.schem.set_MC_version_number(
        MCDataVersion::suggested_version(table.mc_version_));
    auto id = table.block_id_list();
    ret.schem.set_block_id(id);
  }

  build_options fixed_opt = option;
  if (table.is_flat() || !table.is_vanilla()) {
    fixed_opt.compressMethod = compressSettings::noCompress;
    fixed_opt.glassMethod = glassBridgeSettings::noBridge;
  }
  option.ui.report_working_status(workStatus::buidingHeighMap);
  option.main_progressbar.set_range(0, 9 * cvted.size(), 0);
  {
    std::unordered_map<rc_pos, water_y_range> water_list;
    option.main_progressbar.add(cvted.size());
  }

  Eigen::ArrayXi map_color, base_color, high_map, low_map;
  std::unordered_map<rc_pos, water_y_range> water_list;
  {
    auto opt = cvted.height_info(fixed_opt);
    if (!opt) {
      return std::nullopt;
    }
    map_color = std::move(opt.value().map_color);
    base_color = std::move(opt.value().base);
    high_map = std::move(opt.value().high_map);
    low_map = std::move(opt.value().low_map);
    water_list = std::move(opt.value().water_list);
  }

  ret.schem.resize(2 + cvted.cols(), high_map.maxCoeff() + 1, 2 + cvted.rows());
  ret.schem.set_zero();
  // make 3D
  {
    // base_color(r+1,c)<->High(r+1,c)<->Build(c+1,High(r+1,c),r+1)
    // 为了区分玻璃与空气，张量中存储的是 Base+1.所以元素为 1 对应着玻璃，0
    // 对应空气

    for (auto it = water_list.begin(); it != water_list.end();
         it++)  // 水柱周围的玻璃
    {
      const int x = it->first.col + 1;
      const int z = it->first.row;
      const int y = it->second.high_y;
      const int yLow = it->second.low_y;
      ret.schem(x, y + 1, z) = 0 + 1;  // 柱顶玻璃
      for (int yDynamic = yLow; yDynamic <= y; yDynamic++) {
        ret.schem(x - 1, yDynamic, z - 0) = 1;
        ret.schem(x + 1, yDynamic, z + 0) = 1;
        ret.schem(x + 0, yDynamic, z - 1) = 1;
        ret.schem(x + 0, yDynamic, z + 1) = 1;
      }
      if (yLow >= 1) ret.schem(x, yLow - 1, z) = 1;  // 柱底玻璃
    }

    option.main_progressbar.add(cvted.size());

    for (uint32_t r = -1; r < cvted.rows(); r++)  // 普通方块
    {
      for (uint32_t c = 0; c < cvted.cols(); c++) {
        if (base_color(r + 1, c) == 12 || base_color(r + 1, c) == 0) continue;
        const int x = c + 1;
        const int y = low_map(r + 1, c);
        const int z = r + 1;
        if (y >= 1 && table.blocks[base_color(r + 1, c)].needGlass)
          ret.schem(x, y - 1, z) = 0 + 1;
        if ((option.fire_proof &&
             table.blocks[base_color(r + 1, c)].burnable) ||
            (option.enderman_proof &&
             table.blocks[base_color(r + 1, c)].endermanPickable)) {
          if (y >= 1 && ret.schem(x, y - 1, z) == 0)
            ret.schem(x, y - 1, z) = 0 + 1;
          if (x >= 1 && ret.schem(x - 1, y, z) == 0)
            ret.schem(x - 1, y, z) = 0 + 1;
          if (z >= 1 && ret.schem(x, y, z - 1) == 0)
            ret.schem(x, y, z - 1) = 0 + 1;
          if (y + 1 < ret.schem.y_range() && ret.schem(x, y + 1, z) == 0)
            ret.schem(x, y + 1, z) = 0 + 1;
          if (x + 1 < ret.schem.x_range() && ret.schem(x + 1, y, z) == 0)
            ret.schem(x + 1, y, z) = 0 + 1;
          if (z + 1 < ret.schem.z_range() && ret.schem(x, y, z + 1) == 0)
            ret.schem(x, y, z + 1) = 0 + 1;
        }

        ret.schem(x, y, z) = base_color(r + 1, c) + 1;
      }
      option.main_progressbar.add(cvted.cols());
    }

    option.main_progressbar.add(cvted.size());

    for (auto it = water_list.cbegin(); it != water_list.cend(); ++it) {
      const int x = it->first.col + 1;
      const int z = it->first.row;
      const int y = it->second.high_y;
      const int yLow = it->second.low_y;
      for (int yDynamic = yLow; yDynamic <= y; yDynamic++) {
        ret.schem(x, yDynamic, z) = 13;
      }
    }
  }
  option.main_progressbar.set_range(0, 9 * cvted.size(), 8 * cvted.size());
  // build bridges
  if (table.map_type() == mapTypes::Slope &&
      fixed_opt.glassMethod == glassBridgeSettings::withBridge) {
    option.ui.report_working_status(workStatus::constructingBridges);

    option.sub_progressbar.set_range(0, 100, 0);
    const int step = cvted.size() / ret.schem.y_range();

#warning "todo: Use prograss_callback in PrimGlassBuilder"
    PrimGlassBuilder glass_builder;
    option.ui.keep_awake();
    for (uint32_t y = 0; y < ret.schem.y_range(); y++) {
      option.sub_progressbar.add(step);
      if (y % (fixed_opt.bridgeInterval + 1) == 0) {
        std::array<int, 3> start, extension;  // x,z,y
        start[0] = 0;
        start[1] = 0;
        start[2] = y;
        extension[0] = ret.schem.x_range();
        extension[1] = ret.schem.z_range();
        extension[2] = 1;
        TokiMap targetMap =
            ySlice2TokiMap_u16(ret.schem.tensor(), start, extension);
        glassMap glass;
        // cerr << "Construct glass bridge at y=" << y << endl;
        glass = glass_builder.makeBridge(targetMap);
        for (int r = 0; r < glass.rows(); r++)
          for (int c = 0; c < glass.cols(); c++)
            if (ret.schem(r, y, c) == PrimGlassBuilder::air &&
                glass(r, c) == PrimGlassBuilder::glass)
              ret.schem(r, y, c) = PrimGlassBuilder::glass;
      } else {
        continue;
      }
    }
    option.ui.keep_awake();
    option.sub_progressbar.set_range(0, 100, 100);
  }

  if (fixed_opt.connect_mushrooms) {
    ret.schem.process_mushroom_states();
  }
  option.main_progressbar.set_range(0, 9 * cvted.size(), 9 * cvted.size());
  option.ui.report_working_status(workStatus::none);

  ret.map_color = std::move(map_color);
  return ret;
}