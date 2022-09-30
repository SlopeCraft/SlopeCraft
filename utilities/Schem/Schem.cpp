/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#include "Schem.h"
#include "../NBTWriter/NBTWriter.h"
#include "bit_shrink.h"

#include <ctime>
#include <filesystem>
#include <iostream>
#include <memory.h>

using namespace libSchem;

WorldEditSchem_info::WorldEditSchem_info() : date(std::time(nullptr) * 1000) {}

litematic_info::litematic_info()
    : time_created(std::time(nullptr) * 1000),
      time_modified(std::time(nullptr) * 1000) {}

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
  std::vector<__mushroom_type> is_mushroom_LUT;
  is_mushroom_LUT.reserve(this->palette_size() + 64 * 3);
#warning not finished yet
}

bool Schem::export_litematic(std::string_view filename,
                             const litematic_info &info,
                             SCL_errorFlag *const error_flag,
                             std::string *const error_str) const noexcept {

  if (std::filesystem::path(filename).extension() != ".litematic") {
    // wrong extension

    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::EXPORT_SCHEM_WRONG_EXTENSION;
    }
    if (error_str != nullptr) {
      *error_str = "The filename externsion must be \".litematic\".";
    }
    return false;
  }
  // check for invalid blocks
  {
    std::array<int64_t, 3> pos;
    if (this->have_invalid_block(&pos[0], &pos[1], &pos[2])) {

      if (error_flag != nullptr) {
        *error_flag = SCL_errorFlag::EXPORT_SCHEM_HAS_INVALID_BLOCKS;
      }
      if (error_str != nullptr) {
        *error_str = "The first invalid block is at x=";
        *error_str += std::to_string(pos[0]) + ", y=";
        *error_str += std::to_string(pos[1]) + ", z=";
        *error_str += std::to_string(pos[2]);
      }
      return false;
    }
  }

  NBT::NBTWriter<true> lite;

  if (!lite.open(filename.data())) {
    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::EXPORT_SCHEM_FAILED_TO_CREATE_FILE;
    }
    if (error_str != nullptr) {
      *error_str = "Failed to open file : ";
      *error_str += filename;
    }
    return false;
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

      // reportWorkingStatue(wind, workStatues::writingBlockPalette);
      // write block palette
      lite.writeListHead("BlockStatePalette", NBT::Compound,
                         this->palette_size());
      {
        std::string pure_block_id;
        pure_block_id.reserve(1024);
        memset(pure_block_id.data(), 0, pure_block_id.capacity());
        std::vector<std::pair<std::string, std::string>> properties;
        properties.reserve(64);

        for (const char *const block_string : this->block_id_list) {
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
        for (int64_t idx = 0; idx < shrinked.size(); idx++) {
          lite.writeSingleTag<int64_t, true>(
              NBT::Long, "id", reinterpret_cast<int64_t &>(shrinked[idx]));
        }
      }
      // progressAdd(wind, size3D[0]);
    }
    lite.endCompound(); // end current region
  }
  lite.endCompound(); // end all regions

  switch (this->MC_major_ver_number) {
  case ::SCL_gameVersion::MC12:
    lite.writeInt("MinecraftDataVersion", this->MC_version_number());
    lite.writeInt("Version", 4);
    break;
  case ::SCL_gameVersion::MC13:
  case ::SCL_gameVersion::MC14:
  case ::SCL_gameVersion::MC15:
  case ::SCL_gameVersion::MC16:
  case ::SCL_gameVersion::MC17:
  case ::SCL_gameVersion::MC18:
  case ::SCL_gameVersion::MC19:
    lite.writeInt("MinecraftDataVersion", this->MC_version_number());
    lite.writeInt("Version", 5);
    break;
  default:
    std::cerr << "Wrong game version!" << std::endl;
    lite.close();
    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::UNKNOWN_MAJOR_GAME_VERSION;
    }
    if (error_str != nullptr) {
      *error_str = "Unknown major game version! Only 1.12 to 1.19 is "
                   "supported, but given value " +
                   std::to_string(this->MC_major_ver_number);
    }
    return false;
  }
  lite.close();

  if (error_flag != nullptr) {
    *error_flag = SCL_errorFlag::NO_ERROR_OCCUR;
  }
  if (error_str != nullptr) {
    *error_str = "";
  }
  return true;
}

