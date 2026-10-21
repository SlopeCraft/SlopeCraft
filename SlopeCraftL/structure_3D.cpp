//
// Created by joseph on 4/17/24.
//
#include <format>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <zstd.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <magic_enum/magic_enum.hpp>

#include "structure_3D.h"
#include "color_table.h"
#include "lossy_compressor.h"
#include "prim_glass_builder.h"
#include "FlatDiagram.h"
#include "MCDataVersion.h"
#include "SCL_translator.h"

using mc_schem::unique_block;
using mc_schem::unique_region;
using mc_schem::unique_schematic;

std::optional<structure_3D_impl> structure_3D_impl::create(
    const color_table_impl& table, const converted_image_impl& cvted,
    const build_options& option) noexcept {
  if (option.max_allowed_height < 14) {
    option.ui.report_error(
        errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14,
        SCLTranslator::tr("允许最大高度应该不低于14，但是遇到了%1")
            .arg(option.max_allowed_height)
            .toStdString()
            .c_str()
        // std::format("Max allowed height should be >= 14, but found {}",
        //             option.max_allowed_height).c_str()
    );
    return std::nullopt;
  }
  // structure_3D_impl ret;
  auto region = [&] -> unique_region {
    const auto palette = table.block_id_list(true);
    std::vector<const mc_schem::block*> palette_view;
    palette_view.reserve(palette.size());
    for (auto& blk : palette) {
      palette_view.emplace_back(blk.get());
    }

    auto region_res =
        mc_schem::region::create_with_palette({1, 1, 1}, palette_view);
    if (not region_res) {
      option.ui.report_error(errorFlag::MC_SCHEM_ERROR,
                             SCLTranslator::tr("创建区域失败：%1")
                                 .arg(region_res.error()->message().c_str())
                                 .toStdString()
                                 .c_str());
      return nullptr;
    }
    return std::move(*region_res);
  }();
  if (not region) {
    return std::nullopt;
  }

  build_options fixed_opt = option;
  if (table.is_flat() || !table.is_vanilla()) {
    fixed_opt.compress_method = compressSettings::noCompress;
    fixed_opt.glass_method = glassBridgeSettings::noBridge;
  }
  fixed_opt.ui.report_working_status(workStatus::buidingHeighMap);
  fixed_opt.main_progressbar.set_range(0, 10 * cvted.size(), 0);
  {
    std::unordered_map<rc_pos, water_y_range> water_list;
    fixed_opt.main_progressbar.add(cvted.size());
  }

  Eigen::ArrayXXi map_color, base_color, high_map, low_map;
  std::unordered_map<rc_pos, water_y_range> water_list;
  {
    auto opt = cvted.height_info(table, fixed_opt);
    if (!opt) {
      return std::nullopt;
    }
    map_color = std::move(opt.value().map_color);
    base_color = std::move(opt.value().base);
    high_map = std::move(opt.value().high_map);
    low_map = std::move(opt.value().low_map);
    water_list = std::move(opt.value().water_list);
  }
  assert((high_map >= low_map).all());
  assert(low_map.minCoeff() == 0);

  try {
    const int32_t height = high_map.maxCoeff() + 1;
    assert(height > 0);
    assert(region->is_dense());  // By default they are dense
    if (height > 32) {
      region->convert_to_sparse();
    }

    region->reshape({static_cast<int32_t>(2 + cvted.cols()),
                     static_cast<int32_t>(height),
                     static_cast<int32_t>(2 + cvted.rows())});
  } catch (const std::bad_alloc& e) {
    const std::array<uint64_t, 3> shape{
      2 + cvted.cols(), static_cast<uint64_t>(high_map.maxCoeff() + 1),
      2 + cvted.rows()};
    const uint64_t bytes_required = shape[0] * shape[1] * shape[2];
    option.ui.report_error(
        errorFlag::MEMORY_ALLOCATE_FAILED,
        SCLTranslator::tr("分配内存失败，需要%1GiB。详情：\"%2\"")
            .arg(static_cast<double>(bytes_required))
            .arg(e.what())
            .toStdString()
            .c_str());
    return std::nullopt;
  }
  try {
    // make 3D
    {
      // base_color(r+1,c)<->High(r+1,c)<->Build(c+1,High(r+1,c),r+1)
      // 为了区分玻璃与空气，张量中存储的是 Base+1.所以元素为 1 对应着玻璃，0
      // 对应空气

      // 水柱周围的玻璃
      for (auto it = water_list.begin(); it != water_list.end(); ++it) {
        const int32_t x = it->first.col + 1;
        const int32_t z = it->first.row;
        const int32_t y = it->second.high_y;
        const int32_t yLow = it->second.low_y;
        region->set_block({x, y + 1, z}, 0 + 1);  // 柱顶玻璃
        for (int yDynamic = yLow; yDynamic <= y; yDynamic++) {
          region->set_block({x - 1, yDynamic, z - 0}, 1);
          region->set_block({x + 1, yDynamic, z + 0}, 1);
          region->set_block({x + 0, yDynamic, z - 1}, 1);
          region->set_block({x + 0, yDynamic, z + 1}, 1);
        }
        if (yLow >= 1) {  // 柱底玻璃
          region->set_block({x, yLow - 1, z}, 1);
        }
      }

      fixed_opt.main_progressbar.add(cvted.size());

      // std::println("{} rows, {} cols", cvted.rows(), cvted.cols());
      //  Common blocks
      for (int64_t r = -1; r < static_cast<int64_t>(cvted.rows()); r++) {
        for (int64_t c = 0; c < static_cast<int64_t>(cvted.cols()); c++) {
          // std::println("r = {}, c = {}", r, c);
          const int cur_base_color = base_color(r + 1, c);
          if (cur_base_color == 12 || cur_base_color == 0) {
            // water or air
            continue;
          }
          const int x = c + 1;
          const int y = high_map(r + 1, c);
          const int z = r + 1;
          if (y >= 1) {
            auto& blk = table.blocks[base_color(r + 1, c)];
            if (blk.needGlass) {
              region->set_block({x, y - 1, z}, 0 + 1);
            }
            if (blk.needStone[table.mc_version_]) {
              region->set_block({x, y - 1, z}, 11 + 1);
            }
          }
          const bool fire_proof = fixed_opt.fire_proof &&
                                  table.blocks[base_color(r + 1, c)].burnable;
          const bool enderman_proof =
              fixed_opt.enderman_proof &&
              table.blocks[base_color(r + 1, c)].endermanPickable;
          if (fire_proof || enderman_proof) {
            if (y >= 1 && region->block_index_at({x, y - 1, z}) == 0)
              region->set_block({x, y - 1, z}, 0 + 1);
            if (x >= 1 && region->block_index_at({x - 1, y, z}) == 0)
              region->set_block({x - 1, y, z}, 0 + 1);
            if (z >= 1 && region->block_index_at({x, y, z - 1}) == 0)
              region->set_block({x, y, z - 1}, 0 + 1);
            if (y + 1 < region->size_xyz()[1] &&
                region->block_index_at({x, y + 1, z}) == 0)
              region->set_block({x, y + 1, z}, 0 + 1);
            if (x + 1 < region->size_xyz()[0] &&
                region->block_index_at({x + 1, y, z}) == 0)
              region->set_block({x + 1, y, z}, 0 + 1);
            if (z + 1 < region->size_xyz()[2] &&
                region->block_index_at({x, y, z + 1}) == 0)
              region->set_block({x, y, z + 1}, 0 + 1);
          }

          region->set_block({x, y, z}, cur_base_color + 1);
        }
        fixed_opt.main_progressbar.add(cvted.cols());
      }

      fixed_opt.main_progressbar.add(cvted.size());

      for (auto it = water_list.cbegin(); it != water_list.cend(); ++it) {
        const int32_t x = it->first.col + 1;
        const int32_t z = it->first.row;
        const int32_t y = it->second.high_y;
        const int32_t yLow = it->second.low_y;
        for (int32_t yDynamic = yLow; yDynamic <= y; yDynamic++) {
          region->set_block({x, yDynamic, z}, 13);
        }
      }
    }
    fixed_opt.main_progressbar.set_range(0, 9 * cvted.size(), 8 * cvted.size());
    // build bridges
    if (table.map_type() == mapTypes::Slope &&
        fixed_opt.glass_method == glassBridgeSettings::withBridge) {
      fixed_opt.ui.report_working_status(workStatus::constructingBridges);

      fixed_opt.sub_progressbar.set_range(0, 100, 0);
      const int step = cvted.size() / region->size_xyz()[1];

      prim_glass_builder glass_builder;
      glass_builder.ui = fixed_opt.ui;
      glass_builder.progress_bar = fixed_opt.sub_progressbar;
      fixed_opt.ui.keep_awake();
      for (int32_t y = 0; y < region->size_xyz()[1]; y++) {
        fixed_opt.sub_progressbar.add(step);
        if (y % (fixed_opt.bridge_interval + 1) == 0) {
          std::array<int, 3> start, extension;  // x,z,y
          start[0] = 0;
          start[1] = 0;
          start[2] = y;
          extension[0] = region->size_xyz()[0];
          extension[1] = region->size_xyz()[2];
          extension[2] = 1;
          // continue;
          TokiMap targetMap = ySlice2TokiMap_u16(*region, start, extension);
          glassMap glass;
          glass = glass_builder.makeBridge(targetMap);
          for (int32_t r = 0; r < glass.rows(); r++)
            for (int32_t c = 0; c < glass.cols(); c++) {
              const std::array<int32_t, 3> pos{r, y, c};
              if (glass(r, c) == prim_glass_builder::glass and
                  region->block_index_at(pos) == prim_glass_builder::air) {
                region->set_block(pos, prim_glass_builder::glass);
              }
            }

          // if (ret.schem(r, y, c) == prim_glass_builder::air &&
          //     glass(r, c) == prim_glass_builder::glass)
          //   ret.schem(r, y, c) = prim_glass_builder::glass;
        }
      }
      fixed_opt.ui.keep_awake();
      fixed_opt.sub_progressbar.set_range(0, 100, 100);
    }

    // mc_schem 1.3.0 doesn't support this function at 1.12-
    if (fixed_opt.connect_mushrooms and
        table.mc_version_ >= SCL_gameVersion::MC13) {
      const mc_schem::mushroom_state_update_option opt{
        .canonicalize_block_id = true,
      };
      region->update_mushroom_state(opt);
    }

    // Add supporting block
    if (fixed_opt.support_block_settings not_eq
        SCL_supportBlockSettings::none) {
      for (int64_t r = -1; r < static_cast<int64_t>(cvted.rows()); r++) {
        for (int64_t c = 0; c < static_cast<int64_t>(cvted.cols()); c++) {
          const int cur_base_color = base_color(r + 1, c);
          if (cur_base_color == 12 || cur_base_color == 0) {
            // water or air
            continue;
          }
          const int32_t x = c + 1;
          // Pos for color-showing block
          const int32_t y = high_map(r + 1, c);
          const int32_t z = r + 1;
          if (y <= 0) {
            continue;
          }
          if (region->block_index_at({x, y - 1, z}) not_eq 0) {
            continue;
          }
          switch (fixed_opt.support_block_settings) {
            case SCL_supportBlockSettings::stone:
              region->set_block({x, y - 1, z}, 11 + 1);
              break;
            case SCL_supportBlockSettings::transparent:
              region->set_block({x, y - 1, z}, 0 + 1);
              break;
            case SCL_supportBlockSettings::none:
              break;
          }
        }
      }
    }

    {
      auto shrink_result = region->shrink_palette();
      if (not shrink_result) {
        fixed_opt.ui.report_error(
            SCL_errorFlag::EXPORT_SCHEM_HAS_INVALID_BLOCKS,
            SCLTranslator::tr("SlopeCraftL为方块表去重时遇到内部错误：%1")
                .arg(shrink_result.error()->message().c_str())
                .toStdString()
                .c_str());
        return std::nullopt;
      }
    }
  } catch (const std::exception& e) {
    fixed_opt.ui.report_error(SCL_errorFlag::MC_SCHEM_ERROR,
                              SCLTranslator::tr("SlopeCraftL 内部错误：%1")
                                  .arg(e.what())
                                  .toStdString()
                                  .c_str());
    return std::nullopt;
  }

  fixed_opt.main_progressbar.set_range(0, 9 * cvted.size(), 9 * cvted.size());
  fixed_opt.ui.report_working_status(workStatus::none);

  // finish region

  auto schem = mc_schem::schematic::create();
  schem->append_region(std::move(region));
  assert(schem->regions_count() == 1);
  {
    const auto data_version = static_cast<int32_t>(
        MCDataVersion::suggested_version(table.mc_version_));
    auto md = mc_schem::metadata_ir::create(data_version);
    if (not md) {
      fixed_opt.ui.report_error(
          SCL_errorFlag::MC_SCHEM_ERROR,
          SCLTranslator::tr("SlopeCraftL 内部错误：mc_schem不支持数据版本%1")
              .arg(data_version)
              .toStdString()
              .c_str());
      return std::nullopt;
    }
    (*md)->set_author("SlopeCraftL");
    (*md)->set_name("by SlopeCraftL");
    (*md)->set_description(
        "This file is generated by "
        "SlopeCraft(https://github.com/SlopeCraft/SlopeCraft) and "
        "mc_schem(https://github.com/SlopeCraft/mc_schem)");

    schem->set_metadata(std::move(*md));
  }

  structure_3D_impl ret;
  ret.schem = std::move(schem);
  ret.map_color = map_color.cast<uint8_t>();
  return ret;
}

