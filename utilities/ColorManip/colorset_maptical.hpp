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

#ifndef COLORMANIP_COLORSET_MAPTICAL_HPP
#define COLORMANIP_COLORSET_MAPTICAL_HPP

#include "ColorManip.h"
#include <Eigen/Dense>
#include <cmath>
#include <type_traits>

#if __cplusplus < 202002L
#error "C++ 20 required"
#endif

#ifdef RGB
#undef RGB
#endif

class newtokicolor_base_maptical {
 public:
  using TempVectorXf_t =
      Eigen::Array<float, Eigen::Dynamic, 1, Eigen::ColMajor, 256>;

  using result_t = uint8_t;
  // 记录与result的深度值不同的两个有损优化候选色选择系数（升序排列），Depth=3时无效
  std::array<float, 2> sideSelectivity;

  // 记录与result的深度值不同的两个有损优化候选色（升序排列），Depth=3时无效
  std::array<uint8_t, 2> sideResult;

  uint8_t Result{0};  // 最终调色结果

 public:
  // static bool needFindSide;
  // static std::array<uint8_t, 4> DepthCount;
  //  static ::SCL_convertAlgo convertAlgo;

 public:
  inline bool is_result_computed() const noexcept { return (Result != 0); }
};

class alignas(32) colorset_maptical_basic {
 private:
  Eigen::Array<float, 256, 3> __rgb;
  Eigen::Array<float, 256, 3> __hsv;
  Eigen::Array<float, 256, 3> __lab;
  Eigen::Array<float, 256, 3> __xyz;

 public:
  /// The default constructor is deleted
  colorset_maptical_basic() = delete;
  /// Construct from color source
  colorset_maptical_basic(const float *const rgbsrc) {
    assert(rgbsrc);
    memcpy(__rgb.data(), rgbsrc, sizeof(__rgb));

    for (int row = 0; row < 256; row++) {
      const float r = __rgb(row, 0), g = __rgb(row, 1), b = __rgb(row, 2);
      RGB2HSV(r, g, b, __hsv(row, 0), __hsv(row, 1), __hsv(row, 2));
      RGB2XYZ(r, g, b, __xyz(row, 0), __xyz(row, 1), __xyz(row, 2));
      XYZ2Lab(XYZ(row, 0), XYZ(row, 1), XYZ(row, 2), __lab(row, 0),
              __lab(row, 1), __lab(row, 2));
    }
  }
  /// get the color count
  inline constexpr int color_count() const noexcept { return 256; }

  inline const auto &RGB_mat() const noexcept { return __rgb; }

  inline float RGB(const int r, const int c) const noexcept {
    return __rgb(r, c);
  }
  inline float HSV(const int r, const int c) const noexcept {
    return __hsv(r, c);
  }
  inline float Lab(const int r, const int c) const noexcept {
    return __lab(r, c);
  }
  inline float XYZ(const int r, const int c) const noexcept {
    return __xyz(r, c);
  }

  static inline uint8_t Map(const int r) noexcept {
    return (r >> 6) | (r << 2);
  }

  static inline uint8_t colorid_at_index(const uint8_t color_idx) noexcept {
    return Map(color_idx);
  }

  static inline uint8_t colorindex_of_colorid(const uint8_t color_id) noexcept {
    return (color_id << 6) | (color_id >> 2);
  }
};

class alignas(32) colorset_maptical_allowed {
 public:
  using color_col = Eigen::Array<float, 256, 1>;
  static constexpr uint8_t invalid_color_id = 0;

 private:
  std::array<color_col, 3> __rgb;
  std::array<color_col, 3> __hsv;
  std::array<color_col, 3> __lab;
  std::array<color_col, 3> __xyz;
  Eigen::Array<uint8_t, 256, 1> __map;
  // std::array<uint8_t, 256> __map;
  int _color_count{0};

  std::array<uint8_t, 4> depth_counter;

 public:
  bool need_find_side{false};
  [[nodiscard]] const std::array<uint8_t, 4> &depth_count() const noexcept {
    return this->depth_counter;
  }

  inline int color_count() const noexcept { return _color_count; }

  inline float RGB(int r, int c) const noexcept {
    assert(r < color_count());
    return __rgb[c](r);
  }
  inline float HSV(int r, int c) const noexcept {
    assert(r < color_count());
    return __hsv[c](r);
  }
  inline float Lab(int r, int c) const noexcept {
    assert(r < color_count());
    return __lab[c](r);
  }
  inline float XYZ(int r, int c) const noexcept {
    assert(r < color_count());
    return __xyz[c](r);
  }

  inline uint8_t Map(int r) const noexcept {
    assert(r < color_count());
    return __map[r];
  }

  inline auto rgb(int channel) const noexcept {
    return __rgb[channel].head(_color_count);
  }

  inline auto hsv(int channel) const noexcept {
    return __hsv[channel].head(_color_count);
  }

  inline auto lab(int channel) const noexcept {
    return __lab[channel].head(_color_count);
  }

  inline auto xyz(int channel) const noexcept {
    return __xyz[channel].head(_color_count);
  }

  inline auto map() const noexcept { return __map.head(_color_count); }

  inline const float *rgb_data(int channel) const noexcept {
    return __rgb[channel].data();
  }

  inline const float *hsv_data(int channel) const noexcept {
    return __hsv[channel].data();
  }

  inline const float *lab_data(int channel) const noexcept {
    return __lab[channel].data();
  }

  inline const float *xyz_data(int channel) const noexcept {
    return __xyz[channel].data();
  }

  inline const uint8_t *map_data() const noexcept { return __map.data(); }

  bool apply_allowed(const colorset_maptical_basic &src,
                     std::span<const bool, 256> allow_list) noexcept {
    const int new_color_count = allow_list_counter(allow_list);

    if (new_color_count <= 3) {
      return false;
    }

    for (int c = 0; c < 3; c++) {
      __rgb[c].setZero();
      __hsv[c].setZero();
      __lab[c].setZero();
      __xyz[c].setZero();
    }
    __map.setZero();

    _color_count = new_color_count;

    for (int writeidx = 0, readidx = 0; readidx < 256; readidx++) {
      const int base = (readidx & 0b111111);
      if (base == 0) {
        continue;
      }

      if (allow_list[readidx]) {
        for (int c = 0; c < 3; c++) {
          __rgb[c](writeidx) = src.RGB(readidx, c);
          __hsv[c](writeidx) = src.HSV(readidx, c);
          __lab[c](writeidx) = src.Lab(readidx, c);
          __xyz[c](writeidx) = src.XYZ(readidx, c);
        }
        __map[writeidx] = src.Map(readidx);
        writeidx++;
      }
    }
    this->depth_counter.fill(0);
    for (int idx = 0; idx < new_color_count; idx++) {
      const uint8_t mapcolor = this->Map(idx);
      const uint8_t base = mapcolor >> 2;
      if (base != 0) {
        const uint8_t depth = mapcolor & 0b11;
        this->depth_counter[depth]++;
      }
    }

    return true;
  }

 private:
  inline int allow_list_counter(
      std::span<const bool, 256> allow_list) const noexcept {
    int result = 0;
    for (int idx = 0; idx < 256; idx++) {
      const int base = (idx & 0b111111);
      if (base == 0) {
        continue;
      }
      result += allow_list[idx];
    }
    return result;
  }
};

#endif  // COLORMANIP_COLORSET_MAPTICAL_HPP