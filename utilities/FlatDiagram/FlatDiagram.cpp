#include <png.h>
#include "FlatDiagram.h"
#include <stdlib.h>
#include <vector>
#include <stdio.h>

#include <fmt/format.h>

void libFlatDiagram::reverse_color(uint32_t *ptr, size_t num_pixels) noexcept {
  // this can be vertorized by compiler optimization
  for (uint32_t *p = ptr; p < ptr + num_pixels; p++) {
    *p = reverse_color(*p);
  }
}

void libFlatDiagram::ARGB_to_AGBR(uint32_t *ptr, size_t num_pixels) noexcept {
  for (uint32_t *p = ptr; p < ptr + num_pixels; p++) {
    const uint32_t A = getA(*p);
    *p = (*p) << 8 | A;
    *p = reverse_byte(*p);
  }
}

void libFlatDiagram::draw_flat_diagram_to_memory(
    Eigen::Map<EImgRowMajor_t> buffer, const fd_option &opt,
    const get_blk_image_callback_t &blk_image_at) {
  assert(buffer.cols() == opt.cols * 16);
  assert(buffer.rows() >= (opt.row_end - opt.row_start) * 16);

  // copy block images
  for (int64_t r = opt.row_start; r < opt.row_end; r++) {
    const int r_pixel_beg = (r - opt.row_start) * 16;
    for (int64_t c = 0; c < opt.cols; c++) {
      const int c_pixel_beg = c * 16;
      /*
      const bool is_src_aligned =
          (reinterpret_cast<size_t>(
               blkp->project_image_on_exposed_face.data()) %
           32) == 0;
      const bool is_aligned = is_dst_aligned && is_src_aligned;
      */

      buffer.block<16, 16>(r_pixel_beg, c_pixel_beg) = blk_image_at(r, c);
    }
  }

  for (int64_t br = opt.row_start; br < opt.row_end; br++) {
    if ((opt.split_line_row_margin > 0) &&
        (br % opt.split_line_row_margin == 0)) {
      const int64_t pr = (br - opt.row_start) * 16;

      reverse_color(&buffer(pr, 0), buffer.cols());
    }
  }

  for (int64_t bc = 0; bc < opt.cols; bc++) {
    if ((opt.split_line_col_margin > 0) &&
        (bc % opt.split_line_col_margin == 0)) {
      const int64_t pc = bc * 16;

      for (int64_t pr = 0; pr < buffer.rows(); pr++) {
        buffer(pr, pc) = reverse_color(buffer(pr, pc));
      }
    }
  }
}

std::string libFlatDiagram::export_flat_diagram(
    std::string_view png_filename, const fd_option &opt,
    const get_blk_image_callback_t &blk_image_at,
    std::span<std::pair<std::string, std::string>> texts) noexcept {
  const int64_t rows_capacity_by_blocks = 16;

  EImgRowMajor_t buffer(rows_capacity_by_blocks * 16, opt.cols * 16);

  FILE *fp = fopen(png_filename.data(), "wb");

  if (fp == nullptr) {
    return fmt::format("fopen failed to create png file {}.", png_filename);
  }

  png_struct *png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png == nullptr) {
    fclose(fp);
    return fmt::format("fopen failed to create png struct for png file {}.",
                       png_filename);
  }

  png_info *png_info = png_create_info_struct(png);
  if (png_info == nullptr) {
    png_destroy_write_struct(&png, &png_info);
    fclose(fp);
    return fmt::format(
        "fopen failed to create png info struct for png file {}.",
        png_filename);
  }

  png_init_io(png, fp);

  png_set_compression_level(png, opt.png_compress_level);

  png_set_compression_mem_level(png, opt.png_compress_memory_level);

  // png_set_text_compression_level(png, 8);

  png_set_IHDR(png, png_info, opt.cols * 16, 16 * (opt.row_end - opt.row_start),
               8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, png_info);

  {
    std::vector<png_text> png_txts;
    png_txts.reserve(texts.size());
    for (auto &[first, second] : texts) {
      png_text temp;
      temp.compression = -1;
      temp.key = first.data();
      temp.text = second.data();
      png_txts.emplace_back(temp);
    }

    png_set_text(png, png_info, png_txts.data(), png_txts.size());
  }

  try {
    for (int64_t ridx = opt.row_start; ridx < opt.row_end;
         ridx += rows_capacity_by_blocks) {
      const int64_t rows_this_time =
          std::min(opt.row_end - ridx, rows_capacity_by_blocks);
      buffer.fill(0xFFFFFFFF);

      fd_option opt_temp = opt;
      opt_temp.row_start = ridx;
      opt_temp.row_end = ridx + rows_this_time;

      draw_flat_diagram_to_memory({buffer.data(), buffer.rows(), buffer.cols()},
                                  opt_temp, blk_image_at);

      ARGB_to_AGBR(buffer.data(), rows_this_time * 16 * opt.cols * 16);

      for (int64_t pix_r = 0; pix_r < rows_this_time * 16; pix_r++) {
        png_write_row(png,
                      reinterpret_cast<const uint8_t *>(&buffer(pix_r, 0)));
      }
    }
  } catch (const std::exception &e) {
    return fmt::format("Exception occurred while writing flat diagram: {}",
                       e.what());
  }

  png_write_end(png, png_info);

  png_destroy_write_struct(&png, &png_info);
  fclose(fp);

  return {};
}