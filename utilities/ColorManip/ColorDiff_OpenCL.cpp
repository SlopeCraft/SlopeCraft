#include "ColorDiff_OpenCL.h"

#include <CL/cl.hpp>
#include <Eigen/Dense>
#include <stdlib.h>
#include <utilities/SC_GlobalEnums.h>

extern const unsigned char ColorManip_cl_rc[];
extern const unsigned int ColorManip_cl_rc_length;

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

std::string ocl_warpper::platform_str(size_t platform_idx) noexcept {
  cl_int err;
  cl::Platform plat = private_fun_get_platform(platform_idx, err);
  if (err != CL_SUCCESS) {
    return "Failed to get platform info, error code : " + std::to_string(err);
  }

  auto ret = plat.getInfo<CL_PLATFORM_NAME>(&err);
  if (err != CL_SUCCESS) {
    return "Failed to get platform info, error code : " + std::to_string(err);
  }
  return ret;
}

size_t ocl_warpper::device_num(size_t platform_idx) noexcept {

  cl_int err;
  cl::Platform plat = private_fun_get_platform(platform_idx, err);
  if (err != CL_SUCCESS) {
    return {};
  }

  std::vector<cl::Device> devices;

  err = plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);
  if (err != CL_SUCCESS) {
    return 0;
  }

  return devices.size();
}

cl::Device private_fun_get_device(size_t platform_idx, size_t device_idx,
                                  cl_int &err) noexcept {

  auto plat = private_fun_get_platform(platform_idx, err);
  if (err != CL_SUCCESS) {
    return {};
  }

  std::vector<cl::Device> devices;

  err = plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);
  if (err != CL_SUCCESS) {
    return {};
  }

  return devices[device_idx];
}

std::string ocl_warpper::device_str(size_t platform_idx,
                                    size_t device_idx) noexcept {
  cl_int err;
  cl::Device dev = private_fun_get_device(platform_idx, device_idx, err);

  if (err != CL_SUCCESS) {
    return "Failed to get device info, error code : " + std::to_string(err);
  }

  auto ret = dev.getInfo<CL_DEVICE_VENDOR>(&err);

  if (err != CL_SUCCESS) {
    return "Failed to get device info, error code : " + std::to_string(err);
  }

  return ret;
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

ocl_warpper::ocl_resource::ocl_resource(size_t platform_idx,
                                        size_t device_idx) {
  this->platform = private_fun_get_platform(platform_idx, this->error);
  if (!this->ok()) {
    return;
  }

  this->device = private_fun_get_device(platform_idx, device_idx, this->error);
  if (!this->ok()) {
    return;
  }

  this->init_resource();
}

void ocl_warpper::ocl_resource::init_resource() noexcept {
  this->context = cl::Context(this->device, NULL, NULL, NULL, &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create context.";
    return;
  }

  this->queue =
      cl::CommandQueue(this->context, this->device, NULL, &this->error);

  if (!this->ok()) {
    this->err_msg = "Failed to create command queue.";
    return;
  }

  std::pair<const char *, size_t> src;
  src.first = (const char *)ColorManip_cl_rc;
  src.second = ColorManip_cl_rc_length;
  this->program = cl::Program(this->context, {src}, &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create program with source files.";
    return;
  }

  this->error = this->program.build();
  if (!this->ok()) {
    this->err_msg = "Failed to build program.";
    return;
  }

  this->k_RGB = cl::Kernel(this->program, "match_color_RGB", &this->error);
  if (!this->ok()) {
    this->err_msg = "Failed to create kernel(match_color_RGB).";
    return;
  }

#warning create kernels here.

  this->error = private_fun_change_buf_size(
      this->context, this->task.ARGB_device, 128 * sizeof(uint32_t),
      CL_MEM_READ_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to initialize buffer size for ARGB.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_idx_u16_device, 128 * sizeof(uint16_t),
      CL_MEM_READ_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_diff_f32_device, 128 * sizeof(float),
      CL_MEM_READ_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  // initialize buffers for colorset
  this->error = private_fun_change_buf_size(this->context,
                                            this->colorset.num_colors_ushort, 2,
                                            CL_MEM_READ_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->colorset.colorset_float3, 256 * sizeof(float) * 3,
      CL_MEM_READ_ONLY, true);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }
}

void ocl_warpper::ocl_resource::resize_task(size_t task_num) noexcept {
  // this->buf_unconverted_ARGB_host.resize(task_num);
  this->task.result_idx_u16_host.resize(task_num);
  this->task.result_diff_f32_host.resize(task_num);

  const size_t ARGB_required_bytes = task_num * sizeof(uint32_t);
  const size_t result_idx_required_bytes = task_num * sizeof(uint16_t);
  const size_t result_diff_required_bytes = task_num * sizeof(float);

  this->error =
      private_fun_change_buf_size(this->context, this->task.ARGB_device,
                                  ARGB_required_bytes, CL_MEM_READ_ONLY, false);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_idx_u16_device,
      result_idx_required_bytes, CL_MEM_WRITE_ONLY, false);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = private_fun_change_buf_size(
      this->context, this->task.result_diff_f32_device,
      result_diff_required_bytes, CL_MEM_WRITE_ONLY, false);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }
  return;
}

