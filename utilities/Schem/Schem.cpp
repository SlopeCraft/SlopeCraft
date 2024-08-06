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

#include "Schem.h"

#include <memory.h>

#include <ctime>
#include <filesystem>
#include <iostream>
#include <fmt/format.h>

#include "../NBTWriter/NBTWriter.h"
#include "bit_shrink.h"

using namespace libSchem;

WorldEditSchem_info::WorldEditSchem_info() : date(std::time(nullptr) * 1000) {}

litematic_info::litematic_info()
    : time_created(std::time(nullptr) * 1000),
      time_modified(std::time(nullptr) * 1000) {}

void Schem::resize(int64_t x, int64_t y, int64_t z) {
  if (x < 0 || y < 0 || z < 0) {
    return;
  }
  this->xzy.resize(x, z, y);
}

std::string Schem::check_size() const noexcept {
  return check_size(this->x_range(), this->y_range(), this->z_range());
}

std::string Schem::check_size(int64_t x, int64_t y, int64_t z) noexcept {
  if (x < 0 || y < 0 || z < 0) {
    return fmt::format(
        "Size in one or more dimensions is negative. x= {}, y= {}, z= {}", x, y,
        z);
  }
  return {};
}

void Schem::stat_blocks(std::vector<size_t> &dest) const noexcept {
  dest.resize(this->palette_size());
  std::fill(dest.begin(), dest.end(), 0);

  for (ele_t block_index : *this) {
    assert(block_index < this->palette_size());
    dest[block_index] += 1;
  }
}

void Schem::set_block_id(const char *const *const block_ids,
                         const int num) noexcept {
  if (num < 0) {
    return;
  }
  if (block_ids == nullptr) {
    return;
  }
  this->block_id_list.resize(num);

  for (int idx = 0; idx < num; idx++) {
    this->block_id_list[idx] = block_ids[idx];
  }
}

void Schem::set_block_id(std::span<std::string_view> id) noexcept {
  this->block_id_list.resize(id.size());
  for (size_t idx = 0; idx < id.size(); idx++) {
    this->block_id_list[idx] = id[idx];
  }
}

int64_t Schem::non_zero_count() const noexcept {
  int64_t val = 0;

  for (int64_t i = 0; i < size(); i++) {
    val += (xzy(i) != 0);
  }
  return val;
}

bool Schem::have_invalid_block(
    int64_t *first_invalid_block_idx) const noexcept {
  for (int64_t idx = 0; idx < xzy.size(); idx++) {
    if (xzy(idx) >= block_id_list.size()) {
      if (first_invalid_block_idx != nullptr) {
        *first_invalid_block_idx = idx;
      }
      return true;
    }
  }
  return false;
}

bool Schem::have_invalid_block(
    int64_t *first_invalid_block_x_pos, int64_t *first_invalid_block_y_pos,
    int64_t *first_invalid_block_z_pos) const noexcept {
  for (int64_t y = 0; y < y_range(); y++) {
    for (int64_t z = 0; z < z_range(); z++) {
      for (int64_t x = 0; x < x_range(); x++) {
        if (this->xzy(x, z, y) >= block_id_list.size()) {
          if (first_invalid_block_x_pos != nullptr) {
            *first_invalid_block_x_pos = x;
          }
          if (first_invalid_block_y_pos != nullptr) {
            *first_invalid_block_y_pos = y;
          }
          if (first_invalid_block_z_pos != nullptr) {
            *first_invalid_block_z_pos = z;
          }
          return true;
        }
      }
    }
  }
  return false;
}

enum class __mushroom_type : uint8_t {
  not_mushroom = 0,
  red_mushroom = 1,
  brown_mushroom = 2,
  mushroom_stem = 3
};

