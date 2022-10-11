#include "ParseResourcePack.h"
#include "Resource_tree.h"

#include <iostream>
#include <png.h>

using std::cout, std::endl;

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
    cout << "Failed to create png read struct." << endl;
    return false;
  }

  png_info *info = png_create_info_struct(png);
  if (info == NULL) {
    cout << "Failed to create png info struct." << endl;
    png_destroy_read_struct(&png, &info, NULL);
    return false;
  }

  png_info *info_end = png_create_info_struct(png);
  if (info_end == NULL) {

    cout << "Failed to create png info_end struct." << endl;
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

  cout << "\nwidth = " << width;
  cout << "\nheight = " << height;
  cout << "\nbit_depth = " << bit_depth;
  cout << "\ncolor_type = " << color_type << " (";

  if (bit_depth > 8)
    png_set_strip_16(png);
  if (bit_depth < 8)
    png_set_expand(png);

  switch (color_type) {
  case PNG_COLOR_TYPE_GRAY: // fixed
    png_set_gray_to_rgb(png);
    add_alpha = true;
    cout << "PNG_COLOR_TYPE_GRAY";
    break;
  case PNG_COLOR_TYPE_PALETTE: // fixed

    png_set_palette_to_rgb(png);
    png_set_bgr(png);
    int num_trans;
    png_get_tRNS(png, info, NULL, &num_trans, NULL);
    if (num_trans <= 0) {
      add_alpha = true;
    }
    cout << "num_trans = " << num_trans << endl;
    cout << "PNG_COLOR_TYPE_PALETTE";
    break;
  case PNG_COLOR_TYPE_RGB: // fixed
    png_set_bgr(png);
    add_alpha = true;
    cout << "PNG_COLOR_TYPE_RGB";
    break;
  case PNG_COLOR_TYPE_RGB_ALPHA: // fixed
    png_set_bgr(png);
    cout << "PNG_COLOR_TYPE_RGB_ALPHA";
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA: // fixed
    png_set_gray_to_rgb(png);
    // png_set_swap_alpha(png);
    cout << "PNG_COLOR_TYPE_GRAY_ALPHA";
    break;
  default:
    cout << "uknown color type";
  }
  cout << ")\n";
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