void ocl_warpper::ocl_resource::resize_colorset(size_t color_num) noexcept {
  this->error = private_fun_change_buf_size(
      this->context, this->colorset.colorset_float3,
      color_num * 3 * sizeof(float), CL_MEM_READ_ONLY, false);
  if (!this->ok())
    return;

  this->error = private_fun_change_buf_size(this->context,
                                            this->colorset.num_colors_ushort, 2,
                                            CL_MEM_READ_ONLY, false);
  if (!this->ok())
    return;
}

void ocl_warpper::ocl_resource::set_colorset(
    size_t color_num, const std::array<const float *, 3> &color_ptrs) noexcept {
  this->resize_colorset(color_num);
  if (!this->ok()) {
    this->err_msg = "Failed to resize colorset.";
    return;
  }

  uint16_t num = color_num;

  Eigen::ArrayXXf trans(color_num, 3);

  for (size_t c = 0; c < 3; c++) {
    memcpy(&trans(0, c), color_ptrs[c], color_num * sizeof(float));
  }

  trans.transposeInPlace();

  this->error = this->queue.enqueueWriteBuffer(
      this->colorset.colorset_float3, false, 0, color_num * 3 * sizeof(float),
      trans.data());
  if (!this->ok()) {
    this->err_msg = "Failed to copy colorset into device.";
    return;
  }

  this->error = this->queue.enqueueWriteBuffer(this->colorset.num_colors_ushort,
                                               false, 0, 2, &num);
  if (!this->ok()) {
    this->err_msg = "Failed to copy colornum into device.";
    return;
  }

  this->error = this->queue.finish();
  if (!this->ok()) {
    this->err_msg = "Failed to wait for queue.";
    return;
  }
}

void ocl_warpper::ocl_resource::set_task(const uint32_t *src, size_t task_num,
                                         ::SCL_convertAlgo algo) noexcept {
  this->resize_task(task_num);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = this->queue.enqueueWriteBuffer(
      this->task.ARGB_device, false, 0, task_num * sizeof(uint32_t), src);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = this->queue.finish();
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  cl::Kernel *k = nullptr;

  switch (algo) {
  case SCL_convertAlgo::RGB:
    k = &this->k_RGB;
    break;
  default:
    break;
  }

  // set kernel args
  this->error = k->setArg(0, this->colorset.colorset_float3);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = k->setArg(1, this->colorset.num_colors_ushort);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = k->setArg(2, this->task.ARGB_device);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = k->setArg(3, this->task.result_idx_u16_device);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
    return;
  }

  this->error = k->setArg(4, this->task.result_diff_f32_device);
  if (!this->ok()) {
    this->err_msg = "Failed to ###.";
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