void Schem::process_mushroom_states() noexcept {
  static const std::string id_red = "minecraft:red_mushroom_block";
  static const std::string id_brown = "minecraft:brown_mushroom_block";
  static const std::string id_stem = "minecraft:mushroom_stem";

  // map __mushroom_state(u6) to ele_t
  std::array<ele_t, 64> u6_to_ele_red;
  u6_to_ele_red.fill(invalid_ele_t);
  std::array<ele_t, 64> u6_to_ele_brown;
  u6_to_ele_brown.fill(invalid_ele_t);
  std::array<ele_t, 64> u6_to_ele_stem;
  u6_to_ele_stem.fill(invalid_ele_t);

  // find exisiting mushroom blocks
  for (ele_t idx = 0; idx < ele_t(this->palette_size()); idx++) {
    const auto &block_id = this->block_id_list[idx];
    const auto pure_id = ::to_pure_block_id(block_id);
    if (pure_id == id_red || pure_id == id_brown || pure_id == id_stem) {
    } else {
      continue;
    }

    const __mushroom_sides side = __mushroom_sides::from_block_id(block_id);
    if (pure_id == id_red) {
      u6_to_ele_red[side.u6()] = idx;
    } else if (pure_id == id_brown) {
      u6_to_ele_brown[side.u6()] = idx;
    } else {
      u6_to_ele_stem[side.u6()] = idx;
    }
  }

  for (uint8_t u6 = 0; u6 < 64; u6++) {
    const __mushroom_sides side(u6);
    if (u6_to_ele_red[u6] == invalid_ele_t) {
      this->block_id_list.emplace_back(side.to_blockid(id_red));
      u6_to_ele_red[u6] = this->block_id_list.size() - 1;
    }
    if (u6_to_ele_brown[u6] == invalid_ele_t) {
      this->block_id_list.emplace_back(side.to_blockid(id_brown));
      u6_to_ele_brown[u6] = this->block_id_list.size() - 1;
    }
    if (u6_to_ele_stem[u6] == invalid_ele_t) {
      this->block_id_list.emplace_back(side.to_blockid(id_stem));
      u6_to_ele_stem[u6] = this->block_id_list.size() - 1;
    }
  }
  std::vector<__mushroom_type> is_mushroom_LUT;
  is_mushroom_LUT.resize(this->block_id_list.size());

  for (int blockidx = 0; blockidx < int(this->block_id_list.size());
       blockidx++) {
    const auto &block_id = this->block_id_list[blockidx];
    const auto pure_id = ::to_pure_block_id(block_id);
    if (pure_id == id_red) {
      is_mushroom_LUT[blockidx] = __mushroom_type::red_mushroom;
      continue;
    }
    if (pure_id == id_brown) {
      is_mushroom_LUT[blockidx] = __mushroom_type::brown_mushroom;
      continue;
    }
    if (pure_id == id_stem) {
      is_mushroom_LUT[blockidx] = __mushroom_type::mushroom_stem;
      continue;
    }
    is_mushroom_LUT[blockidx] = __mushroom_type::not_mushroom;
  }

  // fix the correct state
  for (int64_t y = 0; y < y_range(); y++) {
    for (int64_t z = 0; z < z_range(); z++) {
      for (int64_t x = 0; x < x_range(); x++) {
        // if current block is not mushroom, continue
        const __mushroom_type current_mushroom_type =
            is_mushroom_LUT[this->operator()(x, y, z)];
        if (current_mushroom_type == __mushroom_type::not_mushroom) {
          continue;
        }

        __mushroom_sides side;
        // match the correct side
        if (x + 1 < x_range()) {
          const ele_t ele_of_side = this->operator()(x + 1, y, z);
          if (is_mushroom_LUT[ele_of_side] != __mushroom_type::not_mushroom) {
            side.east() = false;
          }
        }

        if (x - 1 >= 0) {
          const ele_t ele_of_side = this->operator()(x - 1, y, z);
          if (is_mushroom_LUT[ele_of_side] != __mushroom_type::not_mushroom) {
            side.west() = false;
          }
        }

        if (y + 1 < y_range()) {
          const ele_t ele_of_side = this->operator()(x, y + 1, z);
          if (is_mushroom_LUT[ele_of_side] != __mushroom_type::not_mushroom) {
            side.up() = false;
          }
        }

        if (y - 1 >= 0) {
          const ele_t ele_of_side = this->operator()(x, y - 1, z);
          if (is_mushroom_LUT[ele_of_side] != __mushroom_type::not_mushroom) {
            side.down() = false;
          }
        }

        if (z + 1 < z_range()) {
          const ele_t ele_of_side = this->operator()(x, y, z + 1);
          if (is_mushroom_LUT[ele_of_side] != __mushroom_type::not_mushroom) {
            side.south() = false;
          }
        }

        if (z - 1 >= 0) {
          const ele_t ele_of_side = this->operator()(x, y, z - 1);
          if (is_mushroom_LUT[ele_of_side] != __mushroom_type::not_mushroom) {
            side.north() = false;
          }
        }

        // write in the correct value of ele_t
        ele_t corrected_ele = invalid_ele_t;
        switch (current_mushroom_type) {
          case __mushroom_type::brown_mushroom:
            corrected_ele = u6_to_ele_brown[side.u6()];
            break;
          case __mushroom_type::red_mushroom:
            corrected_ele = u6_to_ele_red[side.u6()];
            break;
          default:
            corrected_ele = u6_to_ele_stem[side.u6()];
            break;
        }

        this->operator()(x, y, z) = corrected_ele;
      }
    }
  }
  return;
}

