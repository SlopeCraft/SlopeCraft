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

#ifndef COLORMANIP_IMAGECONVERT_HPP
#define COLORMANIP_IMAGECONVERT_HPP

#include <omp.h>
#include <uiPack/uiPack.h>

#include <Eigen/Dense>
#include <GPU_interface.h>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <cereal/cereal.hpp>

#include "../SC_GlobalEnums.h"
#include "ColorManip.h"
#include "newColorSet.hpp"
#include "newTokiColor.hpp"

#ifdef RGB
#undef RGB
#endif  // #ifdef RGB

namespace libImageCvt {

using ::Eigen::Dynamic;

static const Eigen::Array<float, 2, 3> dithermap_LR(
    {{0.0 / 16.0, 0.0 / 16.0, 7.0 / 16.0},
     {3.0 / 16.0, 5.0 / 16.0, 1.0 / 16.0}});

static const Eigen::Array<float, 2, 3> dithermap_RL(
    {{7.0 / 16.0, 0.0 / 16.0, 0.0 / 16.0},
     {1.0 / 16.0, 5.0 / 16.0, 3.0 / 16.0}});

template <bool is_not_optical>
struct GPU_wrapper_wrapper {
  constexpr bool have_gpu_resource() const noexcept { return false; }
};

template <>
struct GPU_wrapper_wrapper<false> {
 protected:
  gpu_wrapper::gpu_interface *gpu{nullptr};

 public:
  bool have_gpu_resource() const noexcept { return this->gpu != nullptr; }

  void set_gpu_resource(gpu_wrapper::gpu_interface *gi) noexcept {
    this->gpu = gi;
  }

  inline gpu_wrapper::gpu_interface *gpu_resource() noexcept {
    return this->gpu;
  }
  inline const gpu_wrapper::gpu_interface *gpu_resource() const noexcept {
    return this->gpu;
  }
};

template <bool is_not_optical>
class ImageCvter : public GPU_wrapper_wrapper<is_not_optical> {
 public:
  using basic_colorset_t = colorset_new<true, is_not_optical>;
  using allowed_colorset_t = colorset_new<false, is_not_optical>;
  using TokiColor_t =
      newTokiColor<is_not_optical, basic_colorset_t, allowed_colorset_t>;
  using colorid_t = typename TokiColor_t::result_t;
  using coloridx_t = colorid_t;

  // These static member must be implemented by caller
  //  static const basic_colorset_t &basic_colorset;
  //  static const allowed_colorset_t &allowed_colorset;

  ImageCvter(const basic_colorset_t &basic, const allowed_colorset_t &allowed)
      : basic_colorset{basic}, allowed_colorset{allowed} {}

  ImageCvter(ImageCvter &&) = default;

 protected:
  const basic_colorset_t &basic_colorset;
  const allowed_colorset_t &allowed_colorset;
  Eigen::ArrayXX<ARGB> _raw_image;
  ::SCL_convertAlgo algo;
  bool dither{false};
  std::unordered_map<convert_unit, TokiColor_t, ::hash_cvt_unit> _color_hash;

  Eigen::ArrayXX<ARGB> _dithered_image;
  // Eigen::ArrayXX<colorid_t> colorid_matrix;

 public:
  uiPack ui;
  // SCL_convertAlgo convert_algo{SCL_convertAlgo::RGB_Better};

  inline void clear_images() noexcept {
    this->_raw_image.resize(0, 0);
    this->_dithered_image.resize(0, 0);
  }

  /// When the colorset is changed, the hash must be cleared.
  inline void on_color_set_changed() noexcept { this->clear_color_hash(); }

  /// Call this function when the color set is changed.
  inline void clear_color_hash() noexcept { this->_color_hash.clear(); }

  inline ::SCL_convertAlgo convert_algo() const noexcept { return this->algo; }

  inline bool is_dither() const noexcept { return this->dither; }

