#include "ParseResourcePack.h"
#include "Resource_tree.h"
#include "VisualCraftL.h"
#include "VisualCraftL/ParseResourcePack.h"

#include <filesystem>
#include <iostream>
#include <png.h>

#include <stdio.h>

using std::cout, std::endl;
void test_VCL_single_image();
void test_VCL_full_zip();
void test_VCL_single_img_in_zip();

void test_VCL_block_model_full_block();

VCL_EXPORT void test_VCL() {
  // test_VCL_full_zip();
  // test_VCL_single_image();
  //  test_VCL_single_img_in_zip();
  test_VCL_block_model_full_block();
}

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
  fp=fopen(filename, "wb");

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

void test_VCL_single_image() {
  const char *filename = "anvil.png";

  FILE *fp = NULL;
  fp=::fopen(filename, "rb");

  size_t filesize = std::filesystem::file_size(filename);
  cout << "size of png file : " << filesize << endl;
  void *const buffer = malloc(filesize);
  fread(buffer, 1, filesize, fp);

  Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> img;

  parse_png(buffer, filesize, &img);
  img = resize_image_nearest(img, 60, 32);
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

void test_VCL_full_zip() {

  zipped_folder folder = zipped_folder::from_zip("Vanilla_1_19_2.zip");

  bool is_ok;
  std::string error_message;

  std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                               Eigen::Dynamic, Eigen::RowMajor>>
      images = folder_to_images(folder, &is_ok, &error_message);

  cout << error_message << endl;

  cout << "size of images = " << images.size() << endl;

  cout << "Exporting images again..." << endl;

  for (const auto &file : images) {
    rewrite_png(("test/" + file.first).c_str(), file.second);
  }

  cout << "Finished." << endl;
}

void test_VCL_single_img_in_zip() {

  zipped_folder folder = zipped_folder::from_zip("test.zip");
  zipped_folder *const block_dir = folder.subfolder("assets")
                                       ->subfolder("minecraft")
                                       ->subfolder("textures")
                                       ->subfolder("block");
  const zipped_file &file = block_dir->files.find("anvil.png")->second;
  cout << "Found anvil.png" << endl;
  cout << "size of png file : " << file.file_size() << endl;

  Eigen::Array<ARGB, -1, -1, Eigen::RowMajor> img;

  const bool success = parse_png(file.data(), file.file_size(), &img);

  cout << "success = " << success << endl;

  printf("The first 4 pixel in hex :\n");
  for (int i = 0; i < 4 * 4; i++) {
    printf("%hX,", ((uint8_t *)img.data())[i]);
  }
  printf("\n\n");
  rewrite_png("rewrite_anvil.png", img);
}

void export_projection_images(block_model::model m,
                              const std::string filenameprefix) {
  rewrite_png((filenameprefix + "_up.png").data(),
              m.projection_image(block_model::face_idx::face_up));
  /*
rewrite_png((filenameprefix + "_down.png").data(),
  m.projection_image(block_model::face_idx::face_down));
rewrite_png((filenameprefix + "_east.png").data(),
  m.projection_image(block_model::face_idx::face_east));
rewrite_png((filenameprefix + "_west.png").data(),
  m.projection_image(block_model::face_idx::face_west));
rewrite_png((filenameprefix + "_north.png").data(),
  m.projection_image(block_model::face_idx::face_north));
rewrite_png((filenameprefix + "_south.png").data(),
  m.projection_image(block_model::face_idx::face_south));
  */
}

void test_VCL_block_model_full_block() {
  zipped_folder vanilla = zipped_folder::from_zip("Vanilla_1_19_2.zip");

  Eigen::Array<ARGB, -1, -1, Eigen::RowMajor> texture;

  const auto &images = vanilla.folder_at("assets")
                           ->subfolder("minecraft")
                           ->subfolder("textures")
                           ->subfolder("block")
                           ->files;
  const bool success =
      parse_png(images.at("smooth_stone.png").data(),
                images.at("smooth_stone.png").file_size(), &texture);

  if (!success) {
    cout << "Failed to parse smooth_stone.png" << endl;
    return;
  }

  block_model::model smooth_stone;
  {
    block_model::element element;
    element._from = {0, 0, 0};
    element._to = {16, 16, 16};
    block_model::face_t face;
    face.texture = &texture;

    element.faces.fill(face);

    smooth_stone.elements.emplace_back(element);
  }

  export_projection_images(smooth_stone, "test_block_model/smooth_stone");

  cout << "success" << endl;
}