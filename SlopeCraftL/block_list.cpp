/*
Copyright © 2021-2026  TokiNoBug
This file is part of SlopeCraft.

SlopeCraft is free software: you can redistribute it and/or modify
                                                                  it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

                                        SlopeCraft is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

   Contact with me:
   github:https://github.com/SlopeCraft/SlopeCraft
   bilibili:https://space.bilibili.com/351429231
*/

#include <format>
#include <json.hpp>
#include <expected>

#include <zip.h>
#include <libpng_reader.h>
#include <mc_schem.hpp>

#include <parse_game_version.hpp>
#include <parse_version_set.hpp>

#include "SlopeCraftL.h"
#include "mc_block.h"
#include "blocklist.h"
#include "string_deliver.h"
#include "color_table.h"
#include "SCL_translator.h"

using namespace SlopeCraft;

// mc_block_interface *mc_block_interface::create() { return new mc_block; }

static bool check_block_id(const std::string& id) {
  auto result = mc_schem::block::create(id);
  return result.has_value();
}

std::pair<uint8_t, mc_block> parse_block(const nlohmann::json& jo) noexcept(
    false) {
  mc_block ret;
  const int basecolor = jo.at("baseColor");
  if (basecolor < 0 || basecolor >= 64) {
    throw std::runtime_error{
      SCLTranslator::tr("无效的方块基色 %1").arg(basecolor).toStdString()
      // std::format("invalid base color: {}", basecolor)
    };
  }

  ret.id = jo.at("id");
  ret.nameZH = jo.at("nameZH");
  ret.nameEN = jo.at("nameEN");
  ret.imageFilename = jo.at("icon");
  ret.version = [&]() {
    const auto v_opt =
        MCDataVersion::parse_version_from_njson(jo.at("version"));
    if (not v_opt) {
      throw std::runtime_error{
        SCLTranslator::tr("无效的方块版本 %1")
            .arg(jo.at("version").dump().c_str())
            .toStdString()
        // std::format("Invalid version: {}", )
      };
    }
    return v_opt.value();
  }();
  if (jo.contains("idOld")) {
    ret.idOld = jo.at("idOld");
  } else {
    ret.idOld = ret.id;
  }
  if (not check_block_id(ret.id)) {
    throw std::runtime_error{
      SCLTranslator::tr("无效的方块id %1").arg(ret.id.c_str()).toStdString()};
  }
  if (not check_block_id(ret.idOld)) {
    throw std::runtime_error{SCLTranslator::tr("无效的1.12方块id %1")
                                 .arg(ret.idOld.c_str())
                                 .toStdString()};
  }

  if (jo.contains("endermanPickable")) {
    ret.endermanPickable = jo.at("endermanPickable");
  }

  if (jo.contains("isGlowing")) {
    ret.doGlow = jo.at("isGlowing");
  }

  if (jo.contains("burnable")) {
    ret.burnable = jo.at("burnable");
  }

  if (jo.contains("needGlass")) {
    ret.needGlass = jo.at("needGlass");
  }
  if (jo.contains("stackSize")) {
    const int val = jo.at("stackSize");
    if (val <= 0 or val > 64) {
      throw std::runtime_error{
        SCLTranslator::tr("无效的一组方块数量 %1").arg(val).toStdString()
        // std::format("Invalid stack size: {}", val)
      };
    }
    ret.stackSize = val;
  }
  if (jo.contains("needStone")) {
    auto& need_stone = jo.at("needStone");
    ret.needStone = [&]() {
      if (need_stone.is_boolean()) {
        if (static_cast<bool>(need_stone)) {
          return version_set::all();
        }
        return version_set{0};
      }
      auto vs_opt = parse_version_set(need_stone);
      if (vs_opt.has_value()) {
        return vs_opt.value();
      }
      throw std::runtime_error{
        SCLTranslator::tr("方块属性needStone必须是bool或者版本数组")
            .toStdString()
        // std::format("needStone must be boolean or array of versions")
      };
    }();
  }

  return {basecolor, ret};
}

struct zip_deleter {
  void operator()(zip_t* archive) const noexcept {
    if (archive == nullptr) {
      return;
    }
    zip_close(archive);
  }
};

std::expected<block_list_metainfo, std::string> parse_meta_info(
    std::function<std::expected<void, std::string>(const char* filename,
                                                   std::vector<uint8_t>& dest)>
        extract_file,
    std::vector<uint8_t>& buffer) noexcept {
  using njson = nlohmann::json;
  // parse meta data
  auto res = extract_file("metainfo.json", buffer);
  if (res) {
    block_list_metainfo ret;
    try {
      njson jo = njson::parse(buffer, nullptr, true, true);
      auto& prefix = jo.at("name prefix");
      ret.prefix_ZH = prefix.at("ZH");
      ret.prefix_EN = prefix.at("EN");
      auto& mods = jo.at("required mods");
      ret.required_mods.reserve(mods.size());
      for (size_t i = 0; i < mods.size(); i++) {
        std::string mod_name = mods[i];
        ret.required_mods.emplace_back(std::move(mod_name));
      }

    } catch (const std::exception& e) {
      return std::unexpected(
          SCLTranslator::tr("无法解析 metainfo.json：%1")
              .arg(e.what())
              .toStdString()
          // std::format("Failed to parse \"metainfo.json\": {}", e.what())
      );
    }

    return ret;
  }
  return std::unexpected(
      SCLTranslator::tr("无法解压 metainfo.json：%1")
          .arg(res.error().c_str())
          .toStdString()
      // std::format("Failed to extract \"metainfo.json\": {}", res.error())
  );
}

