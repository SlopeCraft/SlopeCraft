#ifndef SCL_MAPIMAGECVTER_MAPIMAGECVTER_H
#define SCL_MAPIMAGECVTER_MAPIMAGECVTER_H

#include <ColorManip/ColorManip.h>
#include <ColorManip/ImageConvert.hpp>

#include <memory>

namespace GACvter {
class GAConverter;
}

namespace heu {
struct GAOption;
}

namespace libMapImageCvt {

class MapImageCvter : public ::libImageCvt::ImageCvter<true> {
private:
  std::unique_ptr<GACvter::GAConverter> gacvter;

public:
  using Base_t = ::libImageCvt::ImageCvter<true>;

  using TokiColor_t = typename Base_t::TokiColor_t;

  static constexpr int size_of_tokicolor = sizeof(TokiColor_t);

  explicit MapImageCvter();

  ~MapImageCvter() = default;

  // override
  void convert_image(const ::SCL_convertAlgo algo, bool dither,
                     const heu::GAOption *const opt) noexcept;

  inline Eigen::ArrayXX<uint8_t> mapcolor_matrix() const noexcept {
    static_assert(
        std::is_same_v<uint8_t, typename ImageCvter<true>::colorid_t>);
    return this->color_id();
  }

  inline Eigen::ArrayXX<uint8_t> basecolor_matrix() const noexcept {
    return this->color_id() / 4;
  }

  inline Eigen::ArrayXX<uint8_t> shade_matrix() const noexcept {
    Eigen::ArrayXX<uint8_t> result = this->color_id();
    for (int64_t idx = 0; idx < result.size(); idx++) {
      result(idx) &= 0b11;
    }
    return result;
  }

  void col_TokiColor_ptrs(int64_t col,
                          const TokiColor_t **const dest) const noexcept {
    if (dest == nullptr) {
      return;
    }

    if (col < 0 || col > this->cols()) {
      return;
    }

    for (int64_t r = 0; r < this->rows(); r++) {
      auto it = this->_color_hash.find(
          convert_unit(this->_dithered_image(r, col), this->algo));
      dest[r] = &it->second;
    }
  }
};

} // namespace libMapImageCvt
#endif // SCL_MAPIMAGECVTER_MAPIMAGECVTER_H