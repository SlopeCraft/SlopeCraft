#include <cstdio>
#include <string>
#include <SlopeCraftL.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Must provide 1 arguments\n");
    return __LINE__;
  }

  const char* archive_path = argv[1];
  printf("Try parsing %s\n", archive_path);
  SlopeCraft::block_list_interface* blocklist{nullptr};
  {
    std::string errmsg;
    errmsg.resize(8192);
    SlopeCraft::string_deliver err_sd{errmsg.data(), errmsg.size()};

    std::string warnings;
    warnings.resize(8192);
    SlopeCraft::string_deliver warn_sd{warnings.data(), warnings.size()};
    SlopeCraft::block_list_create_info option{
        SC_VERSION_U64,
        &warn_sd,
        &err_sd,
    };
    blocklist = SlopeCraft::SCL_create_block_list(archive_path, option);
    errmsg.resize(err_sd.size);
    warnings.resize(warn_sd.size);

    if (!warnings.empty()) {
      printf("Warnings: %s\n", warnings.data());
    }

    if (!errmsg.empty()) {
      printf("Error: %s\n", errmsg.data());
      return __LINE__;
    }
  }

  printf("%s loaded successfully\n", archive_path);
  SlopeCraft::SCL_destroy_block_list(blocklist);

  return 0;
  // if (archive.getE) return 0;
}