bool structure_3D_impl::export_litematica(
    const char* filename,
    const SlopeCraft::litematic_options& option) const noexcept {
  option.ui.report_working_status(workStatus::writingMetaInfo);
  option.progressbar.set_range(0, 100, 0);

  auto md = this->schem->metadata();
  md->set_name(option.litename_utf8);
  option.progressbar.set_range(1, 100, 0);
  {
    std::ofstream ofs{filename, std::ios::binary};
    mc_schem::litematica_save_option lso{};
    auto result = this->schem->save_litematica(ofs, lso);
    if (not result) {
      option.ui.report_error(SCL_errorFlag::MC_SCHEM_ERROR,
                             SCLTranslator::tr("导出%1失败：%2")
                                 .arg(filename)
                                 .arg(result.error()->message().c_str())
                                 .toStdString()
                                 .c_str());
      return false;
    }
  }
  option.ui.report_working_status(workStatus::none);
  option.progressbar.set_range(0, 100, 100);
  return true;
}

bool structure_3D_impl::export_vanilla_structure(
    const char* filename,
    const SlopeCraft::vanilla_structure_options& option) const noexcept {
  option.ui.report_working_status(workStatus::writingMetaInfo);
  option.progressbar.set_range(0, 100, 0);

  {
    mc_schem::vanilla_structure_save_option vsso{};
    vsso.keep_air = not option.is_air_structure_void;
    std::ofstream ofs{filename, std::ios::binary};
    auto result = this->schem->save_vanilla_structure(ofs, vsso);
    if (not result) {
      option.ui.report_error(SCL_errorFlag::MC_SCHEM_ERROR,
                             SCLTranslator::tr("导出%1失败：%2")
                                 .arg(filename)
                                 .arg(result.error()->message().c_str())
                                 .toStdString()
                                 .c_str());
      return false;
    }
  }

  option.progressbar.set_range(0, 100, 100);
  option.ui.report_working_status(workStatus::none);
  return true;
}