block_list_create_result parse_block_list(zip_t* archive) noexcept {
  std::string warnings{};
  assert(archive not_eq nullptr);

  auto extract_file =
      [archive](
          const char* filename,
          std::vector<uint8_t>& dest) -> std::expected<void, std::string> {
    dest.clear();

    int error_code = ZIP_ER_OK;
    const int64_t index_i =
        zip_name_locate(archive, filename, ZIP_FL_UNCHANGED);
    if (index_i < 0) {
      return std::unexpected(
          SCLTranslator::tr("方块列表压缩包中缺少文件 \"%1\"")
              .arg(filename)
              .toStdString()
          // std::format("File \"{}\" doesn't exist in archive", filename)
      );
    }
    const auto index = static_cast<uint64_t>(index_i);

    zip_stat_t stat;
    error_code = zip_stat_index(archive, index, ZIP_FL_UNCHANGED, &stat);
    if (error_code != ZIP_ER_OK) {
      return std::unexpected(
          SCLTranslator::tr("无法获取方块列表中文件%1的大小：%2，错误码%3")
              .arg(filename)
              .arg(zip_strerror(archive))
              .arg(error_code)
              .toStdString()
          // std::format("Failed to get size of file \"{}\"  in archive:
          // \"{}\" error code = {}",filename, zip_strerror(archive),
          //             error_code)
      );
    }

    const uint64_t file_size = stat.size;
    dest.resize(file_size);

    auto file = zip_fopen(archive, filename, ZIP_FL_UNCHANGED);
    if (file == nullptr) {
      return std::unexpected(
          SCLTranslator::tr("无法从方块列表中解压文件%1：%2")
              .arg(filename)
              .arg(zip_strerror(archive))
              .toStdString()
          // std::format("Failed to extract \"{}\" from archive  : \"{}\" ",
          //             filename, zip_strerror(archive))
      );
    }

    const int64_t read_bytes = zip_fread(file, dest.data(), dest.size());
    if (read_bytes != static_cast<int64_t>(file_size)) {
      return std::unexpected(
          SCLTranslator::tr(
              "无法从方块列表中解压文件%1，应有%2字节，实际上只有%3字节：%4")
              .arg(filename)
              .arg(file_size)
              .arg(read_bytes)
              .arg(zip_strerror(archive))
              .toStdString()
          // std::format("Failed to extract \"{}\" from archive, expected "
          //             "{} bytes, but extracted {} bytes : \"{}\" ",
          //             filename, file_size, read_bytes, zip_strerror(archive))
      );
    }
    return {};
  };

  std::vector<uint8_t> buffer;

  block_list bl{};

  using njson = nlohmann::json;
  block_list_metainfo meta_info;
  {
    constexpr char metainfo_name[] = "metainfo.json";
    const int64_t index =
        zip_name_locate(archive, metainfo_name, ZIP_FL_UNCHANGED);
    if (index >= 0) {
      // metainfo.json exists in the archive
      auto mi_res = parse_meta_info(extract_file, buffer);
      if (not mi_res) {
        warnings +=
            SCLTranslator::tr("方块列表中有metainfo.json，但是解析失败：%1\n")
                .arg(mi_res.error().c_str())
                .toStdString();
        // std::format_to(std::back_inserter(warnings),
        //                "metainfo.json exist in the archive, but failed to "
        //                "parse it: {}\n",
        //                mi_res.error()
        //                );
      }
      meta_info = std::move(mi_res).value_or(block_list_metainfo{});
    }
  }
  // parse JSON array of blocks
  {
    auto err = extract_file("block_list.json", buffer);
    if (!err) {
      return {std::unexpected(err.error()), warnings};
    }
  }
  try {
    njson jo = njson::parse(buffer, nullptr, true, true);
    if (not jo.is_array()) {
      return {
        .result = std::unexpected(
            SCLTranslator::tr("方块列表json应该直接包含json数组").toStdString()
            // std::format("JSON should contain an array directly")
            ),
        .warnings = warnings,
      };
    }

    // parse blocks
    for (size_t idx = 0; idx < jo.size(); idx++) {
      try {
        auto [version, block] = parse_block(jo[idx]);

        block.nameZH = meta_info.prefix_ZH + block.nameZH;
        block.nameEN = meta_info.prefix_EN + block.nameEN;

        bl.blocks().emplace(std::make_unique<mc_block>(block), version);
      } catch (const std::exception& e) {
        return {
          .result = std::unexpected(SCLTranslator::tr("无法解析第%1个方块：%2")
                                        .arg(idx)
                                        .arg(e.what())
                                        .toStdString()
                                    // std::format("Failed to parse block at
                                    // index {}:\n{}", idx, e.what())
                                    ),
          .warnings = warnings,
        };
      }
    }

  } catch (const std::exception& e) {
    return {
      .result = std::unexpected(
          SCLTranslator::tr("解析json时遇到异常：%1")
              .arg(e.what())
              .toStdString()
          // std::format("nlohmann json exception : {}", e.what())
          ),
      .warnings = warnings,
    };
  }
  // load images
  std::vector<uint32_t> buf_pixel;
  for (auto& pair : bl.blocks()) {
    {
      auto err = extract_file(pair.first->imageFilename.c_str(), buffer);
      if (not err) {
        warnings += SCLTranslator::tr("%1缺少图片：%2\n")
                        .arg(pair.first->id.c_str())
                        .arg(err.error().c_str())
                        .toStdString();
        // std::format("{}, required by {}", err.error(), pair.first->id);
        continue;
      }
    }

    pair.first->image.resize(16, 16);
    {
      auto [result, warns] = parse_png_into_argb32(buffer, buf_pixel);
      warnings += warns;

      if (!result) {
        warnings += SCLTranslator::tr("无法读取图片%1：%2\n")
                        .arg(pair.first->getImageFilename())
                        .arg(result.error().c_str())
                        .toStdString();
        // std::format_to(std::back_insert_iterator{warnings},
        //                "Failed to load image \"{}\" because \"{}\"\n",
        //                pair.first->getImageFilename(), result.error());
        continue;
      }
      auto image_size = result.value();
      if (image_size.rows != 16 || image_size.cols != 16) {
        warnings +=
            SCLTranslator::tr("%1的尺寸错误，应当是16x16，但实际上是%2行%3列\n")
                .arg(pair.first->getImageFilename())
                .arg(image_size.rows)
                .arg(image_size.cols)
                .toStdString();
        // std::format_to(std::back_insert_iterator{warnings},
        //                "{} has invalid shape, expected 16x16, but found {} "
        //                "rows x {} cols.\n",
        //                pair.first->getImageFilename(), image_size.rows,
        //                image_size.cols);
        continue;
      }
    }
    assert(buf_pixel.size() == 16 * 16);
    memcpy(pair.first->image.data(), buf_pixel.data(), 256 * sizeof(uint32_t));
  }

  return block_list_create_result{
    .result{std::move(bl)},
    .warnings{warnings},
  };
}

