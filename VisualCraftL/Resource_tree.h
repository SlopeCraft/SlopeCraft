#ifndef SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H
#define SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

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

  inline const zipped_folder *
  subfolder(std::string_view fdname) const noexcept {
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

  static zipped_folder from_zip(std::string_view zipname,
                                bool *const ok = nullptr) noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFTL_RESOURCE_TREE_H