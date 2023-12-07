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

#include "OCLWrapper.h"
#include "../GPU_interface.h"

#include <Eigen/Dense>
#include <stdlib.h>
#include <utilities/SC_GlobalEnums.h>
#include <fmt/format.h>

extern "C" {
extern const unsigned char ColorManip_cl_rc[];
extern const unsigned int ColorManip_cl_rc_length;
}

size_t ocl_warpper::platform_num() noexcept {
  cl_uint ret;
  cl_int err;

  err = clGetPlatformIDs(0, nullptr, &ret);

  if (err != CL_SUCCESS) {
    return 0;
  }
  return ret;
}

cl::Platform private_fun_get_platform(size_t platform_idx,
                                      cl_int &err) noexcept {
  constexpr size_t buffersize = 128;

  cl_platform_id plats[buffersize];

  cl_uint num_plats;

  err = clGetPlatformIDs(buffersize, plats, &num_plats);

  if (err != CL_SUCCESS) {
    return {};
  }

  if (platform_idx >= num_plats) {
    return {};
  }

  return cl::Platform(plats[platform_idx]);
}

ocl_warpper::ocl_platform::ocl_platform(size_t idx) {
  this->platform = private_fun_get_platform(idx, this->err);
  if (this->err != CL_SUCCESS) return;

  this->name = this->platform.getInfo<CL_PLATFORM_NAME>(&this->err);
  if (this->err != CL_SUCCESS) return;

  this->err = this->platform.getDevices(CL_DEVICE_TYPE_ALL, &this->devices);
}

cl_int private_fun_change_buf_size(cl::Context &context, cl::Buffer &buf,
                                   size_t bytes_required, cl_mem_flags flags,
                                   bool initiaze_buf) noexcept;

ocl_warpper::ocl_resource::ocl_resource() {
  this->platform = cl::Platform::getDefault(&error);
  if (!this->ok()) {
    return;
  }

  std::vector<cl::Device> devs;
  this->platform.getDevices(CL_DEVICE_TYPE_ALL, &devs);
  if (!this->ok()) {
    return;
  }

  this->device = devs[0];

  this->init_resource();
}

ocl_warpper::ocl_resource::ocl_resource(cl::Platform plat, cl::Device dev) {
  this->platform = plat;
  this->device = dev;

  this->init_resource();
}

void ocl_warpper::ocl_resource::init_resource() noexcept {
  this->context = cl::Context(this->device, NULL, NULL, NULL, &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create context.";
    return;
  }

  this->queue = cl::CommandQueue(this->context, this->device,
                                 cl::QueueProperties::None, &this->error);

  if (!this->ok()) {
    this->err_msg = "Failed to create command queue.";
    return;
  }

// Here the code differs according to if there is cl.hpp. API to create
// cl::Program is different in cl.hpp and opencl.hpp. I don't know why there are
// 2 differernt opencl C++ bindings.
#ifndef SLOPECRAFT_NO_CL_HPP
  // This works with cl.hpp
  std::pair<const char *, size_t> src;
  src.first = (const char *)ColorManip_cl_rc;
  src.second = ColorManip_cl_rc_length;
  this->program = cl::Program{this->context, {src}, &this->error};
#else
  // This is for opencl.hpp
  std::string source_code{(const char *)ColorManip_cl_rc,
                          ColorManip_cl_rc_length};
  source_code.push_back('\0');
  this->program = cl::Program{
      this->context, std::vector<std::string>{source_code}, &this->error};
#endif

  if (!this->ok()) {
    this->err_msg = "Failed to create program with source files.";
    return;
  }

  this->error = this->program.build(
      "-cl-mad-enable -cl-unsafe-math-optimizations "
      "-cl-single-precision-constant",
      nullptr, nullptr);
  if (!this->ok()) {
    cl_int ec_get_build_log{CL_SUCCESS};
    auto build_log = this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(
        this->device, &ec_get_build_log);
    if (ec_get_build_log == CL_SUCCESS) {
      this->err_msg =
          fmt::format("Failed to build program. Build log:\n{}", build_log);
    } else {
      this->err_msg = fmt::format(
          "Failed to build program, and then failed to retrieve build log with "
          "error code {}",
          ec_get_build_log);
    }
    return;
  }

  this->k_RGB = cl::Kernel(this->program, "match_color_RGB", &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create kernel (match_color_RGB).";
    return;
  }

  this->k_RGB_Better =
      cl::Kernel(this->program, "match_color_RGB_Better", &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create kernel (match_color_Better).";
    return;
  }

  this->k_HSV = cl::Kernel(this->program, "match_color_HSV", &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create kernel (match_color_HSV).";
    return;
  }

  this->k_Lab94 = cl::Kernel(this->program, "match_color_Lab94", &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create kernel (match_color_Lab94).";
    return;
  }

  this->k_Lab00 = cl::Kernel(this->program, "match_color_Lab00", &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create kernel (match_color_Lab00).";
    return;
  }

  this->k_XYZ = cl::Kernel(this->program, "match_color_XYZ", &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create kernel (match_color_XYZ).";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.rawcolor_f32_3_device, 128 * sizeof(float[3]),
      CL_MEM_READ_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to initialize this->task.rawcolor_f32_3_device.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_idx_u16_device, 128 * sizeof(uint16_t),
      CL_MEM_WRITE_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to initialize this->task.result_idx_u16_device.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_diff_f32_device, 128 * sizeof(float),
      CL_MEM_WRITE_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to initialize this->task.result_diff_f32_device.";
    return;
  }

  // initialize buffers for colorset
  this->colorset.colorset_float3 =
      cl::Buffer(this->context, CL_MEM_READ_ONLY, 256 * sizeof(float) * 3, NULL,
                 &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to initialize this->colorset.colorset_float3.";
    return;
  }
}

