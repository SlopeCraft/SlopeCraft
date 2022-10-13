#include "ParseResourcePack.h"
#include "Resource_tree.h"

#include <iostream>
#include <png.h>

using std::endl;

struct read_buffer_wrapper {
  const void *data;
  int64_t offset{0};
  int64_t max_length;
};

void png_callback_read_data_from_memory(png_struct *png, png_byte *data,
                                        size_t read_length) {
  read_buffer_wrapper *const ioptr =
      reinterpret_cast<read_buffer_wrapper *>(png_get_io_ptr(png));
  const size_t can_read_bytes = ioptr->max_length - ioptr->offset;
  if (can_read_bytes < read_length) {
    png_error(png, "EOF");
    return;
  }

  memcpy(data, (char *)(ioptr->data) + ioptr->offset, read_length);
  ioptr->offset += read_length;
}

bool parse_png(
    const void *const data, const int64_t length,
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> *img) {

  png_struct *png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png == NULL) {
    ::std::cerr << "Failed to create png read struct." << endl;
    return false;
  }

  png_info *info = png_create_info_struct(png);
  if (info == NULL) {
    ::std::cerr << "Failed to create png info struct." << endl;
    png_destroy_read_struct(&png, &info, NULL);
    return false;
  }

  png_info *info_end = png_create_info_struct(png);
  if (info_end == NULL) {

    ::std::cerr << "Failed to create png info_end struct." << endl;
    png_destroy_read_struct(&png, &info, &info_end);
    return false;
  }

  // make a ioptr for libpng
  read_buffer_wrapper wrapper;
  wrapper.data = data;
  wrapper.offset = 0;
  wrapper.max_length = length;

  png_set_read_fn(png, &wrapper, png_callback_read_data_from_memory);

  png_read_info(png, info);

  uint32_t width, height;
  int bit_depth, color_type, interlace_method, compress_method, filter_method;
  bool add_alpha = false;

  png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type,
               &interlace_method, &compress_method, &filter_method);

  // cout << "\nwidth = " << width;
  // cout << "\nheight = " << height;
  // cout << "\nbit_depth = " << bit_depth;
  // cout << "\ncolor_type = " << color_type << " (";

  if (bit_depth > 8) {
    png_set_strip_16(png);
  }
  if (bit_depth < 8)
    png_set_expand(png);

  switch (color_type) {
  case PNG_COLOR_TYPE_GRAY: // fixed
    png_set_gray_to_rgb(png);
    add_alpha = true;
    // cout << "PNG_COLOR_TYPE_GRAY";
    break;
  case PNG_COLOR_TYPE_PALETTE: // fixed

    png_set_palette_to_rgb(png);
    png_set_bgr(png);
    {
      int num_trans = 0;
      png_get_tRNS(png, info, NULL, &num_trans, NULL);
      if (num_trans <= 0) {
        add_alpha = true;
      }
      // cout << "num_trans = " << num_trans << endl;
    }

    // cout << "PNG_COLOR_TYPE_PALETTE";
    break;
  case PNG_COLOR_TYPE_RGB: // fixed
    png_set_bgr(png);
    add_alpha = true;
    // cout << "PNG_COLOR_TYPE_RGB";
    break;
  case PNG_COLOR_TYPE_RGB_ALPHA: // fixed
    png_set_bgr(png);
    // cout << "PNG_COLOR_TYPE_RGB_ALPHA";
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA: // fixed
    png_set_gray_to_rgb(png);
    // png_set_swap_alpha(png);
    // cout << "PNG_COLOR_TYPE_GRAY_ALPHA";
    break;
  default:
    ::std::cerr << "Unknown color type " << color_type << endl;
    png_destroy_read_struct(&png, &info, &info_end);
    return false;
  }
  // cout << ")\n";
  //#warning here

  img->resize(height, width);

  std::vector<uint8_t *> row_ptrs;
  row_ptrs.resize(height);

  for (int r = 0; r < height; r++) {
    row_ptrs[r] = reinterpret_cast<uint8_t *>(&(*img)(r, 0));
  }

  png_read_image(png, row_ptrs.data());

  if (add_alpha) { // add alpha manually
    for (int r = 0; r < height; r++) {
      uint8_t *const data = reinterpret_cast<uint8_t *>(&(*img)(r, 0));
      for (int pixel_idx = img->cols() - 1; pixel_idx > 0; pixel_idx--) {
        const uint8_t *const data_src = data + pixel_idx * 3;
        uint8_t *const data_dest = data + pixel_idx * 4;

        for (int i = 2; i >= 0; i--) {
          data_dest[i] = data_src[i];
        }
        data_dest[3] = 0xFF;
      }
      data[3] = 0xFF;
    }
  }

  // png_destroy_info_struct(png, &info);
  png_destroy_read_struct(&png, &info, &info_end);

  return true;
}

