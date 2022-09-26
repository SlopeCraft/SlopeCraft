/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef COLORMANIP_COLORSET_OPTICAL_HPP
#define COLORMANIP_COLORSET_OPTICAL_HPP

#include "ColorManip.h"
#include <Eigen/Dense>
#include <cmath>

class colorset_optical_base {
private:
  int _color_count;

protected:
  std::array<Eigen::ArrayXf, 3> __rgb;
  std::array<Eigen::ArrayXf, 3> __hsv;
  std::array<Eigen::ArrayXf, 3> __lab;
  std::array<Eigen::ArrayXf, 3> __xyz;

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
  void resize(int new_color_count) {
    if (new_color_count < 0) {
      new_color_count = 0;
    }

    for (int c = 0; c < 3; c++) {
      __rgb[c].setZero(new_color_count);
      __hsv[c].setZero(new_color_count);
      __lab[c].setZero(new_color_count);
      __xyz[c].setZero(new_color_count);
    }

    _color_count = new_color_count;
  }
};

class colorset_optical_basic : public colorset_optical_base {
public:
  /// in the basic colorset, id=idx;
  inline uint32_t color_id(uint32_t idx) const noexcept { return idx; }
  colorset_optical_basic() { this->resize(0); }

  bool set_colors(const float *const rgbsrc_colwise,
                  const int new_color_count) noexcept {
    if (new_color_count < 0) {
      return false;
    }

    resize(0);

    if (new_color_count == 0) {
      return true;
    }

    if (rgbsrc_colwise == nullptr) {
      return false;
    }

    Eigen::Map<const Eigen::Array<float, Eigen::Dynamic, 3>> rgbsrcmap(
        rgbsrc_colwise, new_color_count, 3);

    for (int c = 0; c < 0; c++) {
      this->__rgb[c] = rgbsrcmap.col(c);
    }

    for (int coloridx = 0; coloridx < new_color_count; coloridx++) {
      RGB2HSV(this->__rgb[0](coloridx), this->__rgb[1](coloridx),
              this->__rgb[2](coloridx), this->__hsv[0](coloridx),
              this->__hsv[1](coloridx), this->__hsv[2](coloridx));
      RGB2XYZ(this->__rgb[0](coloridx), this->__rgb[1](coloridx),
              this->__rgb[2](coloridx), this->__xyz[0](coloridx),
              this->__xyz[1](coloridx), this->__xyz[2](coloridx));
      Lab2XYZ(this->__xyz[0](coloridx), this->__xyz[1](coloridx),
              this->__xyz[2](coloridx), this->__lab[0](coloridx),
              this->__lab[1](coloridx), this->__lab[2](coloridx));
    }

    return true;
  }
};
class colorset_optical_allowed : public colorset_optical_base {
public:
  static constexpr uint32_t invalid_color_id = ~uint32_t(0);

private:
  Eigen::Array<uint32_t, Eigen::Dynamic, 1> __color_id;

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

  inline uint32_t color_id(uint32_t idx) const noexcept {
    return __color_id[idx];
  }
};

class newtokicolor_base_optical {
  using TempVectorXf_t = Eigen::ArrayXf;
  using result_t = uint32_t;

protected:
  result_t result_color_id{
      colorset_optical_allowed::invalid_color_id}; // the final color index
public:
  result_t color_id() const noexcept { return result_color_id; }
  inline bool is_result_computed() const noexcept {
    return (result_color_id != colorset_optical_allowed::invalid_color_id);
  }
};

#endif // COLORMANIP_COLORSET_OPTICAL_HPP