  inline int64_t rows() const noexcept { return _raw_image.rows(); }
  inline int64_t cols() const noexcept { return _raw_image.cols(); }
  inline int64_t size() const noexcept { return _raw_image.size(); }

  inline const auto &raw_image() const noexcept { return _raw_image; }

  inline const auto &color_hash() const noexcept { return _color_hash; }

  void set_raw_image(const ARGB *const data, const int64_t _rows,
                     const int64_t _cols,
                     const bool is_col_major = true) noexcept {
    if (_rows <= 0 || _cols <= 0) {
      return;
    }
    if (data == nullptr) {
      return;
    }

    if (is_col_major) {
      Eigen::Map<const Eigen::Array<ARGB, Dynamic, Dynamic, Eigen::ColMajor>>
          map(data, _rows, _cols);
      this->_raw_image = map;
    } else {
      Eigen::Map<const Eigen::Array<ARGB, Dynamic, Dynamic, Eigen::RowMajor>>
          map(data, _rows, _cols);
      this->_raw_image = map;
    }
  }

  bool convert_image(::SCL_convertAlgo __algo, bool _dither,
                     bool try_gpu = false) noexcept {
    if (__algo == ::SCL_convertAlgo::gaCvter) {
      __algo = ::SCL_convertAlgo::RGB_Better;
    }
    this->dither = _dither;

    ui.rangeSet(0, 100, 0);

    this->algo = __algo;
    this->add_colors_to_hash();
    ui.rangeSet(0, 100, 25);
    if (!this->match_all_TokiColors(try_gpu)) {
      return false;
    }
    ui.rangeSet(0, 100, 50);

    if (this->dither) {
      switch (this->algo) {
        case ::SCL_convertAlgo::RGB:
          this->template __impl_dither<::SCL_convertAlgo::RGB>();
          break;
        case ::SCL_convertAlgo::RGB_Better:
          this->template __impl_dither<::SCL_convertAlgo::RGB_Better>();
          break;
        case ::SCL_convertAlgo::HSV:
          this->template __impl_dither<::SCL_convertAlgo::HSV>();
          break;
        case ::SCL_convertAlgo::Lab94:
          this->template __impl_dither<::SCL_convertAlgo::Lab94>();
          break;
        case ::SCL_convertAlgo::Lab00:
          this->template __impl_dither<::SCL_convertAlgo::Lab00>();
          break;
        case ::SCL_convertAlgo::XYZ:
          this->template __impl_dither<::SCL_convertAlgo::XYZ>();
          break;

        default:
          abort();
          return false;
      }
    } else {
      this->_dithered_image = this->_raw_image;
    }

    //    for (int64_t idx = 0; idx < this->_dithered_image.size(); idx++) {
    //      const auto current_color{this->_dithered_image(idx)};
    //      if (getA(current_color) > 0) {
    //        continue;
    //      }
    //      const convert_unit key{current_color, this->algo};
    //      if (!this->_color_hash.contains(key)) {
    //        this->_color_hash.emplace(key, uint8_t{0});
    //      }
    //    }

    ui.rangeSet(0, 100, 100);
    return true;
    // fill_coloridmat_by_hash(this->colorid_matrix);
  }

  Eigen::ArrayXX<colorid_t> color_id() const noexcept {
    Eigen::ArrayXX<colorid_t> result;
    result.setZero(this->rows(), this->cols());

    for (int64_t idx = 0; idx < this->size(); idx++) {
      const auto current_color = this->_dithered_image(idx);

      auto it = this->_color_hash.find(convert_unit(current_color, this->algo));

      if (it == this->_color_hash.end()) {
        if (getA(current_color) <= 0) {
          result(idx) = 0;
          continue;
        }

        abort();
      }

      result(idx) = it->second.color_id();
    }
    return result;
  }

