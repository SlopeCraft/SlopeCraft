#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H

#ifdef VISUALCRAFTL_BUILD
#include <utilities/SC_GlobalEnums.h>
#else
#include "SC_GlobalEnums.h"
#endif

#include "VisualCraftL_global.h"

class VCL_Kernel;
class VCL_resource_pack;
class VCL_block_state_list;

class VCL_Kernel {
public:
  VCL_Kernel() = default;
  virtual ~VCL_Kernel() = default;

  virtual VCL_Kernel_step step() const noexcept = 0;

  virtual bool
  set_resource_pack(const VCL_resource_pack *const rp) noexcept = 0;
  virtual const VCL_resource_pack *resource_pack() const noexcept = 0;
};

extern "C" {

[[nodiscard]] VCL_EXPORT_FUN VCL_Kernel *VCL_create_kernel();
VCL_EXPORT_FUN void VCL_destroy_kernel(VCL_Kernel *const ptr);

VCL_EXPORT_FUN void test_VCL();

[[nodiscard]] VCL_EXPORT_FUN VCL_resource_pack *
VCL_create_resource_pack(const int zip_file_count,
                         const char *const *const zip_file_names,
                         bool cover_from_first_to_end = true);
VCL_EXPORT_FUN void VCL_destroy_resource_pack(VCL_resource_pack *const ptr);

[[nodiscard]] VCL_EXPORT_FUN VCL_block_state_list *
VCL_create_block_state_list(const int file_count,
                            const char *const *const json_file_names);

VCL_EXPORT_FUN void
VCL_destroy_block_state_list(VCL_block_state_list *const ptr);
}

#endif // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H