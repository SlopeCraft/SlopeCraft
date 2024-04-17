//
// Created by joseph on 4/17/24.
//
#include <fmt/format.h>
#include "structure_3D.h"
#include "color_table.h"
#include "lossyCompressor.h"
#include "prim_glass_builder.h"

std::optional<structure_3D_impl> structure_3D_impl::create(
    const color_table_impl &table, const converted_image_impl &cvted,
    const build_options &option) noexcept {
  if (option.max_allowed_height < 14) {
    option.ui.report_error(
        errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14,
        fmt::format("Max allowed height should be >= 14, but found {}",
                    option.max_allowed_height)
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
    fixed_opt.compress_method = compressSettings::noCompress;
    fixed_opt.glass_method = glassBridgeSettings::noBridge;
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
      fixed_opt.glass_method == glassBridgeSettings::withBridge) {
    option.ui.report_working_status(workStatus::constructingBridges);

    option.sub_progressbar.set_range(0, 100, 0);
    const int step = cvted.size() / ret.schem.y_range();

#warning "todo: Use prograss_callback in prim_glass_builder"
    prim_glass_builder glass_builder;
    option.ui.keep_awake();
    for (uint32_t y = 0; y < ret.schem.y_range(); y++) {
      option.sub_progressbar.add(step);
      if (y % (fixed_opt.bridge_interval + 1) == 0) {
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
            if (ret.schem(r, y, c) == prim_glass_builder::air &&
                glass(r, c) == prim_glass_builder::glass)
              ret.schem(r, y, c) = prim_glass_builder::glass;
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