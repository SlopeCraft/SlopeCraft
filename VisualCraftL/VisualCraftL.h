#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H

#ifdef VISUALCRAFTL_BUILD
#include <utilities/SC_GlobalEnums.h>
#else
#include "SC_GlobalEnums.h"
#endif

#include "VisualCraftL_global.h"

struct VCL_Kernel;
struct VCL_resource_pack;

#ifdef __cplusplus
#include "VisualCraft_classes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

VCL_EXPORT VCL_Kernel *VCL_create_kernel();
VCL_EXPORT void VCL_destroy_kernel(VCL_Kernel *const ptr);

VCL_EXPORT void test_VCL();

VCL_EXPORT VCL_resource_pack *VCL_create_resource_pack(
    const int zip_file_count, const char *const *const zip_file_names);

#ifdef __cplusplus
}
#endif

#endif  // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H