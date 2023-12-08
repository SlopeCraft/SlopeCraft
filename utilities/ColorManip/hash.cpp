#include "newColorSet.hpp"
#include "imageConvert.hpp"
#include <span>
#include <boost/uuid/detail/sha1.hpp>

namespace buuid = boost::uuids::detail;

/*
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

*/

std::vector<uint8_t> libImageCvt::hash_of_image(
    Eigen::Map<const Eigen::ArrayXX<ARGB>> img) noexcept {
  buuid::sha1 hash;
  {
    const auto rows{img.rows()};
    const auto cols{img.cols()};
    hash.process_bytes(&rows, sizeof(rows));
    hash.process_bytes(&cols, sizeof(cols));
  }

  hash.process_bytes(img.data(), sizeof(uint32_t) * img.size());
  buuid::sha1::digest_type dig;
  hash.get_digest(dig);

  std::span<const uint8_t> temp{reinterpret_cast<const uint8_t*>(dig),
                                sizeof(dig)};

  return {temp.begin(), temp.end()};
}