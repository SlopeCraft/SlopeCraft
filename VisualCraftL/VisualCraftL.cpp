#include "VisualCraftL.h"
#include "TokiVC.h"
#include "VisualCraft_classes.h"

#include <stddef.h>

#include "Resource_tree.h"

#include <iostream>

using std::cout, std::endl;

VCL_EXPORT VCL_Kernel *VCL_create_kernel() { return new TokiVC; }
VCL_EXPORT void VCL_destroy_kernel(VCL_Kernel **ptrptr) {
  if (ptrptr != NULL) {
    delete static_cast<TokiVC *>(*ptrptr);
    *ptrptr = NULL;
  }
}

void display_folder(const zipped_folder &folder, const int offset = 0) {
  std::string spaces;
  spaces.resize(offset);
  for (char &ch : spaces) {
    ch = ' ';
  }
  cout << "{\n";

  for (const auto &file : folder.files) {
    cout << spaces << "  " << file.first << " : " << file.second.file_size()
         << " bytes\n";
  }

  for (const auto &subfolder : folder.subfolders) {

    cout << spaces << "  " << subfolder.first << " : ";
    display_folder(subfolder.second, offset + 2);
  }
  cout << spaces << "}\n";
}

VCL_EXPORT void test_VCL() {

  zipped_folder folder = zipped_folder::from_zip("test.zip");

  display_folder(folder);
  cout << endl;
}