tl::expected<void, std::pair<SCL_errorFlag, std::string>> Schem::pre_check(
    std::string_view filename, std::string_view extension) const noexcept {
  if (std::filesystem::path(filename).extension() != extension) {
    // wrong extension
    return tl::make_unexpected(std::make_pair(
        SCL_errorFlag::EXPORT_SCHEM_WRONG_EXTENSION,
        fmt::format("The filename extension must be \"{}\".", extension)));
  }
  // check for invalid blocks
  {
    std::array<int64_t, 3> pos;
    if (this->have_invalid_block(&pos[0], &pos[1], &pos[2])) {
      return tl::make_unexpected(std::make_pair(
          SCL_errorFlag::EXPORT_SCHEM_HAS_INVALID_BLOCKS,
          fmt::format("The first invalid block is at x={}, y={}, z={}", pos[0],
                      pos[1], pos[2])));
    }
  }
  return {};
}

tl::expected<void, std::pair<SCL_errorFlag, std::string>>
Schem::export_litematic(std::string_view filename,
                        const litematic_info &info) const noexcept {
  {
    auto res = this->pre_check(filename, ".litematic");
    if (not res) {
      return res;
    }
  }
  NBT::NBTWriter<true> lite;

  if (!lite.open(filename.data())) {
    return tl::make_unexpected(
        std::make_pair(SCL_errorFlag::EXPORT_SCHEM_FAILED_TO_CREATE_FILE,
                       fmt::format("Failed to open file: {}", filename)));
  }

  lite.writeCompound("Metadata");
  {
    lite.writeCompound("EnclosingSize");
    {
      lite.writeInt("x", this->x_range());
      lite.writeInt("y", this->y_range());
      lite.writeInt("z", this->z_range());
    }
    lite.endCompound();

    lite.writeString("Author", info.author_utf8.data());

    lite.writeString("Description", info.destricption_utf8.data());

    lite.writeString("Name", info.litename_utf8.data());

    lite.writeInt("RegionCount", 1);
    lite.writeLong("TimeCreated", info.time_created);
    lite.writeLong("TimeModified", info.time_modified);
    lite.writeInt("TotalBlocks", this->non_zero_count());
    lite.writeInt("TotalVolume", this->size());
  }
  lite.endCompound();

  // progressRangeSet(wind, 0, 100 + Build.size(), 50);

  lite.writeCompound("Regions");
  {
    lite.writeCompound(info.regionname_utf8.data());
    {
      lite.writeCompound("Position");
      {
        lite.writeInt("x", 0);
        lite.writeInt("y", 0);
        lite.writeInt("z", 0);
      }
      lite.endCompound();

      lite.writeCompound("Size");
      {
        lite.writeInt("x", this->x_range());
        lite.writeInt("y", this->y_range());
        lite.writeInt("z", this->z_range());
      }
      lite.endCompound();

      // progressRangeSet(wind, 0, 100 + Build.size(), 100);

      // reportWorkingStatue(wind, workStatus::writingBlockPalette);
      // write block palette
      lite.writeListHead("BlockStatePalette", NBT::Compound,
                         this->palette_size());
      {
        std::string pure_block_id;
        pure_block_id.reserve(1024);
        memset(pure_block_id.data(), 0, pure_block_id.capacity());
        std::vector<std::pair<std::string, std::string>> properties;
        properties.reserve(64);

        for (const auto &block_string : this->block_id_list) {
          process_block_id(block_string, &pure_block_id, &properties);
          // write a block
          lite.writeCompound("ThisStringShouldNeverBeSeen");
          {
            lite.writeString("Name", pure_block_id.data());
            if (properties.size()) {
              lite.writeCompound("Properties");
              {
                for (const auto &prop : properties) {
                  lite.writeString(prop.first.data(), prop.second.data());
                }
              }
              lite.endCompound();
            }
          }
          lite.endCompound();
        }
      }

      lite.writeListHead("Entities", NBT::Compound, 0);
      lite.writeListHead("PendingBlockTicks", NBT::Compound, 0);
      lite.writeListHead("PendingFluidTiccks", NBT::Compound, 0);
      lite.writeListHead("TileEntities", NBT::Compound, 0);

      // write 3D
      std::vector<uint64_t> shrinked;
      shrink_bits(this->xzy.data(), xzy.size(), this->palette_size(),
                  &shrinked);

      lite.writeLongArrayHead("BlockStates", shrinked.size());
      {
        for (int64_t idx = 0; idx < int64_t(shrinked.size()); idx++) {
          lite.writeSingleTag<int64_t, true>(
              NBT::Long, "id", reinterpret_cast<int64_t &>(shrinked[idx]));
        }
      }
      // progressAdd(wind, size3D[0]);

      if (not this->entities.empty()) {
        lite.writeListHead("Entities", NBT::tagType::Compound,
                           this->entities.size());
        for (auto &entity : this->entities) {
          assert(entity);
          lite.writeCompound();
          auto res = entity->dump(lite, this->MC_data_ver);
          if (not res) {
            lite.endCompound();
            return tl::make_unexpected(
                std::make_pair(SCL_errorFlag::EXPORT_SCHEM_HAS_INVALID_ENTITY,
                               std::move(res.error())));
          }
          lite.endCompound();
        }
      }
    }
    lite.endCompound();  // end current region
  }
  lite.endCompound();  // end all regions

  switch (this->MC_major_ver) {
    case ::SCL_gameVersion::MC12:
      lite.writeInt("MinecraftDataVersion", (int)this->MC_version_number());
      lite.writeInt("Version", 4);
      break;
    case ::SCL_gameVersion::MC13:
    case ::SCL_gameVersion::MC14:
    case ::SCL_gameVersion::MC15:
    case ::SCL_gameVersion::MC16:
    case ::SCL_gameVersion::MC17:
    case ::SCL_gameVersion::MC18:
    case ::SCL_gameVersion::MC19:
    case ::SCL_gameVersion::MC20:
      lite.writeInt("MinecraftDataVersion", (int)this->MC_version_number());
      lite.writeInt("Version", 5);
      break;
    default:
      lite.close();
      return tl::make_unexpected(std::make_pair(
          SCL_errorFlag::UNKNOWN_MAJOR_GAME_VERSION,
          fmt::format("Unknown major game version! Only 1.12 to 1.19 is "
                      "supported, but given value {}",
                      int(this->MC_major_ver))));
  }
  lite.close();

  return {};
}

