#include "VCLConfigLoader.h"
#include <fstream>
#include <iostream>
#include <json.hpp>


using std::cout, std::endl;

bool load_config(std::string_view filename, VCL_config &cfg) noexcept {

  using njson = nlohmann::json;
  njson jo;
  try {
    std::ifstream ofs(filename.data());
    if (!ofs) {
      cout << "Failed to open default config file " << filename << endl;
      return false;
    }

    ofs >> jo;

    for (size_t idx = 0; idx < jo.at("default_resource_pack_zip").size();
         idx++) {
      njson &rp = jo.at("default_resource_pack_zip")[idx];

      SCL_gameVersion ver = SCL_gameVersion((int)rp[0]);
      std::string zipname = rp[1];

      cfg.default_zips[ver] = zipname;
    }
    {
      njson &bsl = jo.at("default_block_state_list");
      cfg.default_jsons.clear();
      for (size_t i = 0; i < bsl.size(); i++) {
        cfg.default_jsons.emplace_back(bsl[i].get<std::string>());
      }
    }

  } catch (std::exception &e) {
    cout << "Failed to parse default config file(\"" << filename
         << "\'), detail : " << e.what() << endl;
    return false;
  }
  return true;
}