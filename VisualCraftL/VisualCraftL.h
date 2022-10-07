#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H

#ifdef VISUALCRAFTL_BUILD
#include <utilities/SC_GlobalEnums.h>
#else
#include "SC_GlobalEnums.h"
#endif

#include "VisualCraftL_global.h"

struct VCL_Kernel;

#ifdef __cplusplus
#include "VisualCraft_classes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

VCL_EXPORT VCL_Kernel *VCL_create_kernel();
VCL_EXPORT void VCL_destroy_kernel(VCL_Kernel **);

VCL_EXPORT void test_VCL();

#ifdef __cplusplus
}
#endif

#endif // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H