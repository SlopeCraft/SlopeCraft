#ifndef SCL_NEWCOLORSET_HPP
#define SCL_NEWCOLORSET_HPP

#include "ColorManip.h"
#include "newTokiColor.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <type_traits>

#if __cplusplus < 201703L
#error "C++17 required"
#endif

using Eigen::Dynamic;

class colorset_u8array {
public:
  colorset_u8array() {
    for (short r = 0; r < 256; r++)
      Map(r) = 4 * (r % 64) + r / 64;
  }

  Eigen::Array<uint8_t, 256, 1> Map;
};

struct heu_empty_struct00 {};

template <bool is_basical, bool is_not_optical, int max_color_count>
class colorset_new : public std::conditional_t<is_not_optical, colorset_u8array,
                                               heu_empty_struct00> {
public:
  static constexpr int align_bytes = 32;
  // align as 32 bytes.
  static constexpr int _capacity =
      std::ceil(float(sizeof(float) * max_color_count) / align_bytes) *
      align_bytes;

  using color_col = Eigen::Array<float, _capacity, 1>;
  using color_list = std::array<color_col, 3>;

public:
  colorset_new(int colornum = 0) {
    fillzero();
    _colorCount = colornum;
  }

  inline int colorCount() const { return _colorCount; }

  inline auto rgb(int channel) const { return _rgb[channel].head(_colorCount); }

  inline float RGB(const int r, const int c) const { return _rgb[c](r); }

  inline auto hsv(int channel) const { return _hsv[channel].head(_colorCount); }
  inline float HSV(const int r, const int c) const { return _rgb[c](r); }

  inline auto lab(int channel) const { return _lab[channel].head(_colorCount); }
  inline float Lab(const int r, const int c) const { return _rgb[c](r); }

  inline auto xyz(int channel) const { return _xyz[channel].head(_colorCount); }
  inline float XYZ(const int r, const int c) const { return _rgb[c](r); }

  template <typename = void> inline auto map() const {
    static_assert(
        is_not_optical,
        "Function colorset_new::map() is only valid when is_not_optical==true");
    return this->Map.head(_colorCount);
  }

  bool
  applyAllowed(const colorset_new<true, is_not_optical, max_color_count> &src,
               const bool *const MIndex) {
    if (MIndex == nullptr) {
      return false;
    }

    Eigen::Map<const Eigen::Array<bool, Dynamic, 1>> allow_list(
        MIndex, max_color_count);

    // const int new_colornum = (allow_list == true).count();
    int new_colornum = 0;
    for (int idx = 0; idx < max_color_count; idx++) {
      new_colornum += (idx & 0b111111) && MIndex[idx];
    }

    fillzero();

    _colorCount = new_colornum;
    for (int readrow = 0, writerow = 0; readrow < max_color_count; readrow++) {
      const int base = readrow & 0b111111; //  readrow means index
      if (base == 0) {
        continue;
      }

      for (int c = 0; c < 3; c++) {

        _rgb[c][writerow] = src._RGB(readrow, c);
        _hsv[c][writerow] = src.HSV(readrow, c);
        _lab[c][writerow] = src.Lab(readrow, c);
        _xyz[c][writerow] = src.XYZ(readrow, c);
      }
      if constexpr (is_not_optical) {
        this->Map[writerow] = src.Map[readrow];
      }
      writerow++;
    }

    if constexpr (is_not_optical) {
      update_depthcount();
    }

    return true;
  }

  color_list _rgb;
  color_list _hsv;
  color_list _lab;
  color_list _xyz;
  // another member Eigen::Array<uint8_t, 256, 1> Map is brought by inheritage

private:
  int _colorCount;

  void fillzero() {
    for (int c = 0; c < 3; c++) {
      _rgb[c].setZero();
      _hsv[c].setZero();
      _hsv[c].setZero();
      _xyz[c].setZero();
    }
    if constexpr (is_not_optical) {
      this->Map.setZero();
    }
  }

  template <typename = void> void update_depthcount() {
    static_assert(is_not_optical, "");
    static_assert(max_color_count == 256, "");

    newTokiColorBase<true>::DepthCount[0] = 0;
    newTokiColorBase<true>::DepthCount[1] = 0;
    newTokiColorBase<true>::DepthCount[2] = 0;
    newTokiColorBase<true>::DepthCount[3] = 0;
    for (int idx = 0; idx < this->colorCount(); idx++) {
      const uint8_t mapcolor = this->Map[idx];
      const uint8_t base = mapcolor >> 2;
      if (base != 0) {
        const uint8_t depth = mapcolor & 0b11;
        newTokiColorBase<true>::DepthCount[depth]++;
      }
    }
  }
};

/// used for basic color set (colorsource), usually works as global constant
/// object.
template <bool is_not_optical, int color_count>
class colorset_new<true, is_not_optical, color_count>
    : public std::conditional_t<is_not_optical, colorset_u8array,
                                heu_empty_struct00> {
  static_assert(color_count > 0, "");

public:
  using color_mat = Eigen::Array<float, color_count, 3>;
  color_mat _RGB;
  color_mat HSV;
  color_mat Lab;
  color_mat XYZ;

  colorset_new() = delete;
  /// initialize from
  colorset_new(const float *const rgbsrc) {
    if (rgbsrc == nullptr) {
      exit(1);
      return;
    }
    _RGB.setZero();
    HSV.setZero();
    Lab.setZero();
    XYZ.setZero();

    for (int i = 0; i < color_count * 3; i++) {
      _RGB(i) = rgbsrc[i];
    }

    for (int row = 0; row < 256; row++) {
      const float r = _RGB(row, 0), g = _RGB(row, 1), b = _RGB(row, 2);
      RGB2HSV(r, g, b, HSV(row, 0), HSV(row, 1), HSV(row, 2));
      RGB2XYZ(r, g, b, XYZ(row, 0), XYZ(row, 1), XYZ(row, 2));
      XYZ2Lab(XYZ(row, 0), XYZ(row, 1), XYZ(row, 2), Lab(row, 0), Lab(row, 1),
              Lab(row, 2));
    }
  }
  constexpr int colorCount() const { return color_count; }

  inline float color_value(const SlopeCraft::convertAlgo algo, const int r,
                           const int c) const {
    switch (algo) {
    case SlopeCraft::convertAlgo::gaCvter:
    case SlopeCraft::convertAlgo::RGB:
    case SlopeCraft::convertAlgo::RGB_Better:
      return _RGB(r, c);

    case SlopeCraft::convertAlgo::HSV:
      return HSV(r, c);
    case SlopeCraft::convertAlgo::Lab94:
    case SlopeCraft::convertAlgo::Lab00:
      return Lab(r, c);
    case SlopeCraft::convertAlgo::XYZ:
      return XYZ(r, c);
    }
    return NAN;
  }
};

#endif // SCL_NEWCOLORSET_HPP