  void color_id(Eigen::Map<Eigen::ArrayXX<colorid_t>> &result) const noexcept {
    assert(result.rows() == this->rows());
    assert(result.cols() == this->cols());
    // result.resize(this->rows(), this->cols());
    // memset(result.data(), 0, result.rows() * result.cols() *
    // sizeof(uint16_t));

    for (int64_t idx = 0; idx < this->size(); idx++) {
      auto it = this->_color_hash.find(
          convert_unit(this->_dithered_image(idx), this->algo));

      if (it == this->_color_hash.end()) {
        abort();
      }

      result(idx) = it->second.color_id();
    }
  }

  colorid_t color_id(int64_t r, int64_t c) const noexcept {
    assert(r >= 0 && r < this->rows());
    assert(c >= 0 && c < this->cols());

    const auto current_color = this->_dithered_image(r, c);
    auto it = this->_color_hash.find(convert_unit{current_color, this->algo});
    if (it == this->_color_hash.end()) {
      if (getA(current_color) > 0) {
        abort();
      }
      return 0;
    }
    return it->second.color_id();
  }

  inline void converted_image(Eigen::ArrayXX<ARGB> &dest) const noexcept {
    dest.setZero(this->rows(), this->cols());

    converted_image(dest.data());
  }

  inline void converted_image(
      ARGB *const data_dest, int64_t *const rows_dest = nullptr,
      int64_t *const cols_dest = nullptr,
      const bool is_dest_col_major = true) const noexcept {
    if (rows_dest != nullptr) {
      *rows_dest = this->rows();
    }
    if (cols_dest != nullptr) {
      *cols_dest = this->cols();
    }

    if (data_dest != nullptr) {
      for (int64_t r = 0; r < rows(); r++) {
        for (int64_t c = 0; c < cols(); c++) {
          const int64_t idx =
              (is_dest_col_major) ? (c * rows() + r) : (r * cols() + c);
          const ARGB argb = this->_dithered_image(r, c);
          auto it = this->_color_hash.find(convert_unit(argb, this->algo));
          if (it == this->_color_hash.end()) {
            abort();
            return;
          }

          const auto color_id = it->second.color_id();
          const auto color_index =
              basic_colorset.colorindex_of_colorid(color_id);
          if (color_index != allowed_colorset_t::invalid_color_id) {
            data_dest[idx] = RGB2ARGB(basic_colorset.RGB(color_index, 0),
                                      basic_colorset.RGB(color_index, 1),
                                      basic_colorset.RGB(color_index, 2));
          } else {
            data_dest[idx] = 0x00'00'00'00;
          }
        }
      }
    }
  }

 private:
  void add_colors_to_hash() noexcept {
    // this->_color_hash.clear();

    for (int64_t idx = 0; idx < this->_raw_image.size(); idx++) {
      const ARGB argb = this->_raw_image(idx);
      convert_unit cu(argb, this->algo);
      auto it = _color_hash.find(cu);

      // this key isn't inserted
      if (it == _color_hash.end()) this->_color_hash.emplace(cu, TokiColor_t());
    }
  }

  bool match_all_TokiColors(bool try_gpu) noexcept {
    if constexpr (is_not_optical) {
      this->match_all_TokiColors_cpu();
      return true;
    } else {
      if constexpr (gpu_wrapper::have_api) {
        // If converter have gpu resources, compute by gpu
        if (try_gpu && this->have_gpu_resource()) {
          const bool ok = this->match_all_TokiColors_gpu();

          return ok;
        }
      }
      // otherwise compute by cpu
      this->match_all_TokiColors_cpu();
      return true;
    }
  }

