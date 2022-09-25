#include "Schem.h"
#include "../NBTWriter/NBTWriter.h"
#include "bit_shrink.h"

#include <iostream>

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

bool Schem::export_litematic(const std::string_view filename,
                             const litematic_info &info) const noexcept {
  if (filename.empty()) {
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
        std::vector<std::pair<std::string, std::string>> properties;

        for (const char *const block_string : this->block_id_list) {
          process_block_id(block_string, &pure_block_id, &properties);
        }

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

      lite.writeListHead("Entities", NBT::Compound, 0);
      lite.writeListHead("PendingBlockTicks", NBT::Compound, 0);
      lite.writeListHead("PendingFluidTiccks", NBT::Compound, 0);
      lite.writeListHead("TileEntities", NBT::Compound, 0);

      // write 3D
      std::vector<uint64_t> shrinked;
      shrink_bits(this->xzy.data(), xzy.size(), this->block_types(), &shrinked);

      lite.writeLongArrayHead("BlockStates", shrinked.size());
      {
        for (int64_t idx = 0; idx < this->xzy.size(); idx++) {
          lite.writeSingleTag<int64_t, false>(
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