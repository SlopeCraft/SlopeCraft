#include "ColorDiff_OpenCL.h"

#include <CL/cl.hpp>

#include <utilities/SC_GlobalEnums.h>

namespace ocl_warpper {} // namespace ocl_warpper

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

class ocl_resource : public ocl_warpper::ocl_resource_interface {
public:
  cl_int error;
  cl::Platform platform;
  cl::Device device;
  cl::Context context;
  cl::CommandQueue queue;
  cl::Program program;

private:
  void init_resource() noexcept {
    this->context = cl::Context(this->device, NULL, NULL, NULL, &this->error);
    if (this->error != CL_SUCCESS)
      return;

    this->queue =
        cl::CommandQueue(this->context, this->device, NULL, &this->error);

    if (this->error != CL_SUCCESS)
      return;

#warning create program here.
  }

public:
  int error_code() const noexcept override { return this->error; }
  bool ok() const noexcept override { return (this->error == CL_SUCCESS); }

public:
  ocl_resource() {
    this->platform = cl::Platform::getDefault(&error);
    if (this->error != CL_SUCCESS)
      return;

    std::vector<cl::Device> devs;
    this->platform.getDevices(CL_DEVICE_TYPE_ALL, &devs);
    if (this->error != CL_SUCCESS)
      return;

    this->device = devs[0];

    this->init_resource();
  }

  ocl_resource(size_t platform_idx, size_t device_idx) {
    this->platform = private_fun_get_platform(platform_idx, this->error);
    if (this->error != CL_SUCCESS)
      return;

    this->device =
        private_fun_get_device(platform_idx, device_idx, this->error);
    if (this->error != CL_SUCCESS)
      return;

    this->init_resource();
  }
};