  void match_all_TokiColors_cpu() noexcept {
    // const int threadCount = omp_get_num_threads();

    std::vector<std::pair<const convert_unit, TokiColor_t> *> tasks;
    tasks.reserve(_color_hash.size());
    tasks.clear();

    for (auto &pair : _color_hash) {
      if (!pair.second.is_result_computed()) tasks.emplace_back(&pair);
    }
    const size_t taskCount = tasks.size();

#pragma omp parallel for schedule(dynamic)
    for (int taskIdx = 0; taskIdx < (int)taskCount; taskIdx++) {
      tasks[taskIdx]->second.compute(tasks[taskIdx]->first,
                                     this->allowed_colorset);
    }
    // #warning we should parallelize here
    /*
#pragma omp parallel for schedule(dynamic)
    for (int thIdx = 0; thIdx < threadCount; thIdx++) {
      for (uint64_t taskIdx = thIdx; taskIdx < taskCount;
           taskIdx += threadCount) {
      tasks[taskIdx]->second.compute(tasks[taskIdx]->first);
      }
    }
  */
  }

  /// fill a colorid matrix according to raw image and colorhash
  void fill_coloridmat_by_hash(Eigen::ArrayXX<colorid_t> &dest) const noexcept {
    dest.setZero(this->rows(), this->cols());

#pragma omp parallel for
    for (int64_t c = 0; c < this->cols(); c++) {
      for (int64_t r = 0; r < this->rows(); r++) {
        auto it = this->_color_hash.find(this->_raw_image(r, c));
        if (it == this->_color_hash.end()) {
          abort();
        }

        if constexpr (is_not_optical) {
          dest(r, c) = it->Result;
        } else {
          dest(r, c) = it->result_color_id;
        }
      }
    }
  }

