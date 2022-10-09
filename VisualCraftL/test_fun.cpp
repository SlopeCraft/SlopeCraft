#include "ParseResourcePack.h"
#include "Resource_tree.h"
#include "VisualCraftL.h"

#include <filesystem>
#include <iostream>
#include <png.h>

#include <stdio.h>

using std::cout, std::endl;

void display_folder(const zipped_folder &folder, const int offset = 0) {
  std::string spaces;
  spaces.resize(offset);
  for (char &ch : spaces) {
    ch = ' ';
  }
  cout << "{\n";

  for (const auto &file : folder.files) {
    cout << spaces << "  " << file.first << " : " << file.second.file_size()
         << " bytes\n";
  }

  for (const auto &subfolder : folder.subfolders) {

    cout << spaces << "  " << subfolder.first << " : ";
    display_folder(subfolder.second, offset + 2);
  }
  cout << spaces << "}\n";
}

VCL_EXPORT void test_VCL() {
  /*
  zipped_folder folder = zipped_folder::from_zip("test.zip");

  // display_folder(folder);
  // cout << endl;

  const zipped_file &png_to_read = folder.subfolders["assets"]
                                       .subfolders["minecraft"]
                                       .subfolders["textures"]
                                       .subfolders["block"]
                                       .files["weathered_copper_waxed.png"];

  cout << "size of png : " << png_to_read.file_size() << endl;
  */

  FILE *fp = NULL;
  ::fopen_s(&fp, "pack.png", "rb");

  size_t filesize = std::filesystem::file_size("pack.png");
  cout << "size of png file : " << filesize << endl;
  void *const buffer = malloc(filesize);
  fread(buffer, 1, filesize, fp);

  parse_png(buffer, filesize, nullptr);

  free(buffer);
}