bool structure_3D_impl::export_WE_schem(
    const char* filename,
    const SlopeCraft::WE_schem_options& option) const noexcept {
  option.progressbar.set_range(0, 100, 0);

  auto md = this->schem->metadata();
  assert(md);
  md->set_schem_offset(option.offset);
  md->set_schem_we_offset(option.we_offset);

  option.progressbar.set_range(1, 100, 0);
  {
    mc_schem::world_edit13_save_option wsso{};
    std::ofstream ofs{filename, std::ios::binary};
    auto result = this->schem->save_world_edit13(ofs, wsso);
    if (not result) {
      option.ui.report_error(SCL_errorFlag::MC_SCHEM_ERROR,
                             SCLTranslator::tr("导出%1失败：%2")
                                 .arg(filename)
                                 .arg(result.error()->message().c_str())
                                 .toStdString()
                                 .c_str());
      return false;
    }
  }

  option.progressbar.set_range(0, 100, 100);
  return true;
}

bool structure_3D_impl::export_flat_diagram(
    const char* filename, const SlopeCraft::color_table& table_,
    const SlopeCraft::flag_diagram_options& option) const noexcept {
  const auto& table = dynamic_cast<const color_table_impl&>(table_);
  if (table.map_type() != SCL_mapTypes::Flat) {
    option.ui.report_error(
        SCL_errorFlag::EXPORT_FLAT_DIAGRAM_ON_WRONG_MAP_TYPE,
        SCLTranslator::tr(
            "SlopeCraftL只能把平面地图画导出为平面示意图，但是遇到了%1")
            .arg(magic_enum::enum_name(table.map_type()).data())
            .toStdString()
            .c_str()
        // std::format(
        //     "We can only export flat diagram for flat maps, but found {}",
        //     magic_enum::enum_name(table.map_type())).c_str()
    );
    return false;
  }
  auto region = this->schem->get_region(0);
  const libFlatDiagram::fd_option fdopt{
    .row_start = 0,
    .row_end = region->size_xyz<int64_t>()[2],
    .cols = region->size_xyz<int64_t>()[0],
    .split_line_row_margin = option.split_line_row_margin,
    .split_line_col_margin = option.split_line_col_margin,
    .png_compress_level = option.png_compress_level,
    .png_compress_memory_level = option.png_compress_memory_level,
  };

  std::vector<Eigen::Array<uint32_t, 16, 16, Eigen::RowMajor>> img_list_rmj;
  img_list_rmj.reserve(region->palette_size());

  color_table_searching_index block_indexer;
  {
    auto indexer_opt =
        dynamic_cast<const color_table_impl&>(table_).build_indexer();
    if (not indexer_opt) {
      option.ui.report_error(errorFlag::EXPORT_FLAT_DIAGRAM_FAILURE,
                             SCLTranslator::tr("SlopeCraftL内部错误：%1")
                                 .arg(indexer_opt.error().c_str())
                                 .toStdString()
                                 .c_str()
                             // std::format("SlopeCraftL internal error. {}",
                             // indexer_opt.error()).c_str()
      );
      return false;
    }
    block_indexer = std::move(indexer_opt.value());
  }
  for (size_t pblkid = 0; pblkid < region->palette_size(); pblkid++) {
    if (pblkid == 0) {  // air
      img_list_rmj.emplace_back();
      img_list_rmj[0].setZero();
      continue;
    }
    std::string id =
        region->palette(pblkid)->full_id();  // this->schem.palette()[pblkid];
    const mc_block* blkp = block_indexer.find(id);
    if (blkp == nullptr) {
      std::string blkid_full;
      blkid_full.reserve(64 * 2048);
      for (const auto& blk : table.blocks) {
        blkid_full += blk.id;
        blkid_full.push_back('\n');
      }
      option.ui.report_error(
          errorFlag::EXPORT_FLAT_DIAGRAM_FAILURE,
          SCLTranslator::tr(
              "SlopeCraftL内部错误。无法找到与方块%"
              "1匹配的图像。3D结构中对应的方块索引为%2。整个方块列表如下：%3")
              .arg(id.data())
              .arg(pblkid)
              .arg(blkid_full.c_str())
              .toStdString()
              .c_str());
      return false;
    }

    img_list_rmj.emplace_back(blkp->image);
  }

  auto block_at_callback =
      [&img_list_rmj, region](
          const int64_t r, const int64_t c) -> libFlatDiagram::block_img_ref_t {
    if (r < 0 or c < 0 or r >= region->size_xyz()[2] or
        c >= region->size_xyz()[0]) {  // out of range
      return libFlatDiagram::block_img_ref_t{img_list_rmj.at(0).data()};
    }

    const auto ele = region
                         ->block_index_at({static_cast<int32_t>(c), 0,
                                           static_cast<int32_t>(r)})
                         .value();  // this->schem(c, 0, r);
    assert(ele >= 0 and ele < static_cast<ptrdiff_t>(region->palette_size()));

    return libFlatDiagram::block_img_ref_t{img_list_rmj.at(ele).data()};
  };

  std::array<std::pair<std::string, std::string>, 4> txt{
    std::make_pair<std::string, std::string>(
        "Title", "Flat diagram generated by SlopeCraftL."),
    std::make_pair<std::string, std::string>("Software", "SlopeCraftL"),
    std::make_pair<std::string, std::string>(
        "Description",
        "This image is a flat diagram created by SlopeCraftL, which is is "
        "a subproject of SlopeCraft, developed by TokiNoBug."),
    std::make_pair<std::string, std::string>(
        "Comment",
        "SlopeCraft is a free software published "
        "under GPLv3 license. You can find "
        "its repository at https://github.com/SlopeCraft/SlopeCraft")};

  auto err = libFlatDiagram::export_flat_diagram(filename, fdopt,
                                                 block_at_callback, txt);
  if (not err.empty()) {
    option.ui.report_error(errorFlag::EXPORT_FLAT_DIAGRAM_FAILURE, err.c_str());
    return false;
  }
  return true;
}

uint64_t structure_3D_impl::block_count() const noexcept {
  return this->schem->total_blocks(false);
}