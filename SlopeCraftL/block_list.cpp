/*
Copyright © 2021-2023  TokiNoBug
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

#include <fmt/format.h>
#include <json.hpp>
#include <tl/expected.hpp>
#include <zip.h>
#include <libpng_reader.h>

#include "SlopeCraftL.h"
#include "mc_block.h"
#include "blocklist.h"
#include "string_deliver.h"
#include "color_table.h"

using namespace SlopeCraft;

// mc_block_interface *mc_block_interface::create() { return new mc_block; }

std::pair<uint8_t, mc_block> parse_block(const nlohmann::json &jo) noexcept(
    false) {
  mc_block ret;
  const int basecolor = jo.at("baseColor");
  if (basecolor < 0 || basecolor >= 64) {
    throw std::runtime_error{fmt::format("invalid base color: {}", basecolor)};
  }

  ret.id = jo.at("id");
  ret.nameZH = jo.at("nameZH");
  ret.nameEN = jo.at("nameEN");
  ret.imageFilename = jo.at("icon");
  ret.version = jo.at("version");
  if (jo.contains("idOld")) {
    ret.idOld = jo.at("idOld");
  } else {
    ret.idOld = ret.id;
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
      throw std::runtime_error{fmt::format("Invalid stack size: {}", val)};
    }
    ret.stackSize = val;
  }
  if (jo.contains("needStone")) {
    auto &need_stone = jo.at("needStone");
    if (need_stone.is_boolean()) {
      ret.needStone = version_set::all();
    } else if (need_stone.is_array()) {
      for (auto ver : need_stone) {
        if (not ver.is_number_integer()) {
          throw std::runtime_error{
              fmt::format("needStone must be boolean or array of versions, but "
                          "found non-integer element in array")};
        }
        const int ver_int = ver;
        if (ver_int < static_cast<int>(SCL_gameVersion::MC12) or
            ver_int > static_cast<int>(SCL_maxAvailableVersion())) {
          throw std::runtime_error{fmt::format(
              "Found invalid version {} in version list of needStone",
              ver_int)};
        }
        ret.needStone[static_cast<SCL_gameVersion>(ver_int)] = true;
      }
    } else {
      throw std::runtime_error{
          fmt::format("needStone must be boolean or array of versions")};
    }
  }

  return {basecolor, ret};
}

struct zip_deleter {
  void operator()(zip_t *archive) const noexcept {
    if (archive == nullptr) {
      return;
    }
    zip_close(archive);
  }
};

tl::expected<block_list_metainfo, std::string> parse_meta_info(
    std::function<tl::expected<void, std::string>(const char *filename,
                                                  std::vector<uint8_t> &dest)>
        extract_file,
    std::vector<uint8_t> &buffer) noexcept {
  using njson = nlohmann::json;
  // parse meta data
  auto res = extract_file("metainfo.json", buffer);
  if (res) {
    block_list_metainfo ret;
    try {
      njson jo = njson::parse(buffer, nullptr, true, true);
      auto &prefix = jo.at("name prefix");
      ret.prefix_ZH = prefix.at("ZH");
      ret.prefix_EN = prefix.at("EN");
      auto &mods = jo.at("required mods");
      ret.required_mods.reserve(mods.size());
      for (size_t i = 0; i < mods.size(); i++) {
        std::string mod_name = mods[i];
        ret.required_mods.emplace_back(std::move(mod_name));
      }

    } catch (const std::exception &e) {
      return tl::make_unexpected(
          fmt::format("Failed to parse \"metainfo.json\": {}", e.what()));
    }

    return ret;
  }
  return tl::make_unexpected(
      fmt::format("Failed to extract \"metainfo.json\": {}", res.error()));
}

block_list_create_result parse_block_list(zip_t *archive) noexcept {
  std::string warnings{};
  assert(archive not_eq nullptr);

  auto extract_file =
      [archive](const char *filename,
                std::vector<uint8_t> &dest) -> tl::expected<void, std::string> {
    dest.clear();

    int error_code = ZIP_ER_OK;
    const int64_t index_i =
        zip_name_locate(archive, filename, ZIP_FL_UNCHANGED);
    if (index_i < 0) {
      return tl::make_unexpected(
          fmt::format("File \"{}\" doesn't exist in archive", filename));
    }
    const uint64_t index = uint64_t(index_i);

    zip_stat_t stat;
    error_code = zip_stat_index(archive, index, ZIP_FL_UNCHANGED, &stat);
    if (error_code != ZIP_ER_OK) {
      return tl::make_unexpected(
          fmt::format("Failed to get size of file \"{}\"  in archive: \"{}\", "
                      "error code = {}",
                      filename, zip_strerror(archive), error_code));
    }

    const uint64_t file_size = stat.size;
    dest.resize(file_size);

    auto file = zip_fopen(archive, filename, ZIP_FL_UNCHANGED);
    if (file == nullptr) {
      return tl::make_unexpected(
          fmt::format("Failed to extract \"{}\" from archive  : \"{}\" ",
                      filename, zip_strerror(archive)));
    }

    const int64_t read_bytes = zip_fread(file, dest.data(), dest.size());
    if (read_bytes != int64_t(file_size)) {
      return tl::make_unexpected(
          fmt::format("Failed to extract \"{}\" from archive, expected "
                      "{} bytes, but extracted {} bytes : \"{}\" ",
                      filename, file_size, read_bytes, zip_strerror(archive)));
    }
    return {};
  };

  std::vector<uint8_t> buffer;

  block_list bl{};

  using njson = nlohmann::json;
  block_list_metainfo meta_info;
  {
    const char metainfo_name[] = "metainfo.json";
    const int64_t index =
        zip_name_locate(archive, metainfo_name, ZIP_FL_UNCHANGED);
    if (index >= 0) {
      // metainfo.json exists in the archive
      auto mi_res = parse_meta_info(extract_file, buffer);
      if (not mi_res) {
        fmt::format_to(std::back_inserter(warnings),
                       "metainfo.json exist in the archive, but failed to "
                       "parse it: {}\n",
                       mi_res.error());
      }
      meta_info = std::move(mi_res).value_or(block_list_metainfo{});
    }
  }
  // parse json array of blocks
  {
    auto err = extract_file("block_list.json", buffer);
    if (!err) {
      return {tl::make_unexpected(err.error()), warnings};
    }
  }
  try {
    njson jo = njson::parse(buffer, nullptr, true, true);
    if (not jo.is_array()) {
      return {tl::make_unexpected(
                  fmt::format("Json should contain an array directly")),
              warnings};
    }

    // parse blocks
    for (size_t idx = 0; idx < jo.size(); idx++) {
      try {
        auto [version, block] = parse_block(jo[idx]);

        block.nameZH = meta_info.prefix_ZH + block.nameZH;
        block.nameEN = meta_info.prefix_EN + block.nameEN;

        bl.blocks().emplace(std::make_unique<mc_block>(block), version);
      } catch (const std::exception &e) {
        return {tl::make_unexpected(fmt::format(
                    "Failed to parse block at index {}:\n{}", idx, e.what())),
                warnings};
      }
    }

  } catch (const std::exception &e) {
    return {tl::make_unexpected(
                fmt::format("nlohmann json exception : {}", e.what())),
            warnings};
  }
  // load images
  std::vector<uint32_t> buf_pixel;
  for (auto &pair : bl.blocks()) {
    {
      auto err = extract_file(pair.first->imageFilename.c_str(), buffer);
      if (not err) {
        warnings +=
            fmt::format("{}, required by {}", err.error(), pair.first->id);
        continue;
      }
    }

    pair.first->image.resize(16, 16);
    {
      auto [result, warns] = parse_png_into_argb32(buffer, buf_pixel);
      warnings += warns;

      if (!result) {
        fmt::format_to(std::back_insert_iterator{warnings},
                       "Failed to load image \"{}\" because \"{}\"\n",
                       pair.first->getImageFilename(), result.error());
        //        for (uint8_t byte : buffer) {
        //          printf("%02X ", int(byte));
        //        }
        //        printf("\n");
        continue;
      }
      auto image_size = result.value();
      if (image_size.rows != 16 || image_size.cols != 16) {
        fmt::format_to(std::back_insert_iterator{warnings},
                       "{} has invalid shape, expected 16x16, but found {} "
                       "rows x {} cols.\n",
                       pair.first->getImageFilename(), image_size.rows,
                       image_size.cols);
        continue;
      }
    }
    assert(buf_pixel.size() == 16 * 16);
    memcpy(pair.first->image.data(), buf_pixel.data(), 256 * sizeof(uint32_t));
  }

  return block_list_create_result{.result{std::move(bl)}, .warnings{warnings}};
}

block_list_create_result create_block_list_from_file(
    const char *zip_path) noexcept {
  std::string warnings{};
  int error_code = ZIP_ER_OK;
  std::unique_ptr<zip_t, zip_deleter> archive{
      zip_open(zip_path, ZIP_RDONLY | ZIP_CHECKCONS, &error_code)};
  if (error_code not_eq ZIP_ER_OK or archive == nullptr) {
    auto ret = tl::make_unexpected(fmt::format(
        "Failed to open archive \"{}\" : \"{}\" libzip error code = {}",
        zip_path, zip_strerror(archive.get()), error_code));
    return {ret, warnings};
  }

  return parse_block_list(archive.get());
}

block_list_create_result create_block_list_from_buffer(
    std::span<const uint8_t> buffer) noexcept {
  zip_error_t err;
  zip_source_t *const source =
      zip_source_buffer_create(buffer.data(), buffer.size_bytes(), 0, &err);
  if (source == nullptr) {
    return {tl::make_unexpected(fmt::format("Failed to create zip_source_t: {}",
                                            zip_error_strerror(&err))),
            {}};
  }

  std::unique_ptr<zip_t, zip_deleter> archive{
      zip_open_from_source(source, ZIP_RDONLY | ZIP_CHECKCONS, &err)};
  if (archive == nullptr) {
    zip_source_free(source);
    return {tl::make_unexpected(fmt::format("Failed to open zip: {}",
                                            zip_error_strerror(&err))),
            {}};
  }

  return parse_block_list(archive.get());
}