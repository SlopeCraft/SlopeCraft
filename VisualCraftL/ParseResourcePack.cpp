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

  png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type,
               &interlace_method, &compress_method, &filter_method);

  cout << "\nwidth = " << width;
  cout << "\nheight = " << height;
  cout << "\nbit_depth = " << bit_depth;
  cout << "\ncolor_type = " << color_type << " (";

  switch (color_type) {
  case PNG_COLOR_TYPE_GRAY:
    cout << "PNG_COLOR_TYPE_GRAY";
    break;
  case PNG_COLOR_TYPE_PALETTE:
    cout << "PNG_COLOR_TYPE_PALETTE";
    break;
  case PNG_COLOR_TYPE_RGB:
    cout << "PNG_COLOR_TYPE_RGB";
    break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
    cout << "PNG_COLOR_TYPE_RGB_ALPHA";
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA:
    cout << "PNG_COLOR_TYPE_GRAY_ALPHA";
    break;
  default:
    cout << "uknown color type";
  }
  cout << ")\n";
#warning here
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
