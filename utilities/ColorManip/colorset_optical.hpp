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

#ifndef COLORMANIP_COLORSET_OPTICAL_HPP
#define COLORMANIP_COLORSET_OPTICAL_HPP

#include "../SC_aligned_alloc.hpp"
#include "ColorManip.h"
#include <Eigen/Dense>
#include <cmath>

class alignas(32) colorset_optical_base {
 protected:
  std::array<Eigen::ArrayXf, 3> __rgb;
  std::array<Eigen::ArrayXf, 3> __hsv;
  std::array<Eigen::ArrayXf, 3> __lab;
  std::array<Eigen::ArrayXf, 3> __xyz;

 private:
  int _color_count;

 public:
  inline int color_count() const noexcept { return _color_count; }

  inline float RGB(int r, int c) const noexcept { return __rgb[c](r); }
  inline auto rgb(int c) const noexcept { return __rgb[c].head(_color_count); }
  inline const float *rgb_data(int c) const noexcept { return __rgb[c].data(); }

  inline float HSV(int r, int c) const noexcept { return __hsv[c](r); }
  inline auto hsv(int c) const noexcept { return __hsv[c].head(_color_count); }
  inline const float *hsv_data(int c) const noexcept { return __hsv[c].data(); }

  inline float Lab(int r, int c) const noexcept { return __lab[c](r); }
  inline auto lab(int c) const noexcept { return __lab[c].head(_color_count); }
  inline const float *lab_data(int c) const noexcept { return __lab[c].data(); }

  inline float XYZ(int r, int c) const noexcept { return __xyz[c](r); }
  inline auto xyz(int c) const noexcept { return __xyz[c].head(_color_count); }
  inline const float *xyz_data(int c) const noexcept { return __xyz[c].data(); }

 protected:
  void resize(int new_color_count) noexcept {
    if (new_color_count < 0) {
      new_color_count = 0;
    }

    for (int c = 0; c < 3; c++) {
      __rgb[c].setZero(new_color_count);
      __hsv[c].setZero(new_color_count);
      __lab[c].setZero(new_color_count);
      __xyz[c].setZero(new_color_count);
    }

    this->_color_count = new_color_count;
  }
};

class colorset_optical_basic : public colorset_optical_base {
 public:
  /// in the basic colorset, id=idx;
  inline uint16_t color_id(uint16_t idx) const noexcept { return idx; }
  colorset_optical_basic() { this->resize(0); }

  bool set_colors(const float *const rbgsrc_colmajor,
                  const int new_color_count) noexcept {
    if (new_color_count < 0) {
      return false;
    }

    this->resize(0);

    if (new_color_count == 0) {
      return true;
    }

    if (rbgsrc_colmajor == nullptr) {
      return false;
    }

    this->resize(new_color_count);

    {
      Eigen::Map<const Eigen::Array<float, Eigen::Dynamic, 3>> rgbsrcmap(
          rbgsrc_colmajor, new_color_count, 3);

      for (int c = 0; c < 3; c++) {
        memcpy(this->__rgb[c].data(), rbgsrc_colmajor + (new_color_count * c),
               sizeof(float) * new_color_count);
        //= rgbsrcmap.col(c);
      }
    }

    for (int coloridx = 0; coloridx < new_color_count; coloridx++) {
      RGB2HSV(this->__rgb[0](coloridx), this->__rgb[1](coloridx),
              this->__rgb[2](coloridx), this->__hsv[0](coloridx),
              this->__hsv[1](coloridx), this->__hsv[2](coloridx));
      RGB2XYZ(this->__rgb[0](coloridx), this->__rgb[1](coloridx),
              this->__rgb[2](coloridx), this->__xyz[0](coloridx),
              this->__xyz[1](coloridx), this->__xyz[2](coloridx));
      XYZ2Lab(this->__xyz[0](coloridx), this->__xyz[1](coloridx),
              this->__xyz[2](coloridx), this->__lab[0](coloridx),
              this->__lab[1](coloridx), this->__lab[2](coloridx));
    }

    return true;
  }

  inline uint16_t colorid_at_index(const uint16_t color_idx) const noexcept {
    return color_idx;
  }

  inline uint16_t colorindex_of_id(const uint16_t color_id) const noexcept {
    return color_id;
  }

  inline uint16_t colorindex_of_colorid(
      const uint16_t color_id) const noexcept {
    return this->colorindex_of_id(color_id);
  }
};

class colorset_optical_allowed : public colorset_optical_base {
 public:
  static constexpr uint16_t invalid_color_id = 0xFFFF;

 private:
  Eigen::Array<uint16_t, Eigen::Dynamic, 1> __color_id;

  void resize(int new_color_count) {
    colorset_optical_base::resize(new_color_count);
    __color_id.resize(new_color_count);
    __color_id.fill(invalid_color_id);
  }

 public:
  colorset_optical_allowed() { this->resize(0); }

  bool apply_allowed(const colorset_optical_basic &src,
                     const bool *const allow_list) noexcept {
    if (src.color_count() <= 0) {
      return false;
    }

    if (allow_list == nullptr) {
      return false;
    }

    int new_color_count = 0;

    for (int idx = 0; idx < src.color_count(); idx++) {
      new_color_count += (allow_list[idx]);
    }

    resize(new_color_count);

    for (int writeidx = 0, readidx = 0; readidx < src.color_count();
         readidx++) {
      if (!allow_list[readidx]) {
        continue;
      }

      for (int c = 0; c < 3; c++) {
        this->__rgb[c][writeidx] = src.RGB(readidx, c);
        this->__hsv[c][writeidx] = src.HSV(readidx, c);
        this->__lab[c][writeidx] = src.Lab(readidx, c);
        this->__xyz[c][writeidx] = src.XYZ(readidx, c);
      }
      __color_id[writeidx] = src.color_id(readidx);

      writeidx++;
    }

    return true;
  }

  inline uint16_t color_id(uint16_t idx) const noexcept {
    assert(idx < __color_id.size());
    return __color_id[idx];
  }
};

class TempVecOptical : public Eigen::Map<Eigen::ArrayXf> {
 public:
  static constexpr size_t capacity = 65536;
  using Base_t = Eigen::Map<Eigen::ArrayXf>;

 public:
  TempVecOptical(int rows, int cols)
      : Base_t((float *)SC_aligned_alloc(64, capacity * sizeof(float)), rows,
               cols) {
    if (cols != 1) {
      abort();
    }
  }

  TempVecOptical(const TempVecOptical &) = delete;
  TempVecOptical(TempVecOptical &&) = delete;

  ~TempVecOptical() { SC_aligned_free(this->data()); }

  void resize(int rows, int cols) noexcept {
    if (cols != 1 || size_t(rows) > capacity) {
      abort();
    }

    Base_t::resize(rows, cols);
  }

  inline Base_t &base() noexcept { return *this; }
};

class newtokicolor_base_optical {
 public:
  // using TempVectorXf_t = TempVecOptical;
  using TempVectorXf_t = Eigen::ArrayXf;
  using result_t = uint16_t;

 protected:
  result_t result_color_id{
      colorset_optical_allowed::invalid_color_id};  // the final color index
 public:
  inline result_t color_id() const noexcept { return result_color_id; }
  inline bool is_result_computed() const noexcept {
    return (result_color_id != colorset_optical_allowed::invalid_color_id);
  }
};

#endif  // COLORMANIP_COLORSET_OPTICAL_HPP