tl::expected<void, std::pair<SCL_errorFlag, std::string>>
Schem::export_structure(std::string_view filename,
                        const bool is_air_structure_void) const noexcept {
  {
    auto res = this->pre_check(filename, ".nbt");
    if (not res) {
      return res;
    }
  }

  uint16_t number_of_air;
  for (number_of_air = 0; number_of_air < this->block_id_list.size();
       number_of_air++) {
    if (0 ==
        std::strcmp("minecraft:air", block_id_list[number_of_air].c_str())) {
      break;
    }
  }

  if ((!is_air_structure_void) &&
      (number_of_air >= this->block_id_list.size())) {
    std::cerr << "You assigned is_air_structure_void=false, but there is no "
                 "minecraft:air in your block palette."
              << std::endl;

    return tl::make_unexpected(std::make_pair(
        SCL_errorFlag::EXPORT_SCHEM_STRUCTURE_REQUIRES_AIR,
        "You assigned is_air_structure_void=false, but there is no "
        "minecraft:air in your block palette."));
  }

  /*
  reportWorkingStatue(wind, workStatus::writingMetaInfo);
  progressRangeSet(wind, 0, 100 + Build.size(), 0);
  */

  NBT::NBTWriter<true> file;
  if (!file.open(filename.data())) {
    return tl::make_unexpected(
        std::make_pair(SCL_errorFlag::EXPORT_SCHEM_FAILED_TO_CREATE_FILE,
                       fmt::format("Failed to open file {}", filename)));
  }

  file.writeListHead("entities", NBT::Byte, 0);
  file.writeListHead("size", NBT::Int, 3);
  {
    file.writeInt("This should never be shown", x_range());
    file.writeInt("This should never be shown", y_range());
    file.writeInt("This should never be shown", z_range());
  }
  // reportWorkingStatue(wind, workStatus::writingBlockPalette);
  file.writeListHead("palette", NBT::Compound, palette_size());
  {
    std::string pure_block_id;
    pure_block_id.reserve(1024);
    memset(pure_block_id.data(), 0, pure_block_id.capacity());
    std::vector<std::pair<std::string, std::string>> properties;
    properties.reserve(64);
    for (const auto &block_string : this->block_id_list) {
      process_block_id(block_string, &pure_block_id, &properties);
      // write a block
      file.writeCompound("ThisStringShouldNeverBeSeen");
      {
        file.writeString("Name", pure_block_id.data());
        if (properties.size()) {
          file.writeCompound("Properties");
          {
            for (const auto &prop : properties) {
              file.writeString(prop.first.data(), prop.second.data());
            }
          }
          file.endCompound();
        }
      }
      file.endCompound();
    }
  }
  // end a list

  int64_t blocks_to_write = 0;

  for (int64_t idx = 0; idx < xzy.size(); idx++) {
    if (xzy(idx) == number_of_air) {
      if (!is_air_structure_void) {
        blocks_to_write++;
      }
    } else {
      blocks_to_write++;
    }
  }
  file.writeListHead("blocks", NBT::Compound, blocks_to_write);
  {
    for (int64_t y = 0; y < y_range(); y++) {
      for (int64_t z = 0; z < z_range(); z++) {
        for (int64_t x = 0; x < x_range(); x++) {
          bool should_write = false;

          if (xzy(x, z, y) == number_of_air) {
            if (!is_air_structure_void) {
              should_write = true;
            }
          } else {
            should_write = true;
          }

          if (should_write) {
            file.writeCompound("This should never be shown");
            {
              file.writeListHead("pos", NBT::Int, 3);
              {
                file.writeInt("This should never be shown", x);
                file.writeInt("This should never be shown", y);
                file.writeInt("This should never be shown", z);
              }
              file.writeInt("state", (*this)(x, y, z));
            }
            file.endCompound();
            // finish the block
          }
        }
      }
    }
    // finish writing the whole 3D array

    if (not this->entities.size()) {
      // write entities
      file.writeListHead("entities", NBT::tagType::Compound,
                         this->entities.size());
      for (auto &entity : this->entities) {
        file.writeCompound();
        {
          file.writeListHead("pos", NBT::tagType::Double, 3);
          for (double pos : entity->position()) {
            file.writeDouble("", pos);
          }
          file.writeListHead("blockPos", NBT::tagType::Int, 3);
          for (double pos : entity->position()) {
            file.writeInt("", std::floor(pos));
          }
          file.writeCompound("nbt");
          {
            auto res = entity->dump(file, this->MC_data_ver);
            if (not res) {
              file.endCompound();
              return tl::make_unexpected(
                  std::make_pair(SCL_errorFlag::EXPORT_SCHEM_HAS_INVALID_ENTITY,
                                 std::move(res.error())));
            }
          }
          file.endCompound();
        }
        file.endCompound();
      }
      // finish writing entities
    }

    switch (this->MC_major_ver) {
      case ::SCL_gameVersion::MC12:
      case ::SCL_gameVersion::MC13:
      case ::SCL_gameVersion::MC14:
      case ::SCL_gameVersion::MC15:
      case ::SCL_gameVersion::MC16:
      case ::SCL_gameVersion::MC17:
      case ::SCL_gameVersion::MC18:
      case ::SCL_gameVersion::MC19:
      case ::SCL_gameVersion::MC20:
        file.writeInt("MinecraftDataVersion", (int)this->MC_data_ver);
        break;
      default:
        std::cerr << "Wrong game version!" << std::endl;
        file.close();
        return tl::make_unexpected(std::make_pair(
            SCL_errorFlag::UNKNOWN_MAJOR_GAME_VERSION,
            fmt::format("Unknown major game version! Only 1.12 to 1.19 is "
                        "supported, but given value {}",
                        (int)this->MC_major_ver)));
    }
  }
  file.close();

  return {};
}