std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                             Eigen::Dynamic, Eigen::RowMajor>>
folder_to_images(const zipped_folder &src, bool *const is_ok,
                 std::string *const error_string) noexcept {

  std::unordered_map<std::string, Eigen::Array<ARGB, Eigen::Dynamic,
                                               Eigen::Dynamic, Eigen::RowMajor>>
      result;

  const zipped_folder *folder_ptr = &src;
  folder_ptr = folder_ptr->subfolder("assets");
  if (folder_ptr != nullptr) {
    folder_ptr = folder_ptr->subfolder("minecraft");
  }
  if (folder_ptr != nullptr) {
    folder_ptr = folder_ptr->subfolder("textures");
  }
  if (folder_ptr != nullptr) {
    folder_ptr = folder_ptr->subfolder("block");
    std::cout << "Found assets/minecraft/textures/block" << endl;
  }
  // the folder is empty
  if (folder_ptr == nullptr) {
    if (is_ok != nullptr) {
      *is_ok = true;
    }
    if (error_string != nullptr) {
      *error_string = "The folder is empty";
    }
    return result;
  }

  std::cout << "Parsing png files" << endl;

  std::cout << "file num = " << folder_ptr->files.size() << endl;

  // go through the folder
  result.reserve(folder_ptr->files.size());
  for (const auto &file : folder_ptr->files) {
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> img;

    if (file.first.substr(file.first.size() - 4, 4) != ".png") {
      continue;
    }

    const bool success =
        parse_png(file.second.data(), file.second.file_size(), &img);
    if (!success) {
      ::std::cerr << "Failed to read " << file.first
                  << ", this file will be skipped." << endl;
      continue;
    }

    result.emplace(file.first, std::move(img));
  }

  if (is_ok != nullptr) {
    *is_ok = true;
  }
  if (error_string != nullptr) {
    *error_string = "";
  }
  return result;
}

Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
resize_image_nearest(const Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic,
                                        Eigen::RowMajor> &src,
                     int rows, int cols) noexcept {
  Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> result(0,
                                                                             0);
  if (rows <= 0 || cols <= 0 || src.size() <= 0)
    return result;
  result.resize(rows, cols);
  result.setZero();

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int src_row = std::min((int)std::round(float(row * src.rows()) / rows),
                             (int)src.rows() - 1);
      int src_col = std::min((int)std::round(float(col * src.cols()) / cols),
                             (int)src.cols() - 1);
      /*
std::cout << "row = " << row << ", col = " << col
<< ", src_row = " << src_row << ", src_col = " << src_col
<< endl;
*/
      result(row, col) = src(src_row, src_col);
    }
  }

  return result;
}

Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
resize_image_nearest(
    const decltype(Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic,
                                Eigen::RowMajor>()
                       .block(0, 0, 1, 1)) src_block,
    int rows, int cols) noexcept {
  Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> result(0,
                                                                             0);
  if (rows <= 0 || cols <= 0 || src_block.size() <= 0)
    return result;
  result.resize(rows, cols);
  result.setZero();

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int src_row =
          std::min((int)std::round(float(row * src_block.rows()) / rows),
                   (int)src_block.rows() - 1);
      int src_col =
          std::min((int)std::round(float(col * src_block.cols()) / cols),
                   (int)src_block.cols() - 1);
      /*
std::cout << "row = " << row << ", col = " << col
<< ", src_row = " << src_row << ", src_col = " << src_col
<< endl;
*/
      result(row, col) = src_block(src_row, src_col);
    }
  }

  return result;
}