  template <typename = void>
  bool match_all_TokiColors_gpu() noexcept {
    static_assert(!is_not_optical,
                  "GPU boosting is only avaliable for VisualCraftL.");
    // static_assert(gpu_wrapper::have_api, "No avaliable GPU api.");

    if (!this->have_gpu_resource()) {
      abort();
      return false;
    }

    std::vector<std::pair<const convert_unit, TokiColor_t> *> tasks;
    tasks.reserve(_color_hash.size());
    tasks.clear();

    for (auto &pair : this->_color_hash) {
      if (!pair.second.is_result_computed()) {
        if ((pair.first._ARGB & 0xFF'00'00'00) == 0) {
          pair.second.compute(pair.first, this->allowed_colorset);
        } else {
          tasks.emplace_back(&pair);
        }
      }
    }

    if (tasks.size() <= 0) {
      return true;
    }

    const SCL_convertAlgo algo = tasks[0]->first.algo;

    const uint64_t taskCount = tasks.size();

    const uint64_t gpu_task_count =
        taskCount - taskCount % this->gpu->local_work_group_size_v();
    const uint64_t cpu_task_count = taskCount - gpu_task_count;

    if (gpu_task_count > 0) {
      std::vector<std::array<float, 3>> task_colors(gpu_task_count);
      for (size_t tid = 0; tid < gpu_task_count; tid++) {
        if (tasks[tid]->first.algo != algo) {
          return false;
        }

        const Eigen::Array3f c3_eig = tasks[tid]->first.to_c3();
        for (size_t channel = 0; channel < 3; channel++) {
          task_colors[tid][channel] = c3_eig[channel];
        }
      }

      //  set task
      this->gpu->set_task_v(task_colors.size(), task_colors.data());
      if (!this->gpu->ok_v()) {
        return false;
      }
    }

    // set colorset to device

    std::array<const float *, 3> colorset_ptrs{nullptr, nullptr, nullptr};
    switch (algo) {
      case SCL_convertAlgo::RGB:
      case SCL_convertAlgo::RGB_Better:
        colorset_ptrs = {this->allowed_colorset.rgb_data(0),
                         this->allowed_colorset.rgb_data(1),
                         this->allowed_colorset.rgb_data(2)};
        break;
      case SCL_convertAlgo::HSV:
        colorset_ptrs = {this->allowed_colorset.hsv_data(0),
                         this->allowed_colorset.hsv_data(1),
                         this->allowed_colorset.hsv_data(2)};
        break;
      case SCL_convertAlgo::Lab94:
      case SCL_convertAlgo::Lab00:
        colorset_ptrs = {this->allowed_colorset.lab_data(0),
                         this->allowed_colorset.lab_data(1),
                         this->allowed_colorset.lab_data(2)};
        break;
      case SCL_convertAlgo::XYZ:
        colorset_ptrs = {this->allowed_colorset.xyz_data(0),
                         this->allowed_colorset.xyz_data(1),
                         this->allowed_colorset.xyz_data(2)};
        break;
      default:
        abort();
    }

    if (gpu_task_count > 0) {
      // set colorset for ocl
      this->gpu->set_colorset_v(this->allowed_colorset.color_count(),
                                colorset_ptrs);
      if (!this->gpu->ok_v()) {
        return false;
      }

      this->gpu->execute_v(algo, false);
      if (!this->gpu->ok_v()) {
        return false;
      }
    }
    // compute rest tasks on cpu
    for (uint64_t ctid = 0; ctid < cpu_task_count; ctid++) {
      const uint64_t tid = gpu_task_count + ctid;
      tasks[tid]->second.compute(tasks[tid]->first, this->allowed_colorset);
    }

    if (gpu_task_count > 0) {
      this->gpu->wait_v();
      if (!this->gpu->ok_v()) {
        return false;
      }
    }

    if (gpu_task_count > 0)
      for (size_t tid = 0; tid < gpu_task_count; tid++) {
        TokiColor_t &tc = tasks[tid]->second;

        const uint16_t tempidx = this->gpu->result_idx_v()[tid];
        if (tempidx >= this->allowed_colorset.color_count()) {
          abort();
        }

        tc.set_gpu_result(this->allowed_colorset.color_id(tempidx),
                          this->gpu->result_diff_v()[tid]);
      }

    return true;
  }

  template <SCL_convertAlgo algo>
  inline static ARGB ColorCvt(float c0, float c1, float c2) noexcept {
    switch (algo) {
      case ::SCL_convertAlgo::RGB:
      case ::SCL_convertAlgo::RGB_Better:
      case ::SCL_convertAlgo::gaCvter:
        return RGB2ARGB(c0, c1, c2);
      case ::SCL_convertAlgo::HSV:
        return HSV2ARGB(c0, c1, c2);
      case ::SCL_convertAlgo::Lab00:
      case ::SCL_convertAlgo::Lab94:
        return Lab2ARGB(c0, c1, c2);
      case ::SCL_convertAlgo::XYZ:
        return XYZ2ARGB(c0, c1, c2);
    }
    // unreachable
    abort();
    return 0;
  }

  template <SCL_convertAlgo cvt_algo>
  void __impl_dither() noexcept {
    std::array<Eigen::ArrayXXf, 3> dither_c3;
    for (auto &i : dither_c3) {
      i.setZero(this->rows() + 2, this->cols() + 2);
    }

    // dest.setZero(this->rows(), this->cols());
    this->_dithered_image.setZero(this->rows(), this->cols());

    for (int64_t r = 0; r < this->rows(); r++) {
      for (int64_t c = 0; c < this->cols(); c++) {
        auto it = this->_color_hash.find(
            convert_unit(this->_raw_image(r, c), this->algo));
        if (it == this->_color_hash.end()) {
          // unreachable
          abort();
          return;
        }
        for (int ch = 0; ch < 3; ch++) {
          dither_c3[ch](r + 1, c + 1) = it->first.to_c3()[ch];
        }
      }
    }

    // int64_t inserted_count = 0;
    bool is_dir_LR = true;
    for (int64_t row = 0; row < this->rows(); row++) {
      if (is_dir_LR)
        for (int64_t col = 0; col < this->cols(); col++) {
          if (::getA(this->_raw_image(row, col)) <= 0) {
            continue;
          }

          const ARGB current_argb = ColorCvt<cvt_algo>(
              dither_c3[0](row + 1, col + 1), dither_c3[1](row + 1, col + 1),
              dither_c3[2](row + 1, col + 1));
          // ditheredImage(r, c) = Current;
          this->_dithered_image(row, col) = current_argb;
          auto it_to_old_color =
              this->_color_hash.find(convert_unit(current_argb, this->algo));
          // if this color isn't matched, match it.
          if (it_to_old_color == this->_color_hash.end()) {
            convert_unit cu(current_argb, this->algo);
            auto ret = this->_color_hash.emplace(cu, TokiColor_t());
            it_to_old_color = ret.first;
            it_to_old_color->second.compute(cu, this->allowed_colorset);
            // inserted_count++;
          }

          TokiColor_t &old_color = it_to_old_color->second;
          // mapPic(r, c) = oldColor.Result;

          coloridx_t coloridx;
          if constexpr (is_not_optical) {
            coloridx = basic_colorset.colorindex_of_colorid(old_color.Result);
          } else {
            coloridx =
                basic_colorset.colorindex_of_colorid(old_color.color_id());
          }

          for (int ch = 0; ch < 3; ch++) {
            const float color_error =
                it_to_old_color->first.to_c3()[ch] -
                basic_colorset.color_value(cvt_algo, coloridx, ch);
            dither_c3[ch].block<2, 3>(row + 1, col + 1 - 1) +=
                color_error * dithermap_LR;
          }
        }
      else
        for (int64_t col = this->cols() - 1; col >= 0; col--) {
          if (::getA(this->_raw_image(row, col)) <= 0) {
            continue;
          }

          const ARGB current_argb = ColorCvt<cvt_algo>(
              dither_c3[0](row + 1, col + 1), dither_c3[1](row + 1, col + 1),
              dither_c3[2](row + 1, col + 1));
          this->_dithered_image(row, col) = current_argb;
          convert_unit cu(current_argb, this->algo);
          auto it_to_old_color = this->_color_hash.find(cu);
          // if this color isn't matched, match it.
          if (it_to_old_color == this->_color_hash.end()) {
            auto ret = this->_color_hash.emplace(cu, TokiColor_t());
            it_to_old_color = ret.first;
            it_to_old_color->second.compute(cu, this->allowed_colorset);
            // inserted_count++;
          }

          TokiColor_t &old_color = it_to_old_color->second;
          // mapPic(r, c) = oldColor.Result;

          coloridx_t coloridx;
          if constexpr (is_not_optical) {
            coloridx = basic_colorset.colorindex_of_colorid(old_color.Result);
          } else {
            coloridx =
                basic_colorset.colorindex_of_colorid(old_color.color_id());
          }

          for (int ch = 0; ch < 3; ch++) {
            const float color_error =
                it_to_old_color->first.to_c3()[ch] -
                basic_colorset.color_value(cvt_algo, coloridx, ch);
            dither_c3[ch].block<2, 3>(row + 1, col + 1 - 1) +=
                color_error * dithermap_RL;
          }
        }

      is_dir_LR = !is_dir_LR;

      // report
    }
    return;
  }

 public:
  [[deprecated]] uint64_t task_hash() const noexcept {
    return this->task_hash(this->algo, this->dither);
  }

  uint64_t task_hash(SCL_convertAlgo a, bool d) const noexcept {
    const auto &img = this->_raw_image;
    return std::hash<std::string_view>()(

               std::string_view{(const char *)img.data(),
                                img.size() * sizeof(uint32_t)}) ^
           std::hash<char>()((char)a) ^ std::hash<bool>()(d);
  }
};

std::vector<uint8_t> hash_of_image(
    Eigen::Map<const Eigen::ArrayXX<ARGB>> img) noexcept;

}  // namespace libImageCvt

#endif  // COLORMANIP_IMAGECONVERT_HPP
