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
      auto it = this->_color_hash.find(
          convert_unit(this->_dithered_image(r, col), this->algo));
      dest.emplace_back(&it->second);
    }
  }
  
  // temp is a temporary container to pass ownership
  void load_from_itermediate(MapImageCvter &&temp) noexcept {
    this->_raw_image = std::move(temp._raw_image);
    this->algo = temp.algo;
    this->_dithered_image = std::move(temp._dithered_image);

    assert(this->_raw_image.rows() == this->_dithered_image.rows());
    assert(this->_raw_image.cols() == this->_dithered_image.cols());
    if (this->_color_hash.empty()) {
      this->_color_hash = std::move(temp._color_hash);
    } else {
      temp._color_hash.merge(this->_color_hash);
      this->_color_hash = std::move(temp._color_hash);
    }
  }

 private:
  friend class cereal::access;
  template <class archive>
  void save(archive &ar) const {
    assert(this->_raw_image.rows() == this->_dithered_image.rows());
    assert(this->_raw_image.cols() == this->_dithered_image.cols());
    ar(this->_raw_image);
    ar(this->algo);
    ar(this->dither);
    // ar(this->_color_hash);
    ar(this->_dithered_image);
    // save required colorset
    {
      std::unordered_set<uint32_t> colors_dithered_img;
      colors_dithered_img.reserve(this->_dithered_image.size());
      for (int64_t i = 0; i < this->_dithered_image.size(); i++) {
        colors_dithered_img.emplace(this->_dithered_image(i));
        colors_dithered_img.emplace(this->_raw_image(i));
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
    ar(this->_raw_image);
    ar(this->algo);
    ar(this->dither);
    // ar(this->_color_hash);
    ar(this->_dithered_image);

    assert(this->_raw_image.rows() == this->_dithered_image.rows());
    assert(this->_raw_image.cols() == this->_dithered_image.cols());

    {
      size_t size_colorset{0};
      ar(size_colorset);
      for (size_t idx = 0; idx < size_colorset; idx++) {
        convert_unit key;
        TokiColor_t val;
        ar(key, val);

        this->_color_hash.emplace(key, val);
      }

      for (int64_t i = 0; i < this->_dithered_image.size(); i++) {
        auto it = this->_color_hash.find(
            convert_unit{this->_dithered_image(i), this->convert_algo()});
        if (it == this->_color_hash.end()) {
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