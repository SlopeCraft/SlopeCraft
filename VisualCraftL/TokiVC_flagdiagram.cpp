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

#include "TokiVC.h"
#include "VCL_internal.h"
#include "VisualCraftL.h"

#ifdef min
#undef min
#endif

constexpr uint32_t reverse_color(uint32_t ARGB_src) noexcept {
  return ARGB32(255 - getR(ARGB_src), 255 - getG(ARGB_src),
                255 - getB(ARGB_src), getA(ARGB_src));
}

void reverse_color(uint32_t *ptr, size_t num_pixels) noexcept {
  // this can be vertorized by compiler optimization
  for (uint32_t *p = ptr; p < ptr + num_pixels; p++) {
    *p = reverse_color(*p);
  }
}

constexpr uint32_t reverse_byte(uint32_t v) noexcept {
  const uint32_t a = (v & 0xFF'00'00'00) >> 24;
  const uint32_t b = (v & 0x00'FF'00'00) >> 8;
  const uint32_t c = (v & 0x00'00'FF'00) << 8;
  const uint32_t d = (v & 0x00'00'00'FF) << 24;

  return a | b | c | d;
}

void ARGB_to_AGBR(uint32_t *ptr, size_t num_pixels) noexcept {
  for (uint32_t *p = ptr; p < ptr + num_pixels; p++) {
    const uint32_t A = getA(*p);
    *p = (*p) << 8 | A;
    *p = reverse_byte(*p);
  }
}

void TokiVC::draw_flag_diagram_to_memory(uint32_t *image_u8c3_rowmajor,
                                         const flag_diagram_option &opt,
                                         int layer_idx) const noexcept {
  Eigen::Map<block_model::EImgRowMajor_t> map(
      image_u8c3_rowmajor, (opt.row_end - opt.row_start) * 16,
      this->cols() * 16);

  memset(image_u8c3_rowmajor, 0,
         (opt.row_end - opt.row_start) * 16 * this->img_cvter.cols() * 16 *
             sizeof(uint32_t));

  /*s  constexpr int size_of_u32_per_vec = 32 / sizeof(uint32_t);

  const bool is_dst_aligned =
      (reinterpret_cast<size_t>(image_u8c3_rowmajor) % 32) == 0;

      */
  // copy block images
  for (int64_t r = opt.row_start; r < opt.row_end; r++) {
    const int r_pixel_beg = (r - opt.row_start) * 16;
    for (int64_t c = 0; c < this->cols(); c++) {
      const int c_pixel_beg = c * 16;
      const uint16_t current_color_idx = this->img_cvter.color_id(r, c);

      const auto &variant =
          TokiVC::LUT_basic_color_idx_to_blocks[current_color_idx];
      const VCL_block *blkp = nullptr;
      if (variant.index() == 0) {
        if (layer_idx == 0) {
          blkp = std::get<0>(variant);
        }
      } else {
        const auto &vec = std::get<1>(variant);

        if (layer_idx < (int)vec.size()) {
          blkp = vec[layer_idx];
        }
      }

      if (blkp == nullptr) {
        map.block<16, 16>(r_pixel_beg, c_pixel_beg).fill(0x00'FF'FF'FF);
        continue;
      }
      /*
      const bool is_src_aligned =
          (reinterpret_cast<size_t>(
               blkp->project_image_on_exposed_face.data()) %
           32) == 0;
      const bool is_aligned = is_dst_aligned && is_src_aligned;
      */

      map.block<16, 16>(r_pixel_beg, c_pixel_beg) =
          blkp->project_image_on_exposed_face;
    }
  }

  for (int64_t br = opt.row_start; br < opt.row_end; br++) {
    if ((opt.split_line_row_margin > 0) &&
        (br % opt.split_line_row_margin == 0)) {
      const int64_t pr = (br - opt.row_start) * 16;

      reverse_color(&map(pr, 0), map.cols());
    }
  }

  for (int64_t bc = 0; bc < this->img_cvter.cols(); bc++) {
    if ((opt.split_line_col_margin > 0) &&
        (bc % opt.split_line_col_margin == 0)) {
      const int64_t pc = bc * 16;

      for (int64_t pr = 0; pr < map.rows(); pr++) {
        map(pr, pc) = reverse_color(map(pr, pc));
      }
    }
  }
}

void TokiVC::flag_diagram(uint32_t *image_u8c3_rowmajor,
                          const flag_diagram_option &opt, int layer_idx,
                          int64_t *rows_required_dest,
                          int64_t *cols_required_dest) const noexcept {
  if (this->_step < ::VCL_Kernel_step::VCL_wait_for_build) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export flag diagram without image converted.");
    return;
  }

  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (rows_required_dest != nullptr) {
    *rows_required_dest = (opt.row_end - opt.row_start) * 16;
  }

  if (cols_required_dest != nullptr) {
    *cols_required_dest = this->img_cvter.cols() * 16;
  }

  if (image_u8c3_rowmajor == nullptr) {
    return;
  }

  this->draw_flag_diagram_to_memory(image_u8c3_rowmajor, opt, layer_idx);
}

#include <png.h>
#include <zlib.h>

bool TokiVC::export_flag_diagram(const char *png_filename,
                                 const flag_diagram_option &opt,
                                 int layer_idx) const noexcept {
  if (this->_step < ::VCL_Kernel_step::VCL_wait_for_build) {
    VCL_report(VCL_report_type_t::error,
               "Trying to export flag diagram without image converted.");
    return false;
  }

  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  this->img_cvter.ui.rangeSet(0, this->img_cvter.rows(), 0);

  const int64_t rows_capacity_by_blocks = 64;

  block_model::EImgRowMajor_t buffer(rows_capacity_by_blocks * 16,
                                     this->img_cvter.cols() * 16);

  FILE *fp = fopen(png_filename, "wb");

  if (fp == nullptr) {
    std::string msg =
        fmt::format("fopen failed to create png file {}.", png_filename);
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  png_struct *png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png == nullptr) {
    fclose(fp);
    std::string msg = fmt::format(
        "fopen failed to create png struct for png file {}.", png_filename);
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  png_info *png_info = png_create_info_struct(png);
  if (png_info == nullptr) {
    png_destroy_write_struct(&png, &png_info);
    fclose(fp);
    std::string msg =
        fmt::format("fopen failed to create png info struct for png file {}.",
                    png_filename);
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  png_init_io(png, fp);

  png_set_compression_level(png, opt.png_compress_level);

  png_set_compression_mem_level(png, opt.png_compress_memory_level);

  // png_set_text_compression_level(png, 8);

  png_set_IHDR(png, png_info, this->img_cvter.cols() * 16,
               16 * (opt.row_end - opt.row_start), 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, png_info);

  {
    std::array<std::pair<std::string, std::string>, 4> txt{
        std::make_pair<std::string, std::string>(
            "Title", "Flat diagram generated by VisualCraftL."),
        std::make_pair<std::string, std::string>("Software", "VisualCraftL"),
        std::make_pair<std::string, std::string>(
            "Description",
            "This image is a flat diagram created by VisualCraftL, which is is "
            "a subproject of SlopeCraft, developed by TokiNoBug."),
        std::make_pair<std::string, std::string>(
            "Comment",
            "SlopeCraft is a free software published "
            "under GPLv3 license. You can find "
            "its repository at https://github.com/SlopeCraft/SlopeCraft")};

    std::array<png_text, txt.size()> png_txts;

    for (size_t i = 0; i < txt.size(); i++) {
      png_txts[i].compression = -1;
      png_txts[i].key = txt[i].first.data();
      png_txts[i].text = txt[i].second.data();
      // png.txts[i].key =
    }

    png_set_text(png, png_info, png_txts.data(), png_txts.size());
  }

  for (int64_t ridx = opt.row_start; ridx < opt.row_end;
       ridx += rows_capacity_by_blocks) {
    const int64_t rows_this_time =
        std::min(opt.row_end - ridx, rows_capacity_by_blocks);
    memset(buffer.data(), 0xFF, buffer.size() * sizeof(uint32_t));
    this->draw_flag_diagram_to_memory(
        buffer.data(),
        {opt.lib_version, ridx, ridx + rows_this_time,
         opt.split_line_row_margin, opt.split_line_col_margin},
        layer_idx);

    ARGB_to_AGBR(buffer.data(),
                 rows_this_time * 16 * this->img_cvter.cols() * 16);

    for (int64_t pix_r = 0; pix_r < rows_this_time * 16; pix_r++) {
      png_write_row(png, reinterpret_cast<const uint8_t *>(&buffer(pix_r, 0)));
    }

    this->img_cvter.ui.rangeSet(0, this->img_cvter.rows(),
                                ridx - opt.row_start);
  }

  png_write_end(png, png_info);

  png_destroy_write_struct(&png, &png_info);
  fclose(fp);

  this->img_cvter.ui.rangeSet(0, this->img_cvter.rows(),
                              this->img_cvter.rows());
  return true;
}
