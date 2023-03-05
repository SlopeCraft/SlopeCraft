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