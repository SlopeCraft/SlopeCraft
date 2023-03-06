#include "../GPU_interface.h"
#include "OCLWrapper.h"

size_t gpu_wrapper::platform_num() noexcept {
  return ::ocl_warpper::platform_num();
}

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

gpu_wrapper::gpu_interface *
gpu_wrapper::gpu_interface::create(gpu_wrapper::platform_wrapper *pw,
                                   gpu_wrapper::device_wrapper *dw) noexcept {
  ocl_warpper::ocl_platform *plat =
      static_cast<ocl_warpper::ocl_platform *>(pw);
  ocl_warpper::ocl_device *dev = static_cast<ocl_warpper::ocl_device *>(dw);

  ocl_warpper::ocl_resource *ret =
      new ocl_warpper::ocl_resource(plat->platform, dev->device);

  if (!ret->ok()) {
    delete ret;
    return nullptr;
  }

  return static_cast<::gpu_wrapper::gpu_interface *>(ret);
}

void ::gpu_wrapper::gpu_interface::destroy(gpu_interface *gi) noexcept {

  delete static_cast<ocl_warpper::ocl_resource *>(gi);
  return;
}
