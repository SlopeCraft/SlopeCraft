#ifndef SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H
#define SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H

#include <CL/cl.hpp>
#include <array>
#include <assert.h>
#include <string>
#include <utilities/SC_GlobalEnums.h>

namespace ocl_warpper {

size_t platform_num() noexcept;
std::string platform_str(size_t platform_idx) noexcept;

size_t device_num(size_t platform_idx) noexcept;
std::string device_str(size_t platform_idx, size_t device_idx) noexcept;

class ocl_resource {
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

  ocl_resource(size_t platform_idx, size_t device_idx);

  void set_colorset(size_t color_num,
                    const std::array<const float *, 3> &color_ptrs) noexcept;

  void set_task(const std::array<float,3> *src, size_t task_num) noexcept;

  void execute(::SCL_convertAlgo algo) noexcept;

  inline size_t task_count() const noexcept {
    assert(this->task.result_diff_f32_host.size() ==
           this->task.result_idx_u16_host.size());
    return this->task.result_diff_f32_host.size();
  }

  inline std::string device_vendor() const noexcept {
    return this->device.getInfo<CL_DEVICE_VENDOR>();
  }

private:
  void resize_task(size_t task_num) noexcept;
  void resize_colorset(size_t color_num) noexcept;

  cl::Kernel *kernel_by_algo(::SCL_convertAlgo algo) noexcept;

  void set_args(::SCL_convertAlgo algo) noexcept;
};

} // namespace ocl_warpper

#endif // SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H