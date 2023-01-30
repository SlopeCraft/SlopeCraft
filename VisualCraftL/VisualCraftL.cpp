#include "VisualCraftL.h"

#include <stddef.h>

#include <iostream>

#include "ParseResourcePack.h"
#include "Resource_tree.h"
#include "TokiVC.h"

#include "BlockStateList.h"

using std::cout, std::endl;

VCL_EXPORT_FUN VCL_Kernel *VCL_create_kernel() { return new TokiVC; }
VCL_EXPORT_FUN void VCL_destroy_kernel(VCL_Kernel *const ptr) {
  if (ptr != nullptr) {
    delete dynamic_cast<TokiVC *>(ptr);
  }
}

VCL_EXPORT_FUN VCL_resource_pack *
VCL_create_resource_pack(const int zip_file_count,
                         const char *const *const zip_file_names,
                         bool cover_from_first_to_end) {

  if (zip_file_count <= 0) {
    return nullptr;
  }

  VCL_resource_pack *const rp = new VCL_resource_pack;
  /*
    zipped_folder zf = zipped_folder::from_zip(zip_file_names[0]);
    rp->add_block_models(zf);
    rp->add_block_states(zf);
    rp->add_textures(zf);
  */

  int success_count = 0;
  for (int zfidx = 0; zfidx < zip_file_count; zfidx++) {

    zipped_folder zf_2 = zipped_folder::from_zip(zip_file_names[zfidx]);
    if (!rp->add_block_models(zf_2, !cover_from_first_to_end)) {
      cout << "Warning : Failed to add block models from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }
    if (!rp->add_block_states(zf_2, !cover_from_first_to_end)) {
      cout << "Failed to add block states from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }

    if (!rp->add_textures(zf_2, !cover_from_first_to_end)) {
      cout << "Failed to add textures from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }

    success_count++;
  }

  if (success_count < zip_file_count) {
    cout << "Warning : " << zip_file_count - success_count
         << " resource pack(s) not parsed." << endl;
  }

  if (success_count <= 0) {
    delete rp;
    return nullptr;
  }

  return rp;
}

VCL_EXPORT_FUN void VCL_destroy_resource_pack(VCL_resource_pack *const ptr) {
  if (ptr != nullptr) {
    delete ptr;
  }
}

[[nodiscard]] VCL_EXPORT_FUN VCL_block_state_list *
VCL_create_block_state_list(const int file_count,
                            const char *const *const json_file_names) {
  if (file_count <= 0 || json_file_names == nullptr) {
    return nullptr;
  }

  VCL_block_state_list *const bsl = new VCL_block_state_list;

  for (int i = 0; i < file_count; i++) {
    bsl->add(json_file_names[i]);
  }

  return bsl;
}

VCL_EXPORT_FUN void
VCL_destroy_block_state_list(VCL_block_state_list *const ptr) {
  delete ptr;
}