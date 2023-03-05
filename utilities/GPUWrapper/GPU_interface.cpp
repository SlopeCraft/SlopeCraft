#include "GPU_interface.h"

#ifdef SLOPECRAFT_HAVE_OPENCL
#include "OCLWrapper.h"

#endif

void gpu_wrapper::destroy(gpu_interface *gi) {

#ifdef SLOPECRAFT_HAVE_OPENCL
  if (dynamic_cast<ocl_warpper::ocl_resource *>(gi) != nullptr) {
    delete dynamic_cast<ocl_warpper::ocl_resource *>(gi);
    return;
  }
#endif

  delete gi;
}

#ifdef SLOPECRAFT_HAVE_OPENCL
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
#endif // #ifdef SLOPECRAFT_HAVE_OPENCL