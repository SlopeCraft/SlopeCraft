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

#ifndef SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H
#define SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H
#ifdef SLOPECRAFT_NO_CL_HPP
#include <CL/opencl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "../GPU_interface.h"
#include <array>
#include <assert.h>
#include <string>
#include <utilities/SC_GlobalEnums.h>

namespace ocl_warpper {

size_t platform_num() noexcept;

class ocl_resource : public ::gpu_wrapper::gpu_interface {
 public:
  struct task_rcs {
    // std::vector<uint32_t> buf_unconverted_ARGB_host;
    cl::Buffer rawcolor_f32_3_device;
    std::vector<uint16_t> result_idx_u16_host;
    cl::Buffer result_idx_u16_device;
    std::vector<float> result_diff_f32_host;
    cl::Buffer result_diff_f32_device;
  };

  struct colorset_rcs {
    cl::Buffer colorset_float3;
    uint16_t colorset_color_num{0};
  };

 private:
  cl_int error{CL_SUCCESS};
  std::string err_msg{""};
  cl::Platform platform;
  cl::Device device;
  cl::Context context;
  cl::CommandQueue queue;
  cl::Program program;

  cl::Kernel k_RGB;
  cl::Kernel k_RGB_Better;
  cl::Kernel k_HSV;
  cl::Kernel k_Lab94;
  cl::Kernel k_Lab00;
  cl::Kernel k_XYZ;

  task_rcs task;
  colorset_rcs colorset;

  // buffers to for colorset

 private:
  void init_resource() noexcept;

 public:
  inline int error_code() const noexcept { return this->error; }
  inline bool ok() const noexcept { return (this->error == CL_SUCCESS); }

  inline const std::string &error_detail() const noexcept {
    return this->err_msg;
  }

 public:
  ocl_resource();

  ocl_resource(cl::Platform plat, cl::Device dev);

  void set_colorset(size_t color_num,
                    const std::array<const float *, 3> &color_ptrs) noexcept;

  void set_task(size_t task_num, const std::array<float, 3> *data) noexcept;

  void execute(::SCL_convertAlgo algo, bool wait) noexcept;

  void wait() noexcept;

  inline size_t task_count() const noexcept {
    assert(this->task.result_diff_f32_host.size() ==
           this->task.result_idx_u16_host.size());
    return this->task.result_diff_f32_host.size();
  }

  inline std::string device_vendor() const noexcept {
    return this->device.getInfo<CL_DEVICE_VENDOR>();
  }

  inline const auto &result_idx() const noexcept {
    return this->task.result_idx_u16_host;
  }

  inline const auto &result_diff() const noexcept {
    return this->task.result_diff_f32_host;
  }

  constexpr size_t local_work_group_size() const noexcept { return 32; }

  size_t local_work_group_size_v() const noexcept override {
    return this->local_work_group_size();
  }

 private:
  void resize_task(size_t task_num) noexcept;
  void resize_colorset(size_t color_num) noexcept;

  cl::Kernel *kernel_by_algo(::SCL_convertAlgo algo) noexcept;

  void set_args(::SCL_convertAlgo algo) noexcept;

 public:
  // overrided functions

  const char *api_v() const noexcept override { return "OpenCL"; }

  int error_code_v() const noexcept override { return this->error_code(); }
  bool ok_v() const noexcept override { return this->ok(); }

  std::string error_detail_v() const noexcept override {
    return this->error_detail();
  };

  void set_colorset_v(
      size_t color_num,
      const std::array<const float *, 3> &color_ptrs) noexcept override {
    this->set_colorset(color_num, color_ptrs);
  }

  void set_task_v(size_t task_num,
                  const std::array<float, 3> *data) noexcept override {
    this->set_task(task_num, data);
  }

  void execute_v(::SCL_convertAlgo algo, bool wait) noexcept override {
    this->execute(algo, wait);
  }

  void wait_v() noexcept override { this->wait(); }

  size_t task_count_v() const noexcept override { return this->task_count(); }

  std::string device_vendor_v() const noexcept override {
    return this->device_vendor();
  }

  const uint16_t *result_idx_v() const noexcept override {
    return this->result_idx().data();
  }

  const float *result_diff_v() const noexcept override {
    return this->result_diff().data();
  }
};

class ocl_platform : public ::gpu_wrapper::platform_wrapper {
 public:
  ocl_platform() = delete;
  ocl_platform(size_t idx);
  cl::Platform platform;
  std::string name;

  std::vector<cl::Device> devices;

  cl_int err{0};

  const char *name_v() const noexcept override { return this->name.c_str(); };
  size_t num_devices_v() const noexcept override {
    return this->devices.size();
  }
};

class ocl_device : public ::gpu_wrapper::device_wrapper {
 public:
  ocl_device() = delete;
  ocl_device(cl::Device);

  cl::Device device;
  std::string name;
  cl_int err{0};

  const char *name_v() const noexcept override { return this->name.c_str(); }
};

}  // namespace ocl_warpper

#endif  // SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H