#include "Resource_tree.h"

#include <filesystem>
#include <iostream>
#include <zip.h>

using std::cout, std::endl;

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

  return result;
}