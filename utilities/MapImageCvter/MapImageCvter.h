/*
 Copyright © 2021-2026  TokiNoBug
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

#ifndef SCL_MAPIMAGECVTER_MAPIMAGECVTER_H
#define SCL_MAPIMAGECVTER_MAPIMAGECVTER_H

#include <ColorManip/ColorManip.h>
#include <ColorManip/imageConvert.hpp>
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/unordered_map.hpp>
#include <memory>
#include <exception>

namespace GACvter {
class GAConverter;
void delete_GA_converter(GAConverter *) noexcept;
}  // namespace GACvter

namespace heu {
struct GAOption;
}

namespace libMapImageCvt {

class MapImageCvter : public ::libImageCvt::ImageCvter<true> {
 private:
  using deleter_t = decltype([](GACvter::GAConverter *g) {
    GACvter::delete_GA_converter(g);
  });
  std::unique_ptr<GACvter::GAConverter, deleter_t> gacvter;

 public:
  using Base_t = ::libImageCvt::ImageCvter<true>;

  using TokiColor_t = typename Base_t::TokiColor_t;

  static constexpr int size_of_tokicolor = sizeof(TokiColor_t);

  MapImageCvter(const Base_t::basic_colorset_t &basic,
                const Base_t::allowed_colorset_t &allowed);
  MapImageCvter(MapImageCvter &&) = default;

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

  void col_TokiColor_ptrs(
      int64_t col, std::vector<const TokiColor_t *> &dest) const noexcept {
    if (col < 0 || col > this->cols()) {
      return;
    }
    dest.clear();
    for (int64_t r = 0; r < this->rows(); r++) {
      auto it = this->color_hash_.find(
          convert_unit(this->dithered_image_(r, col), this->algo));
      dest.emplace_back(&it->second);
    }
  }

  // temp is a temporary container to pass ownership
  void load_from_itermediate(MapImageCvter &&temp) noexcept {
    this->raw_image_ = std::move(temp.raw_image_);
    this->algo = temp.algo;
    this->dithered_image_ = std::move(temp.dithered_image_);

    assert(this->raw_image_.rows() == this->dithered_image_.rows());
    assert(this->raw_image_.cols() == this->dithered_image_.cols());
    if (this->color_hash_.empty()) {
      this->color_hash_ = std::move(temp.color_hash_);
    } else {
      temp.color_hash_.merge(this->color_hash_);
      this->color_hash_ = std::move(temp.color_hash_);
    }
  }

 private:
  friend class cereal::access;
  template <class archive>
  void save(archive &ar) const {
    assert(this->raw_image_.rows() == this->dithered_image_.rows());
    assert(this->raw_image_.cols() == this->dithered_image_.cols());
    ar(this->raw_image_);
    ar(this->algo);
    ar(this->dither);
    // ar(this->_color_hash);
    ar(this->dithered_image_);
    // save required colorset
    {
      std::unordered_set<uint32_t> colors_dithered_img;
      colors_dithered_img.reserve(this->dithered_image_.size());
      for (int64_t i = 0; i < this->dithered_image_.size(); i++) {
        colors_dithered_img.emplace(this->dithered_image_(i));
        colors_dithered_img.emplace(this->raw_image_(i));
      }

      const size_t size_colorset = colors_dithered_img.size();
      ar(size_colorset);
      for (uint32_t color : colors_dithered_img) {
        auto it =
            this->color_hash().find(convert_unit{color, this->convert_algo()});
        if (it == this->color_hash().end()) {
          assert(getA(color) <= 0);
          continue;
        }

        ar(it->first, it->second);
      }
    }
  }

  template <class archive>
  void load(archive &ar) {
    ar(this->raw_image_);
    ar(this->algo);
    ar(this->dither);
    // ar(this->_color_hash);
    ar(this->dithered_image_);

    assert(this->raw_image_.rows() == this->dithered_image_.rows());
    assert(this->raw_image_.cols() == this->dithered_image_.cols());

    {
      size_t size_colorset{0};
      ar(size_colorset);
      for (size_t idx = 0; idx < size_colorset; idx++) {
        convert_unit key;
        TokiColor_t val;
        ar(key, val);

        this->color_hash_.emplace(key, val);
      }

      for (int64_t i = 0; i < this->dithered_image_.size(); i++) {
        auto it = this->color_hash_.find(
            convert_unit{this->dithered_image_(i), this->convert_algo()});
        if (it == this->color_hash_.end()) {
          throw std::runtime_error{
              "One or more colors not found in cached colorhash"};
        }
      }
    }
  }

 public:
  bool save_cache(const char *filename) const noexcept;
  bool examine_cache(const char *filename, uint64_t expected_task_hash,
                     MapImageCvter *itermediate = nullptr) const noexcept;
  [[nodiscard]] bool load_cache(const char *filename,
                                uint64_t expected_task_hash) noexcept;
  bool load_cache(const char *filename) noexcept;
};

}  // namespace libMapImageCvt
#endif  // SCL_MAPIMAGECVTER_MAPIMAGECVTER_H