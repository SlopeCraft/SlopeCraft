#include "VisualCraftL.h"

#include <stddef.h>

#include <iostream>

#include "ParseResourcePack.h"
#include "Resource_tree.h"
#include "TokiVC.h"
#include "VisualCraftL/VisualCraftL_global.h"
#include "VisualCraft_classes.h"

using std::cout, std::endl;

VCL_EXPORT VCL_Kernel *VCL_create_kernel() { return new TokiVC; }
VCL_EXPORT void VCL_destroy_kernel(VCL_Kernel *const ptr) {
  if (ptr != nullptr) {
    delete static_cast<TokiVC *>(ptr);
  }
}

VCL_EXPORT VCL_resource_pack *VCL_create_resource_pack(
    const int zip_file_count, const char *const *const zip_file_names) {
  return nullptr;
}