#ifndef SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H
#define SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H

#include <string>
#include <variant>

namespace ocl_warpper {

size_t platform_num() noexcept;
std::string platform_str(size_t platform_idx) noexcept;

size_t device_num(size_t platform_idx) noexcept;
std::string device_str(size_t platform_idx, size_t device_idx) noexcept;

class ocl_resource_interface {
public:
  ocl_resource_interface() = default;
  virtual ~ocl_resource_interface() = default;

  virtual int error_code() const noexcept = 0;
  virtual bool ok() const noexcept = 0;
};

ocl_resource_interface *create_default() noexcept;
ocl_resource_interface *create_idx(size_t platform_idx,
                                   size_t device_idx) noexcept;
void destroy_ocl_resource(ocl_resource_interface *) noexcept;

} // namespace ocl_warpper

#endif // SLOPECRAFT_UTILITIES_COLORDIRR_OPENCL_H