block_list_create_result create_block_list_from_file(
    const char* zip_path) noexcept {
  std::string warnings{};
  int error_code = ZIP_ER_OK;
  std::unique_ptr<zip_t, zip_deleter> archive{
    zip_open(zip_path, ZIP_RDONLY | ZIP_CHECKCONS, &error_code)};
  if (error_code not_eq ZIP_ER_OK or archive == nullptr) {
    auto ret = std::unexpected(
        SCLTranslator::tr("无法打开压缩包%1：%2。libzip错误码：%3")
            .arg(zip_path)
            .arg(zip_strerror(archive.get()))
            .arg(error_code)
            .toStdString()
        // std::format(
        //   "Failed to open archive \"{}\" : \"{}\" libzip error code = {}",
        //   zip_path, zip_strerror(archive.get()), error_code)
    );
    return {
      .result = ret,
      .warnings = warnings,
    };
  }

  return parse_block_list(archive.get());
}

block_list_create_result create_block_list_from_buffer(
    std::span<const uint8_t> buffer) noexcept {
  zip_error_t err;
  zip_source_t* const source =
      zip_source_buffer_create(buffer.data(), buffer.size_bytes(), 0, &err);
  if (source == nullptr) {
    return {
      .result = std::unexpected(SCLTranslator::tr("无法创建zip_source_t：%1")
                                    .arg(zip_error_strerror(&err))
                                    .toStdString()),
      // std::unexpected(std::format("Failed to create zip_source_t:
      // {}",
      //                                     zip_error_strerror(&err))),
      .warnings = {},
    };
  }

  std::unique_ptr<zip_t, zip_deleter> archive{
    zip_open_from_source(source, ZIP_RDONLY | ZIP_CHECKCONS, &err)};
  if (archive == nullptr) {
    zip_source_free(source);
    return {
      .result = std::unexpected(
          SCLTranslator::tr(
              "无法打开压缩包，zip_err错误码：%1，sys_err错误码：%2")
              .arg(err.zip_err)
              .arg(err.sys_err)
              .toStdString()),
      // std::unexpected(
      //           std::format("Failed to open zip, zip_err = {},
      //           sys_err = {}",
      //                       err.zip_err, err.sys_err)),
      .warnings = {},
    };
  }

  return parse_block_list(archive.get());
}