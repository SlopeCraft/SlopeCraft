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
    delete dynamic_cast<TokiVC *>(ptr);
  }
}

VCL_EXPORT VCL_resource_pack *
VCL_create_resource_pack(const int zip_file_count,
                         const char *const *const zip_file_names,
                         bool cover_from_first_to_end) {

  if (zip_file_count <= 0) {
    return nullptr;
  }
  zipped_folder zf = zipped_folder::from_zip(zip_file_names[0]);

  VCL_resource_pack *const rp = new VCL_resource_pack;
  rp->add_block_models(zf);
  rp->add_block_states(zf);
  rp->add_textures(zf);

  for (int zfidx = 1; zfidx < zip_file_count; zfidx++) {

    zipped_folder zf_2 = zipped_folder::from_zip(zip_file_names[zfidx]);

    rp->add_block_models(zf_2, !cover_from_first_to_end);
    rp->add_block_states(zf_2, !cover_from_first_to_end);
    rp->add_textures(zf_2, !cover_from_first_to_end);
  }

  return rp;
}

VCL_EXPORT void VCL_destroy_resource_pack(VCL_resource_pack *const ptr) {
  if (ptr != nullptr) {
    delete ptr;
  }
}