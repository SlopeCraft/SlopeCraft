#include "newColorSet.hpp"
#include "imageConvert.hpp"

#include <sha3.h>

std::vector<uint8_t> internal::hash_of_colorset(
    const hash_temp &temp) noexcept {
  Chocobo1::SHA3_512 stream;

  for (const auto &cptrs : temp.color_ptrs) {
    for (const float *fptr : cptrs) {
      stream.addData(fptr, temp.color_count * sizeof(float));
    }
  }

  stream.addData(temp.color_id_ptr,
                 temp.color_count *
                     (temp.is_maptical ? sizeof(uint8_t) : sizeof(uint16_t)));
  stream.addData(&temp.color_count, sizeof(temp.color_count));

  return stream.finalize().toVector();
}

std::vector<uint8_t> libImageCvt::hash_of_image(
    Eigen::Map<const Eigen::ArrayXX<ARGB>> img) noexcept {
  Chocobo1::SHA3_512 stream;
  {
    const auto rows{img.rows()};
    const auto cols{img.cols()};
    stream.addData(&rows, sizeof(rows));
    stream.addData(&cols, sizeof(cols));
  }

  stream.addData(img.data(), sizeof(uint32_t) * img.size());

  return stream.finalize().toVector();
}