bool Schem::export_structure(std::string_view filename,
                             const bool is_air_structure_void,
                             SCL_errorFlag *const error_flag,
                             std::string *const error_str) const noexcept {

  if (std::filesystem::path(filename).extension() != ".nbt") {
    // wrong extension

    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::EXPORT_SCHEM_WRONG_EXTENSION;
    }
    if (error_str != nullptr) {
      *error_str = "The filename externsion must be \".nbt\".";
    }
    return false;
  }

  // check for invalid blocks
  {
    std::array<int64_t, 3> pos;
    if (this->have_invalid_block(&pos[0], &pos[1], &pos[2])) {

      if (error_flag != nullptr) {
        *error_flag = SCL_errorFlag::EXPORT_SCHEM_HAS_INVALID_BLOCKS;
      }
      if (error_str != nullptr) {
        *error_str = "The first invalid block is at x=";
        *error_str += std::to_string(pos[0]) + ", y=";
        *error_str += std::to_string(pos[1]) + ", z=";
        *error_str += std::to_string(pos[2]);
      }
      return false;
    }
  }

  uint16_t number_of_air;
  for (number_of_air = 0; number_of_air < this->block_id_list.size();
       number_of_air++) {
    if (0 == std::strcmp("minecraft:air", block_id_list[number_of_air])) {
      break;
    }
  }

  if ((!is_air_structure_void) &&
      (number_of_air >= this->block_id_list.size())) {
    std::cerr << "You assigned is_air_structure_void=false, but there is no "
                 "minecraft:air in your block palette."
              << std::endl;

    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::EXPORT_SCHEM_STRUCTURE_REQUIRES_AIR;
    }
    if (error_str != nullptr) {
      *error_str = "You assigned is_air_structure_void=false, but there is no "
                   "minecraft:air in your block palette.";
    }
    return false;
  }

  /*
  reportWorkingStatue(wind, workStatues::writingMetaInfo);
  progressRangeSet(wind, 0, 100 + Build.size(), 0);
  */

  NBT::NBTWriter<true> file;
  if (!file.open(filename.data())) {
    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::EXPORT_SCHEM_FAILED_TO_CREATE_FILE;
    }
    if (error_str != nullptr) {
      *error_str = "Failed to open file : ";
      *error_str += filename;
    }
    return false;
  }

  file.writeListHead("entities", NBT::Byte, 0);
  file.writeListHead("size", NBT::Int, 3);
  {
    file.writeInt("This should never be shown", x_range());
    file.writeInt("This should never be shown", y_range());
    file.writeInt("This should never be shown", z_range());
  }
  // reportWorkingStatue(wind, workStatues::writingBlockPalette);
  file.writeListHead("palette", NBT::Compound, palette_size());
  {
    std::string pure_block_id;
    pure_block_id.reserve(1024);
    memset(pure_block_id.data(), 0, pure_block_id.capacity());
    std::vector<std::pair<std::string, std::string>> properties;
    properties.reserve(64);
    for (const char *const block_string : this->block_id_list) {
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
    // finish writting the whole 3D array

    switch (this->MC_major_ver_number) {
    case ::SCL_gameVersion::MC12:
    case ::SCL_gameVersion::MC13:
    case ::SCL_gameVersion::MC14:
    case ::SCL_gameVersion::MC15:
    case ::SCL_gameVersion::MC16:
    case ::SCL_gameVersion::MC17:
    case ::SCL_gameVersion::MC18:
    case ::SCL_gameVersion::MC19:
      file.writeInt("MinecraftDataVersion", this->MC_ver_number);
      break;
    default:
      std::cerr << "Wrong game version!" << std::endl;
      file.close();
      if (error_flag != nullptr) {
        *error_flag = SCL_errorFlag::UNKNOWN_MAJOR_GAME_VERSION;
      }
      if (error_str != nullptr) {
        *error_str = "Unknown major game version! Only 1.12 to 1.19 is "
                     "supported, but given value " +
                     std::to_string(this->MC_major_ver_number);
      }
      return false;
    }
  }
  file.close();

  if (error_flag != nullptr) {
    *error_flag = SCL_errorFlag::NO_ERROR_OCCUR;
  }
  if (error_str != nullptr) {
    *error_str = "";
  }
  return true;
}

bool Schem::export_WESchem(std::string_view filename,
                           const WorldEditSchem_info &info,
                           SCL_errorFlag *const error_flag,
                           std::string *const error_str) const noexcept {

  if (std::filesystem::path(filename).extension() != ".schem") {
    // wrong extension

    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::EXPORT_SCHEM_WRONG_EXTENSION;
    }
    if (error_str != nullptr) {
      *error_str = "The filename externsion must be \".schem\".";
    }
    return false;
  }

  // check for invalid blocks
  {
    std::array<int64_t, 3> pos;
    if (this->have_invalid_block(&pos[0], &pos[1], &pos[2])) {

      if (error_flag != nullptr) {
        *error_flag = SCL_errorFlag::EXPORT_SCHEM_HAS_INVALID_BLOCKS;
      }
      if (error_str != nullptr) {
        *error_str = "The first invalid block is at x=";
        *error_str += std::to_string(pos[0]) + ", y=";
        *error_str += std::to_string(pos[1]) + ", z=";
        *error_str += std::to_string(pos[2]);
      }
      return false;
    }
  }

  if (this->MC_major_ver_number <= MC12) {
    if (error_flag != nullptr) {
      *error_flag = ::SCL_errorFlag::EXPORT_SCHEM_MC12_NOT_SUPPORTED;
    }
    if (error_str != nullptr) {
      *error_str = "Exporting a schematic as 1.12 WorldEdit .schematic format "
                   "is not supported. Try other tools.";
    }
    return false;
  }

  NBT::NBTWriter<true> file;

  if (!file.open(filename.data())) {
    if (error_flag != nullptr) {
      *error_flag = SCL_errorFlag::EXPORT_SCHEM_FAILED_TO_CREATE_FILE;
    }
    if (error_str != nullptr) {
      *error_str = "Failed to open file : ";
      *error_str += filename;
    }
    return false;
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
  } // finish compound
  file.endCompound();

  file.writeCompound("Palette");
  {
    for (int idx = 0; idx < block_id_list.size(); idx++) {
      file.writeInt(block_id_list[idx], idx);
    }
  } // finished palette
  file.endCompound();

  file.writeListHead("BlockEntities", NBT::Compound, 0);

  file.writeInt("DataVersion", this->MC_ver_number);

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
    for (int64_t idx = 0; idx < blockdata.size(); idx++) {
      file.writeByte("", data[idx]);
    }
  } // end array

  file.writeIntArrayHead("Offset", 3);
  {
    file.writeInt("x", info.offset[0]);
    file.writeInt("y", info.offset[1]);
    file.writeInt("z", info.offset[2]);
  } // end array

  file.close();
  if (error_flag != nullptr) {
    *error_flag = SCL_errorFlag::NO_ERROR_OCCUR;
  }
  if (error_str != nullptr) {
    *error_str = "";
  }
  return true;
}
