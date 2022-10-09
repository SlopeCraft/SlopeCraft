#include "ParseResourcePack.h"

#include <png.h>

bool parse_png(
    const void *const data, const int64_t length,
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> *img) {
#warning here
  return true;
}

std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                             Eigen::Dynamic, Eigen::RowMajor>>
folder_to_images(const zipped_folder &src, bool *const error,
                 std::string *const error_string) noexcept {

  std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                               Eigen::Dynamic, Eigen::RowMajor>>
      result;

  const zipped_folder *folder_ptr = &src;
  {
    auto it = src.subfolders.find("assets");

    if (it != src.subfolders.end()) {
      folder_ptr = &it->second;
      it = folder_ptr->subfolders.find("minecraft");
      if (it != folder_ptr->subfolders.end()) {

        folder_ptr = &it->second;
        it = folder_ptr->subfolders.find("textures");
        if (it != folder_ptr->subfolders.end()) {
          folder_ptr = &it->second;
          // success fully located assets/minecraft/textures
        }
      } else {
        folder_ptr = nullptr;
      }
    } else {
      folder_ptr = nullptr;
    }
  }

  // the folder is empty
  if (folder_ptr == nullptr) {
    if (error != nullptr) {
      *error = true;
    }
    if (error_string != nullptr) {
      *error_string = "";
    }
    return result;
  }

  // go through the folder
  result.reserve(folder_ptr->files.size());
  for (const auto &file : folder_ptr->files) {
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> img;
  }

  if (error != nullptr) {
    *error = true;
  }
  if (error_string != nullptr) {
    *error_string = "";
  }
  return result;
}