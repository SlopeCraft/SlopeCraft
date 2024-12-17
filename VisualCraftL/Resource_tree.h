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

#ifndef SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H
#define SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>
#include <span>
#include <cstdint>
// forward declaration for zip and zip_t defined in zip.h
struct zip;

class zipped_file;
class zipped_folder;

class zipped_file {
 private:
  std::vector<uint8_t> __data;

 public:
  friend class zipped_folder;
  inline int64_t file_size() const noexcept { return __data.size(); }

  inline uint8_t *data() noexcept { return __data.data(); }

  inline const uint8_t *data() const noexcept { return __data.data(); }

  inline void append_0_for_c_str() noexcept { this->__data.push_back(0); }
};

class zipped_folder {
 public:
  std::unordered_map<std::string, zipped_folder> subfolders;
  std::unordered_map<std::string, zipped_file> files;

  inline zipped_folder *folder_at(const char *const str) noexcept {
    auto it = subfolders.find(str);
    if (it != subfolders.end()) {
      return &it->second;
    }
    return nullptr;
  }

  inline zipped_folder *subfolder(std::string_view fdname) noexcept {
    auto it = subfolders.find(std::string(fdname));
    if (it == subfolders.end()) {
      return nullptr;
    } else {
      return &it->second;
    }
  }

  inline const zipped_folder *subfolder(
      std::string_view fdname) const noexcept {
    auto it = subfolders.find(std::string(fdname));
    if (it == subfolders.end()) {
      return nullptr;
    } else {
      return &it->second;
    }
  }

  //*this is additional, and source_base is basical resource pack.
  // so *this have higher priority than source_base
  void merge_from_base(const zipped_folder &source_base) noexcept;

  void merge_from_base(zipped_folder &&source_base) noexcept;

  static std::optional<zipped_folder> from_zip(std::string_view zipname,
                                               zip *archive) noexcept;

  static std::optional<zipped_folder> from_zip(
      std::string_view zipname) noexcept;

  static std::optional<zipped_folder> from_zip(
      std::string_view zipname,
      const std::span<const uint8_t> zip_content) noexcept;
};

#endif  // SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H