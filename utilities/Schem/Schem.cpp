#include "Schem.h"
#include "../NBTWriter/NBTWriter.h"
#include "bit_shrink.h"

#include <filesystem>
#include <iostream>
#include <memory.h>

using namespace libSchem;

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

bool Schem::export_litematic(std::string_view filename,
                             const litematic_info &info) const noexcept {
  if (filename.empty()) {
    return false;
  }

  if (std::filesystem::path(filename).extension() != ".litematic") {
    // wrong extension
    return false;
  }

  NBT::NBTWriter<true> lite;

  if (!lite.open(filename.data())) {
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
    lite.writeLong("TimeCreated", 114514);
    lite.writeLong("TimeModified", 1919810);
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
                         this->block_types());
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
      shrink_bits(this->xzy.data(), xzy.size(), this->block_types(), &shrinked);

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
  case 12:
    lite.writeInt("MinecraftDataVersion", this->MC_version_number());
    lite.writeInt("Version", 4);
    break;
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
  case 19:
    lite.writeInt("MinecraftDataVersion", this->MC_version_number());
    lite.writeInt("Version", 5);
    break;
  default:
    std::cerr << "Wrong game version!" << std::endl;
    lite.close();
    return false;
    break;
  }
  lite.close();
  return true;
}

bool Schem::export_structure(std::string_view filename,
                             const bool is_air_structure_void) const noexcept {

  if (filename.empty()) {
    return false;
  }

  if (std::filesystem::path(filename).extension() != ".nbt") {
    // wrong extension
    return false;
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
    return false;
  }

  /*
  reportWorkingStatue(wind, workStatues::writingMetaInfo);
  progressRangeSet(wind, 0, 100 + Build.size(), 0);
  */

  NBT::NBTWriter<true> file;
  if (!file.open(filename.data())) {
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
  file.writeListHead("palette", NBT::Compound, block_types());
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
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
      file.writeInt("MinecraftDataVersion", this->MC_ver_number);
      break;
    default:
      std::cerr << "Wrong game version!" << std::endl;
      file.close();
      return false;
    }
  }
  file.close();

  return true;
}

bool Schem::export_WESchem(std::string_view filename,
                           const WorldEditSchem_info &info) const noexcept {

  if (filename.empty()) {
    return false;
  }

  if (std::filesystem::path(filename).extension() != ".schem") {
    // wrong extension
    return false;
  }

  NBT::NBTWriter<true> file;

  if (!file.open(filename.data())) {
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
    file.writeLong("Date", std::time(nullptr) * 1000);

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
  return true;
}