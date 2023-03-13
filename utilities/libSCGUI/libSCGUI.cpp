/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "libSCGUI.h"
#include "libAbstractGUI.h"
#include "libSCGUI/libSCGUI.h"

// #include <Eigen/Dense>

using namespace libSCGUI;
using namespace libAbstractGUI;
using std::string, std::vector, std::string_view;

void SCGUI::on_load_image_clicked() noexcept {

  const vector<string> image_files = callback_get_open_filenames(
      "Select one or multiple images", prev_dir, "*.png;;*.jpg;;*.jpeg;;*.bmp");

  if (image_files.size() <= 0) {
    return;
  }

  if (image_files.size() == 1) {
    string_view filename = image_files.front();

    if (filename.size() <= 0) {
      return;
    }
    int img_rows = 0, img_cols = 0;
    callback_read_image_from_file(filename, nullptr, 0, &img_rows, &img_cols,
                                  nullptr);

    if (img_rows <= 0 || img_cols <= 0) {
      // the image seems to be broken.
      return;
    }
    bool is_row_major = false;
    uint32_t *buffer =
        (uint32_t *)malloc(sizeof(uint32_t) * img_rows * img_cols);
    callback_read_image_from_file(filename, buffer,
                                  sizeof(uint32_t) * img_rows * img_cols,
                                  &img_rows, &img_cols, &is_row_major);
    // change the storage order to col-major
    if (is_row_major) {
      for (int r = 0; r < img_rows; r++) {
        for (int c = 0; c < img_cols; c++) {
          std::swap(buffer[r * img_cols + c], buffer[c * img_rows + r]);
        }
      }
    }

    kernel->setRawImage(buffer, img_rows, img_cols);
    callback_set_image(label_show_raw_image, buffer, img_rows, img_cols,
                       is_row_major);
    callback_set_image(label_show_image_before_convert, buffer, img_rows,
                       img_cols, is_row_major);
    free(buffer);
  }

#warning batch op not finished yet.
}

void cb_progress_range_set(void *libAbstractGUI_progress_bar_ptr, int min,
                           int max, int val) {
  callback_set_progress_bar(
      *reinterpret_cast<progress_bar *>(libAbstractGUI_progress_bar_ptr), min,
      max, val);
}
void cb_progress_add(void *libAbstractGUI_progress_bar_ptr, int delta) {
  int min, max, val;
  callback_get_progress_bar(
      *reinterpret_cast<progress_bar *>(libAbstractGUI_progress_bar_ptr), &min,
      &max, &val);
  callback_set_progress_bar(
      *reinterpret_cast<progress_bar *>(libAbstractGUI_progress_bar_ptr), min,
      max, val + delta);
}

void SCGUI::on_convert_clicked(const ::SCL_convertAlgo algo,
                               const bool dither) noexcept {
  callback_set_freezed(main_window, true);
  kernel->setWindPtr(&progressbar_convert_image);
  kernel->setProgressRangeSet(cb_progress_range_set);
  kernel->setProgressAdd(cb_progress_add);

  kernel->convert(algo, dither);
  refreash_converted_image();
  callback_set_freezed(main_window, false);
}

void SCGUI::refreash_raw_image() noexcept {}

void SCGUI::refreash_converted_image() noexcept {
  int rows = 0, cols = 0;
  kernel->getConvertedImage(&rows, &cols, nullptr);

  uint32_t *buffer = (uint32_t *)malloc(sizeof(uint32_t) * rows * cols);

  kernel->getConvertedImage(nullptr, nullptr, buffer);

  callback_set_image(label_show_image_after_convert, buffer, rows, cols, false);

  free(buffer);
}