void ocl_warpper::ocl_resource::resize_task(size_t task_num) noexcept {
  // this->buf_unconverted_ARGB_host.resize(task_num);
  this->task.result_idx_u16_host.resize(task_num);
  this->task.result_diff_f32_host.resize(task_num);

  for (uint16_t &val : this->task.result_idx_u16_host) {
    val = UINT16_MAX;
  }
  for (float &val : this->task.result_diff_f32_host) {
    val = NAN;
  }

  const size_t task_f32_3_required_bytes = task_num * sizeof(float[3]);
  const size_t result_idx_required_bytes = task_num * sizeof(uint16_t);
  const size_t result_diff_required_bytes = task_num * sizeof(float);

  this->error = private_fun_change_buf_size(
      this->context, this->task.rawcolor_f32_3_device,
      task_f32_3_required_bytes, CL_MEM_READ_ONLY, false);
  if (!this->ok()) {
    this->err_msg =
        "Failed to allocate device memory for "
        "this->task.rawcolor_f32_3_device.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_idx_u16_device,
      result_idx_required_bytes, CL_MEM_WRITE_ONLY, false);
  if (!this->ok()) {
    this->err_msg =
        "Failed to allocate device memory for "
        "this->task.result_idx_u16_device.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_diff_f32_device,
      result_diff_required_bytes, CL_MEM_WRITE_ONLY, false);
  if (!this->ok()) {
    this->err_msg =
        "Failed to allocate device memory for "
        "this->task.result_diff_f32_device.";
    return;
  }
  return;
}

void ocl_warpper::ocl_resource::resize_colorset(size_t color_num) noexcept {
  this->error = private_fun_change_buf_size(
      this->context, this->colorset.colorset_float3,
      color_num * 3 * sizeof(float), CL_MEM_READ_ONLY, false);
  if (!this->ok()) return;

  this->colorset.colorset_color_num = color_num;
}

void ocl_warpper::ocl_resource::set_colorset(
    size_t color_num, const std::array<const float *, 3> &color_ptrs) noexcept {
  this->resize_colorset(color_num);
  if (!this->ok()) {
    this->err_msg = "Failed to resize colorset.";
    return;
  }

  Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> trans(
      color_num, 3);
  for (size_t r = 0; r < color_num; r++) {
    for (size_t c = 0; c < 3; c++) {
      trans(r, c) = color_ptrs[c][r];
    }
  }

  // trans.transposeInPlace();

  this->error = this->queue.enqueueWriteBuffer(
      this->colorset.colorset_float3, false, 0, color_num * sizeof(float[3]),
      trans.data());
  if (!this->ok()) {
    this->err_msg = "Failed to copy colorset into device.";
    return;
  }

  this->error = this->queue.finish();
  if (!this->ok()) {
    this->err_msg = "Failed to wait for queue.";
    return;
  }
}

cl::Kernel *ocl_warpper::ocl_resource::kernel_by_algo(
    ::SCL_convertAlgo algo) noexcept {
  switch (algo) {
    case SCL_convertAlgo::RGB:
      return &this->k_RGB;
    case SCL_convertAlgo::RGB_Better:
      return &this->k_RGB_Better;
    case SCL_convertAlgo::HSV:
      return &this->k_HSV;
    case SCL_convertAlgo::Lab94:
      return &this->k_Lab94;
    case SCL_convertAlgo::Lab00:
      return &this->k_Lab00;
    case SCL_convertAlgo::XYZ:
      return &this->k_XYZ;
    default:
      return nullptr;
  }
  return nullptr;
}

