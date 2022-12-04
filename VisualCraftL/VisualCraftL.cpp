#include "VisualCraftL.h"
#include "TokiVC.h"
#include "VisualCraft_classes.h"

#include <stddef.h>

#include "Resource_tree.h"

#include <iostream>

using std::cout, std::endl;

VCL_EXPORT VCL_Kernel *VCL_create_kernel() { return new TokiVC; }
VCL_EXPORT void VCL_destroy_kernel(VCL_Kernel *const ptr) {

  if (ptr != nullptr) {
    delete static_cast<TokiVC *>(ptr);
  }
}
