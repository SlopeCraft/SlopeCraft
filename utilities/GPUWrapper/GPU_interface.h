#ifndef SLOPECRAFT_UTILITIES_GPUWRAPPER_GPUINTERFACE_H
#define SLOPECRAFT_UTILITIES_GPUWRAPPER_GPUINTERFACE_H

#include <array>
#include <string>
#include <utilities/SC_GlobalEnums.h>

namespace gpu_wrapper {
class gpu_interface {
public:
  gpu_interface() = default;
  virtual ~gpu_interface() = default;

  virtual const char *api_v() const noexcept = 0;

  virtual int error_code_v() const noexcept = 0;
  virtual bool ok_v() const noexcept = 0;
  virtual std::string error_detail_v() const noexcept = 0;

  virtual void
  set_colorset_v(size_t color_num,
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

void destroy(gpu_interface *);

class platform_wrapper {
public:
  platform_wrapper() = default;
  virtual ~platform_wrapper() = default;

  virtual const char *name_v() const noexcept = 0;

  virtual size_t num_devices_v() const noexcept = 0;

  // virtual size_t index() const noexcept = 0;

  [[nodiscard]] static platform_wrapper *
  create(size_t idx, int *errorcode = nullptr) noexcept;
  static void destroy(platform_wrapper *) noexcept;
};

class device_wrapper {
public:
  device_wrapper() = default;
  virtual ~device_wrapper() = default;

  virtual const char *name_v() const noexcept = 0;

  [[nodiscard]] static device_wrapper *
  create(platform_wrapper *pw, size_t idx, int *errorcode = nullptr) noexcept;
  static void destroy(device_wrapper *) noexcept;
};

size_t platform_num() noexcept;
std::string platform_str(size_t platform_idx) noexcept;

size_t device_num(size_t platform_idx) noexcept;
std::string device_str(size_t platform_idx, size_t device_idx) noexcept;

#ifdef SLOPECRAFT_HAVE_OPENCL
[[nodiscard]] gpu_wrapper::gpu_interface *create_opencl(size_t platform_idx,
                                                        size_t device_idx);
#endif // #ifdef SLOPECRAFT_HAVE_OPENCL

} // namespace gpu_wrapper

#endif // SLOPECRAFT_UTILITIES_GPUWRAPPER_GPUINTERFACE_H