tl::expected<void, std::pair<SCL_errorFlag, std::string>> Schem::export_WESchem(
    std::string_view filename, const WorldEditSchem_info &info) const noexcept {
  {
    auto res = this->pre_check(filename, ".schem");
    if (not res) {
      return res;
    }
  }

  if (this->MC_major_ver <= SCL_gameVersion::MC12) {
    return tl::make_unexpected(std::make_pair(
        ::SCL_errorFlag::EXPORT_SCHEM_MC12_NOT_SUPPORTED,
        "Exporting a schematic as 1.12 WorldEdit .schematic format "
        "is not supported. Try other tools."));
  }

  NBT::NBTWriter<true> file;

  if (!file.open(filename.data())) {
    return tl::make_unexpected(
        std::make_pair(SCL_errorFlag::EXPORT_SCHEM_FAILED_TO_CREATE_FILE,
                       fmt::format("Failed to open file {}", filename)));
  }

  // write metadata
  file.writeCompound("Metadata");
  {
    file.writeInt("WEOffsetX", info.WE_offset[0]);
    file.writeInt("WEOffsetY", info.WE_offset[1]);
    file.writeInt("WEOffsetZ", info.WE_offset[2]);
    file.writeString("Name", info.schem_name_utf8.data());
    file.writeString("Author", info.author_utf8.data());
    file.writeLong("Date", info.date);

    file.writeListHead("RequiredMods", NBT::String,
                       info.required_mods_utf8.size());
    {
      for (const auto &str : info.required_mods_utf8) {
        file.writeString("", str.data());
      }
    }
    // finish list
  }  // finish compound
  file.endCompound();

  file.writeCompound("Palette");
  {
    for (int idx = 0; idx < int(block_id_list.size()); idx++) {
      file.writeInt(block_id_list[idx].c_str(), idx);
    }
  }  // finished palette
  file.endCompound();

  file.writeListHead("BlockEntities", NBT::Compound, 0);

  file.writeInt("DataVersion", (int)this->MC_data_ver);

  file.writeShort("Width", x_range());
  file.writeShort("Height", y_range());
  file.writeShort("Length", z_range());

  file.writeInt("Version", 2);

  file.writeInt("PaletteMax", block_id_list.size());

  std::vector<uint8_t> blockdata;
  ::shrink_bytes_weSchem(xzy.data(), xzy.size(), block_id_list.size(),
                         &blockdata);

  file.writeByteArrayHead("BlockData", blockdata.size());
  {
    const int8_t *data = reinterpret_cast<int8_t *>(blockdata.data());
    for (int64_t idx = 0; idx < int64_t(blockdata.size()); idx++) {
      file.writeByte("", data[idx]);
    }
  }  // end array

  file.writeIntArrayHead("Offset", 3);
  {
    file.writeInt("x", info.offset[0]);
    file.writeInt("y", info.offset[1]);
    file.writeInt("z", info.offset[2]);
  }  // end array

  file.close();
  return {};
}
