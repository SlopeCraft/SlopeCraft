/*
 Copyright © 2021-2026  TokiNoBug
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

#include "processMapFiles.h"

#include <filesystem>
#include <fstream>
#include <vector>
#include <zlib.h>

#include <iostream>

using std::cout, std::endl;

bool uncompress_map_file(const char *filename, std::vector<uint8_t> *const dest,
                         std::string *const error_info);

const uint8_t *find_color_begin(const std::vector<uint8_t> &inflated,
                                std::string *const error_info);

bool uncompress_map_file(const char *filename, std::vector<uint8_t> *const dest,
                         std::string *const error_info) {
  dest->clear();

  const size_t source_size = std::filesystem::file_size(filename);
  dest->reserve(4 * source_size);

  gzFile gz_file = ::gzopen(filename, "rb");

  if (gz_file == nullptr) {
    if (error_info != nullptr) {
      *error_info = "Failed to open map data file ";
      *error_info += filename;
      *error_info += ", the file may be invalid.";
      *error_info +=
          "\nThis error may also be caused by non-English "
          "characters in the filename of path.";
    }
    return false;
  }

  while (true) {
    dest->emplace_back();
    if (::gzfread(&dest->back(), 1, 1, gz_file) <= 0) {
      dest->pop_back();
      break;
    }
  }

  ::gzclose(gz_file);
  dest->shrink_to_fit();

  return true;
}

const uint8_t *find_color_begin(const std::vector<uint8_t> &inflated,
                                std::string *const error_info) {
  if (inflated.size() <= 128 * 128 * sizeof(char)) {
    if (error_info != nullptr) {
      *error_info =
          "The inflated file content is invalid. The file size is " +
          std::to_string(inflated.size()) +
          " which is less than 128*128, it can't contain the map content.";
    }
    return nullptr;
  }

  // std::vector<const uint8_t *> possiblePtrs;

  constexpr uint8_t feature[] = {0x07, 0x00, 0x06, 0x63, 0x6F, 0x6C, 0x6F,
                                 0x72, 0x73, 0x00, 0x00, 0x40, 0x00};
  constexpr size_t feature_length = sizeof(feature) / sizeof(uint8_t);
  for (const uint8_t *ptr = inflated.data();
       ptr != inflated.data() + inflated.size(); ptr++) {
    if (ptr[0] != feature[0]) {
      continue;
    }
    // cout<<"found the header of feature"<<endl;
    bool same = true;
    for (size_t idx = 1; idx < feature_length; idx++) {
      same = same && (ptr[idx] == feature[idx]);
    }

    if (same) {
      const size_t left_bytes =
          inflated.data() + inflated.size() - (ptr + feature_length);
      if (left_bytes >= 128 * 128 * sizeof(uint8_t)) {
        // cout<<"found feature, but left bytes are too few"<<endl;
        return ptr + feature_length;
      }
    }
  }

  if (error_info != nullptr) {
    *error_info =
        "Failed to find map data array named color. The map data "
        "file may be invalid.";
  }

  // cout<<"failed to find feature"<<endl;

  return nullptr;
}

bool process_map_file(
    const char *filename,
    Eigen::Array<uint8_t, 128, 128, Eigen::RowMajor> *const dest,
    std::string *const error_info) {
  if (filename == nullptr || strlen(filename) <= 0) {
    if (error_info != nullptr) *error_info = "Invalid input : filename";
    return false;
  }

  if (dest == nullptr) {
    if (error_info != nullptr) *error_info = "Invalid input : dest";
    return false;
  }

  if ((!std::filesystem::exists(filename)) ||
      (!std::filesystem::is_regular_file(filename))) {
    if (error_info != nullptr)
      *error_info = std::string("File ") + filename +
                    " doesn't exist, otherwise it is not a regular file.";
    return false;
  }

  std::vector<uint8_t> inflated;

  if (!uncompress_map_file(filename, &inflated, error_info)) {
    return false;
  }

  const uint8_t *const color_ptr = find_color_begin(inflated, error_info);

  if (color_ptr == nullptr) {
    return false;
  }

  memcpy(dest->data(), color_ptr, 128 * 128 * sizeof(uint8_t));

  return true;
}
