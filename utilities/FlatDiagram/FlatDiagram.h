#ifndef SLOPECRAFT_UTILITIES_FLATDIAGRAM_FLATDIAGRAM_H
#define SLOPECRAFT_UTILITIES_FLATDIAGRAM_FLATDIAGRAM_H

#include <ColorManip.h>
#include <functional>
#include <Eigen/Dense>
#include <string>
#include <span>
#include <utility>

namespace libFlatDiagram {

using EImgRowMajor_t =
    Eigen::Array<ARGB, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

constexpr uint32_t reverse_color(uint32_t ARGB_src) noexcept {
  return ARGB32(255 - getR(ARGB_src), 255 - getG(ARGB_src),
                255 - getB(ARGB_src), getA(ARGB_src));
}

constexpr uint32_t reverse_byte(uint32_t v) noexcept {
  const uint32_t a = (v & 0xFF'00'00'00) >> 24;
  const uint32_t b = (v & 0x00'FF'00'00) >> 8;
  const uint32_t c = (v & 0x00'00'FF'00) << 8;
  const uint32_t d = (v & 0x00'00'00'FF) << 24;

  return a | b | c | d;
}

void reverse_color(uint32_t *ptr, size_t num_pixels) noexcept;

void ARGB_to_AGBR(uint32_t *ptr, size_t num_pixels) noexcept;

struct fd_option {
  // [row_start,row_end) * [o,col_count) will be written
  int64_t row_start;              // by block
  int64_t row_end;                // by block
  int64_t cols;                   // by block
  int32_t split_line_row_margin;  // 0 or negative number means no split lines
  int32_t split_line_col_margin;  // 0 or negative number means no split lines
  int png_compress_level{9};
  int png_compress_memory_level{8};
};

using block_img_ref_t =
    Eigen::Map<const Eigen::Array<uint32_t, 16, 16, Eigen::RowMajor>>;

constexpr size_t ret_size = sizeof(block_img_ref_t);

using get_blk_image_callback_t =
    std::function<block_img_ref_t(int64_t row_by_blk, int64_t col_by_blk)>;

constexpr size_t callback_size = sizeof(get_blk_image_callback_t);

void draw_flat_diagram_to_memory(Eigen::Map<EImgRowMajor_t> buffer,
                                 const fd_option &opt,
                                 const get_blk_image_callback_t &blk_image_at);

std::string export_flat_diagram(
    std::string_view png_filename, const fd_option &opt,
    const get_blk_image_callback_t &blk_image_at,
    std::span<std::pair<std::string, std::string>> texts) noexcept;

}  // namespace libFlatDiagram

#endif  // SLOPECRAFT_UTILITIES_FLATDIAGRAM_FLATDIAGRAM_H