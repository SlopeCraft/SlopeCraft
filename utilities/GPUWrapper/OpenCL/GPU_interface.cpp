#include "../GPU_interface.h"
#include "OCLWrapper.h"

void gpu_wrapper::destroy(gpu_interface *gi) {

  delete static_cast<ocl_warpper::ocl_resource *>(gi);
  return;
}

size_t gpu_wrapper::platform_num() noexcept {
  return ::ocl_warpper::platform_num();
}
std::string gpu_wrapper::platform_str(size_t platform_idx) noexcept {
  return ::ocl_warpper::platform_str(platform_idx);
}

size_t gpu_wrapper::device_num(size_t platform_idx) noexcept {
  return ::ocl_warpper::device_num(platform_idx);
}
std::string gpu_wrapper::device_str(size_t platform_idx,
                                    size_t device_idx) noexcept {
  return ::ocl_warpper::device_str(platform_idx, device_idx);
}

namespace gpu_wrapper {
gpu_interface *create_opencl(size_t platform_idx, size_t device_idx) {
  return static_cast<gpu_interface *>(
      new ocl_warpper::ocl_resource(platform_idx, device_idx));
}
} // namespace gpu_wrapper

ocl_warpper::ocl_platform::ocl_platform(size_t idx) {
  this->platform = private_fun_get_platform(idx, this->err);
  if (this->err != CL_SUCCESS)
    return;

  this->name = this->platform.getInfo<CL_PLATFORM_NAME>(&this->err);
  if (this->err != CL_SUCCESS)
    return;

  this->err = this->platform.getDevices(CL_DEVICE_TYPE_ALL, &this->devices);
}

::gpu_wrapper::platform_wrapper * ::gpu_wrapper::platform_wrapper::create(
    size_t idx, int *errorcode) noexcept {
  ::ocl_warpper::ocl_platform *plat = new ::ocl_warpper::ocl_platform(idx);

  if (errorcode != nullptr) {
    *errorcode = plat->err;
  }

  if (plat->err != CL_SUCCESS) {
    delete plat;
    return nullptr;
  }
  return static_cast<::gpu_wrapper::platform_wrapper *>(plat);
}

void ::gpu_wrapper::platform_wrapper::destroy(platform_wrapper *pw) noexcept {
  delete static_cast<::ocl_warpper::ocl_platform *>(pw);
}

ocl_warpper::ocl_device::ocl_device(cl::Device __dev) : device(__dev) {

  this->name = __dev.getInfo<CL_DEVICE_NAME>(&this->err);
}

::gpu_wrapper::device_wrapper * ::gpu_wrapper::device_wrapper::create(
    platform_wrapper *pw, size_t idx, int *errorcode) noexcept {
  ::ocl_warpper::ocl_device *dev = new ::ocl_warpper::ocl_device(
      static_cast<::ocl_warpper::ocl_platform *>(pw)->devices[idx]);

  if (errorcode != nullptr) {
    *errorcode = dev->err;
  }

  if (dev->err != CL_SUCCESS) {
    delete dev;
    return nullptr;
  }
  return static_cast<::gpu_wrapper::device_wrapper *>(dev);
}

void ::gpu_wrapper::device_wrapper::destroy(device_wrapper *dw) noexcept {
  delete static_cast<::ocl_warpper::ocl_device *>(dw);
}