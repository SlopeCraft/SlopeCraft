#include "TokiVC.h"
#include "VisualCraftL.h"

#include <immintrin.h>
#include <xmmintrin.h>

// in-place-reverse color
void reverse_color_avx2(uint32_t *ptr_32_aligned, size_t num_pixels) noexcept {

  const __m256i a = _mm256_set1_epi8(0xFF);

  if (num_pixels % 8 != 0) [[unlikely]] {
    abort();
  }

  for (uint32_t *ptr = ptr_32_aligned; ptr < ptr_32_aligned + num_pixels / 8;
       ptr += 8) {
    // const __m256i ARGB_src = _mm256_load_epi32(ptr);
    const __m256i ARGB_src = *reinterpret_cast<__m256i *>(ptr);

    const __m256i A_src =
        _mm256_and_si256(ARGB_src, _mm256_set1_epi32(0xFF'00'00'00));
    const __m256i ARGB_inv = _mm256_subs_epu8(a, ARGB_src);

    const __m256i A_RGB_inv = _mm256_or_si256(ARGB_inv, A_src);

    *reinterpret_cast<__m256i *>(ptr) = A_RGB_inv;
    //_mm256_store_epi32(ptr, A_RGB_inv);
  }
}

void reverse_color(uint32_t *ptr, size_t num_pixels) noexcept {
  for (uint32_t *p = ptr; p < ptr + num_pixels; p++) {
    const uint32_t ARGB_src = *p;
    const uint32_t result = ARGB32(255 - getR(ARGB_src), 255 - getG(ARGB_src),
                                   255 - getB(ARGB_src), getA(ARGB_src));
    *p = result;
  }
}

void TokiVC::draw_flag_diagram_to_memory(uint32_t *image_u8c3_rowmajor,
                                         const flag_diagram_option &opt,
                                         int layer_idx) const noexcept {
  Eigen::Map<block_model::EImgRowMajor_t> map(
      image_u8c3_rowmajor, (opt.row_end - opt.row_start) * 16,
      this->cols() * 16);

  memset(image_u8c3_rowmajor, 0,
         (opt.row_end - opt.row_start) * 16 * this->cols() * 16 *
             sizeof(uint32_t));

  constexpr int size_of_u32_per_vec = 32 / sizeof(uint32_t);

  const bool is_dst_aligned =
      (reinterpret_cast<size_t>(image_u8c3_rowmajor) % 32) == 0;

  for (int64_t r = opt.row_start; r < opt.row_end; r++) {
    const int r_pixel_beg = (r - opt.row_start) * 16;
    for (int64_t c = 0; c < this->cols(); c++) {
      const int c_pixel_beg = c * 16;
      const uint16_t current_color_idx = this->img_cvter.color_id(r, c);

      const auto &variant =
          TokiVC::LUT_basic_color_idx_to_blocks[current_color_idx];
      const VCL_block *blkp = nullptr;
      if (variant.index() == 0 && layer_idx == 0) {
        blkp = std::get<0>(variant);
      } else {
        const auto &vec = std::get<1>(variant);

        if (layer_idx < (int)vec.size()) {
          blkp = vec[layer_idx];
        }
      }

      if (blkp == nullptr) {
        continue;
      }

      const bool is_src_aligned =
          (reinterpret_cast<size_t>(
               blkp->project_image_on_exposed_face.data()) %
           32) == 0;
      const bool is_aligned = is_dst_aligned && is_src_aligned;

      if (!is_aligned) {
        map.block<16, 16>(r_pixel_beg, c_pixel_beg) =
            blkp->project_image_on_exposed_face;
      } else {
        for (int r_p_offset = 0; r_p_offset < 16;
             r_p_offset += size_of_u32_per_vec) {
          for (int c_p_offset = 0; c_p_offset < 16;
               c_p_offset += size_of_u32_per_vec) {
            uint32_t *const dst =
                &map(r_pixel_beg + r_p_offset, c_pixel_beg + c_p_offset);
            const uint32_t *const src =
                &blkp->project_image_on_exposed_face(r_p_offset, c_p_offset);

            //__m256i data = _mm256_load_epi32(src);
            //_mm256_store_epi32(dst, data);
            __m256i data = *reinterpret_cast<const __m256i *>(src);
            *reinterpret_cast<__m256i *>(dst) = data;
          }
        }
      }
    }
  }
}

void TokiVC::flag_diagram(uint32_t *image_u8c3_rowmajor,
                          const flag_diagram_option &, int layer_idx,
                          int64_t *rows_required_dest,
                          int64_t *cols_required_dest) const noexcept {}
bool TokiVC::export_flag_diagram(const char *png_filename,
                                 const flag_diagram_option &,
                                 int layer_idx) const noexcept {}
