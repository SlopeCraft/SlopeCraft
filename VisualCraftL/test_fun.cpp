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

bool rewrite_png(const char *const filename,
                 const Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic,
                                    Eigen::RowMajor> &img) {

  png_struct *png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png == NULL) {
    return false;
  }

  png_info *info = png_create_info_struct(png);

  if (info == NULL) {
    png_destroy_write_struct(&png, NULL);
    return false;
  }

  FILE *fp = NULL;
  fopen_s(&fp, filename, "wb");

  if (fp == NULL) {
    png_destroy_write_struct(&png, &info);
    return false;
  }

  png_init_io(png, fp);

  png_set_IHDR(png, info, img.cols(), img.rows(), 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_set_bgr(png);
  png_write_info(png, info);

  for (int r = 0; r < img.rows(); r++) {
    const uint8_t *const rowptr = reinterpret_cast<const uint8_t *>(&img(r, 0));
    png_write_row(png, rowptr);
  }

  png_write_end(png, info);

  png_destroy_write_struct(&png, &info);

  fclose(fp);

  return true;
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

  const char *filename = "src_idx8_noaplha.png";

  FILE *fp = NULL;
  ::fopen_s(&fp, filename, "rb");

  size_t filesize = std::filesystem::file_size(filename);
  cout << "size of png file : " << filesize << endl;
  void *const buffer = malloc(filesize);
  fread(buffer, 1, filesize, fp);

  Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> img;

  parse_png(buffer, filesize, &img);
  /*
  printf("The 1st pixel is : %#X\n", img(0));
  printf("The 2nd pixel is : %#X\n", img(1));
  printf("The 3rd pixel is : %#X\n", img(2));
  */

  printf("The first 4 pixel in hex :\n");
  for (int i = 0; i < 4 * 4; i++) {
    printf("%hX,", ((uint8_t *)img.data())[i]);
  }
  printf("\n\n");

  rewrite_png("test_rewrite.png", img);

  free(buffer);
}
