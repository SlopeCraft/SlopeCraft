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

    if (!rp->add_textures(zf_2, !cover_from_first_to_end)) {
      cout << "Failed to add textures from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }

    if (!rp->add_block_states(zf_2, !cover_from_first_to_end)) {
      cout << "Failed to add block states from resource pack : "
           << zip_file_names[zfidx] << endl;
      continue;
    }

    if (!rp->add_block_models(zf_2, !cover_from_first_to_end)) {
      cout << "Warning : Failed to add block models from resource pack : "
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

VCL_EXPORT_FUN void VCL_display_resource_pack(const VCL_resource_pack *rp) {
  if (rp == nullptr) {
    return;
  }

  cout << "There are " << rp->get_block_states().size()
       << " block state files : \n";

  for (const auto &pair : rp->get_block_states()) {
    cout << pair.first << " : {";
    for (const auto &i : pair.second.LUT) {
      cout << "\n  [";
      for (const auto &j : i.first) {
        cout << j.key << '=' << j.value << ',';
      }

      if (!i.first.empty()) {
        cout << '\b';
      }

      cout << "] => " << i.second.model_name;
      cout << ", x=" << int(i.second.x) * 10;
      cout << ", y=" << int(i.second.y) * 10;
      cout << ", uvlock="
           << (const char *)(i.second.uvlock ? ("true") : ("false"));
    }
    cout << "}\n";
  }

  cout << "There are " << rp->get_models().size() << " models : \n";

  for (const auto &pair : rp->get_models()) {
    cout << pair.first << " : " << pair.second.elements.size() << " elements\n";
  }

  cout << endl;

  cout << "There are " << rp->get_textures().size() << " textures : \n";

  for (const auto &pair : rp->get_textures()) {
    cout << pair.first << " : [" << pair.second.rows() << ", "
         << pair.second.cols() << "]\n";
  }

  cout << endl;
}
VCL_EXPORT_FUN void
VCL_display_block_state_list(const VCL_block_state_list *bsl) {
  if (bsl == nullptr) {
    return;
  }

  cout << "Block state contains " << bsl->block_states().size()
       << " blocks : \n";

  for (const auto &pair : bsl->block_states()) {
    cout << pair.first << " : ";
    cout << "nameZH = \"" << pair.second.name_ZH << "\", nameEN = \""
         << pair.second.name_EN;
    cout << "\", transparent = "
         << (const char *)(pair.second.is_transparent() ? "true" : "false");
    cout << ", supported versions = [";

    for (SCL_gameVersion v = SCL_gameVersion::MC12; v <= max_version;
         v = SCL_gameVersion(int(v) + 1)) {
      if (pair.second.version_info.contains(v)) {
        cout << int(v) << ',';
      }
    }

    cout << "]\n";
  }

  cout << endl;
}