#ifndef SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H
#define SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H

#include <string>
#include <unordered_map>
#include <vector>

class zipped_file;
class zipped_folder;

class zipped_file {
private:
  std::vector<uint8_t> __data;

public:
  inline int64_t file_size() const noexcept { return __data.size(); }

  inline uint8_t *data() noexcept { return __data.data(); }

  inline const uint8_t *data() const noexcept { return __data.data(); }
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

  static zipped_folder from_zip(std::string_view zipname) noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H