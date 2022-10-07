#include "Resource_tree.h"

#include <filesystem>
#include <iostream>
#include <zip.h>

//#include "VisualCraftL.h"

using std::cout, std::endl;

auto split_by_slash(std::string_view str) noexcept {
  std::vector<std::basic_string_view<char>> result;

  int64_t substr_start = 0;

  for (int64_t cur_slash = 0; cur_slash < str.length(); cur_slash++) {
    if (str.at(cur_slash) == '/') {
      if (cur_slash != 0) {
        result.emplace_back(str.substr(substr_start, cur_slash - substr_start));
      }
      substr_start = cur_slash += 1;
    }
  }
  // if (str.back() != '/')
  result.emplace_back(str.substr(substr_start));

  return result;
}

zipped_folder zipped_folder::from_zip(std::string_view zipname) noexcept {
  zipped_folder result;
  {
    std::filesystem::path path = zipname;
    if (zipname.empty()) {
      return result;
    }

    if (!std::filesystem::is_regular_file(path)) {
      return result;
    }

    if (path.extension() != ".zip") {
      return result;
    }
  }
  int errorcode;
  zip_t *const zip = zip_open(zipname.data(), ZIP_RDONLY, &errorcode);

  if (zip == NULL) {
    cout << "Failed to open zip file : " << zipname
         << ", error code = " << errorcode << endl;
    return result;
  }

  const int64_t entry_num = zip_get_num_entries(zip, ZIP_FL_UNCHANGED);

  for (int64_t entry_idx = 0; entry_idx < entry_num; entry_idx++) {
    auto splited =
        split_by_slash(::zip_get_name(zip, entry_idx, ZIP_FL_ENC_GUESS));

    zipped_folder *curfolder = &result;
    zipped_file *destfile = nullptr;
    for (int idx = 0; idx + 1 < splited.size(); idx++) {
      if (idx + 1 < splited.size()) {
        // is folder name
        curfolder = &curfolder->subfolders[std::string(splited.at(idx))];
      }
    }

    if (std::string_view(::zip_get_name(zip, entry_idx, ZIP_FL_ENC_GUESS))
            .back() != '/') {
      auto ret = curfolder->files.emplace(splited.back(), zipped_file());
      destfile = &ret.first->second;
    }

    if (destfile == nullptr) {
      continue;
    }

    zip_stat_t stat;
    zip_stat_index(zip, entry_idx, ZIP_FL_UNCHANGED, &stat);

    destfile->__data.resize(stat.size);
    zip_file_t *const zfile = zip_fopen_index(zip, entry_idx, ZIP_FL_UNCHANGED);
    if (zfile == NULL) {
      cout << "Failed to open file in zip. index : " << entry_idx
           << "file name : " << ::zip_get_name(zip, entry_idx, ZIP_FL_ENC_GUESS)
           << endl;
      continue;
    }

    zip_fread(zfile, destfile->__data.data(), stat.size);
  }

  return result;
}