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

extern "C" {

[[nodiscard]] VCL_EXPORT VCL_Kernel *VCL_create_kernel();
VCL_EXPORT void VCL_destroy_kernel(VCL_Kernel *const ptr);

VCL_EXPORT void test_VCL();

[[nodiscard]] VCL_EXPORT VCL_resource_pack *
VCL_create_resource_pack(const int zip_file_count,
                         const char *const *const zip_file_names,
                         bool cover_from_first_to_end = true);
VCL_EXPORT void VCL_destroy_resource_pack(VCL_resource_pack *const ptr);
}

#endif // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H