void ocl_warpper::ocl_resource::set_task(
    size_t task_num, const std::array<float, 3> *data) noexcept {
  this->resize_task(task_num);
  if (!this->ok()) {
    return;
  }

  this->error =
      this->queue.enqueueWriteBuffer(this->task.rawcolor_f32_3_device, false, 0,
                                     task_num * sizeof(float) * 3, data);
  if (!this->ok()) {
    this->err_msg = "Failed to write unconverted colors to device.";
    this->err_msg += " task num = " + std::to_string(task_num);
    return;
  }

  // fill with 65535
  this->error = this->queue.enqueueFillBuffer(this->task.result_idx_u16_device,
                                              uint16_t(0xFFFF), 0,
                                              task_num * sizeof(uint16_t));
  if (!this->ok()) {
    this->err_msg =
        "Failed to fill this->task.result_idx_u16_device with 0xFFFF.";
    return;
  }

  // fill with nan
  /*
  this->error =
      this->queue.enqueueFillBuffer(this->task.result_diff_f32_device,
                                    float(NAN), 0, task_num * sizeof(float));
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }*/

  this->error = this->queue.finish();
  if (!this->ok()) {
    this->err_msg = "Failed to wait for queue.";
    return;
  }
}

/*
void ocl_warpper::ocl_resource::set_task(const std::array<float, 3> *src,
                                         size_t task_num) noexcept {
  this->resize_task(task_num);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error =
      this->queue.enqueueWriteBuffer(this->task.rawcolor_f32_3_device, true, 0,
                                     task_num * sizeof(float[3]), src->data());
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }
  // fill with 65536
  this->error = this->queue.enqueueFillBuffer(this->task.result_idx_u16_device,
                                              uint16_t(0xFFFF), 0,
                                              task_num * sizeof(uint16_t));
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  // fill with nan
  this->error =
      this->queue.enqueueFillBuffer(this->task.result_diff_f32_device,
                                    float(NAN), 0, task_num * sizeof(float));
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = this->queue.finish();
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }
}

*/

void ocl_warpper::ocl_resource::set_args(::SCL_convertAlgo algo) noexcept {
  this->wait();
  if (!this->ok()) {
    this->err_msg = "Failed to wait.";
    return;
  }

  cl::Kernel *k = this->kernel_by_algo(algo);

  // set kernel args
  // cl_mem mem = this->colorset.colorset_float3;
  this->error = k->setArg(0, this->colorset.colorset_float3);
  if (!this->ok()) {
    this->err_msg = "Failed to set arg0.";
    return;
  }

  this->error = k->setArg(1, this->colorset.colorset_color_num);
  if (!this->ok()) {
    this->err_msg = "Failed to set arg1.";
    return;
  }

  this->error = k->setArg(2, this->task.rawcolor_f32_3_device);
  if (!this->ok()) {
    this->err_msg = "Failed to set arg2.";
    return;
  }

  this->error = k->setArg(3, this->task.result_idx_u16_device);
  if (!this->ok()) {
    this->err_msg = "Failed to set arg3.";
    return;
  }

  this->error = k->setArg(4, this->task.result_diff_f32_device);
  if (!this->ok()) {
    this->err_msg = "Failed to set arg4.";
    return;
  }
}

cl_int private_fun_change_buf_size(cl::Context &context, cl::Buffer &buf,
                                   size_t bytes_required, cl_mem_flags flags,
                                   bool initiaze_buf) noexcept {
  cl_int err = CL_SUCCESS;
  if (!initiaze_buf) {
    const size_t bytes = buf.getInfo<CL_MEM_SIZE>(&err);
    if (err != CL_SUCCESS) {
      return err;
    }

    if (bytes >= bytes_required) {
      return CL_SUCCESS;
    }
  }
  // cl::Buffer replace(flags, bytes_required, NULL, &err);
  cl::Buffer replace(context, flags, bytes_required, NULL, &err);
  if (err != CL_SUCCESS) {
    return err;
  }

  buf = replace;

  return CL_SUCCESS;
}

void ocl_warpper::ocl_resource::execute(::SCL_convertAlgo algo,
                                        bool wait) noexcept {
  this->set_args(algo);
  if (!this->ok()) {
    this->err_msg = "Failed to set args.";
    return;
  }

  cl::Kernel *const k = this->kernel_by_algo(algo);

  this->error = this->queue.enqueueNDRangeKernel(
      *k, {0}, {this->task_count()}, {this->local_work_group_size()});
  if (!this->ok()) {
    this->err_msg = "Failed to execute kernel function.";
    return;
  }

  this->error =
      this->queue.enqueueReadBuffer(this->task.result_idx_u16_device, false, 0,
                                    this->task_count() * sizeof(uint16_t),
                                    this->task.result_idx_u16_host.data());
  if (!this->ok()) {
    this->err_msg = "Failed to read result index from device.";
    return;
  }

  this->error =
      this->queue.enqueueReadBuffer(this->task.result_diff_f32_device, false, 0,
                                    this->task_count() * sizeof(float),
                                    this->task.result_diff_f32_host.data());
  if (!this->ok()) {
    this->err_msg = "Failed to read result diff from device.";
    return;
  }

  if (wait) {
    this->wait();
  }
}

void ocl_warpper::ocl_resource::wait() noexcept {
  this->error = this->queue.finish();
  if (!this->ok()) {
    this->err_msg = "Failed to wait for queue.";
    return;
  }
}
