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

#ifndef SLOPECRAFT_UTILITIES_GPUWRAPPER_GPUINTERFACE_H
#define SLOPECRAFT_UTILITIES_GPUWRAPPER_GPUINTERFACE_H

#include "../SC_GlobalEnums.h"
#include <array>
#include <string>
#include <cstdint>
#include <cstddef>

namespace gpu_wrapper {

#ifdef SLOPECRAFT_GPU_API
static constexpr bool have_api = true;
#else
static constexpr bool have_api = false;
#endif  // #ifdef SLOPECRAFT_GPU_API

const char *api_name() noexcept;

size_t platform_num() noexcept;

class platform_wrapper {
 public:
  platform_wrapper() = default;
  virtual ~platform_wrapper() = default;

  virtual const char *name_v() const noexcept = 0;

  virtual size_t num_devices_v() const noexcept = 0;

  // virtual size_t index() const noexcept = 0;

  [[nodiscard]] static platform_wrapper *create(
      size_t idx, int *errorcode = nullptr) noexcept;
  static void destroy(platform_wrapper *) noexcept;
};

class device_wrapper {
 public:
  device_wrapper() = default;
  virtual ~device_wrapper() = default;

  virtual const char *name_v() const noexcept = 0;

  [[nodiscard]] static device_wrapper *create(
      platform_wrapper *pw, size_t idx, int *errorcode = nullptr) noexcept;
  static void destroy(device_wrapper *) noexcept;
};

class gpu_interface {
 public:
  gpu_interface() = default;
  virtual ~gpu_interface() = default;

  [[nodiscard]] static gpu_interface *create(platform_wrapper *pw,
                                             device_wrapper *dw) noexcept;
  [[nodiscard]] static gpu_interface *create(
      platform_wrapper *pw, device_wrapper *dw,
      std::pair<int, std::string> &err) noexcept;
  static void destroy(gpu_interface *) noexcept;

  virtual const char *api_v() const noexcept = 0;

  virtual int error_code_v() const noexcept = 0;
  virtual bool ok_v() const noexcept = 0;
  virtual std::string error_detail_v() const noexcept = 0;

  virtual void set_colorset_v(
      size_t color_num,
      const std::array<const float *, 3> &color_ptrs) noexcept = 0;

  virtual void set_task_v(size_t task_num,
                          const std::array<float, 3> *data) noexcept = 0;

  virtual void execute_v(::SCL_convertAlgo algo, bool wait) noexcept = 0;

  virtual void wait_v() noexcept = 0;

  virtual size_t task_count_v() const noexcept = 0;

  virtual std::string device_vendor_v() const noexcept = 0;

  virtual const uint16_t *result_idx_v() const noexcept = 0;

  virtual const float *result_diff_v() const noexcept = 0;

  virtual size_t local_work_group_size_v() const noexcept = 0;
};

}  // namespace gpu_wrapper

#endif  // SLOPECRAFT_UTILITIES_GPUWRAPPER_GPUINTERFACE_H