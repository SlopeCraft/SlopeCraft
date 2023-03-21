#ifndef SLOPECRAFT_UTILITIES_VCLCONFIGLOADER_VCLCONFIGLOADER_H
#define SLOPECRAFT_UTILITIES_VCLCONFIGLOADER_VCLCONFIGLOADER_H

#include "../SC_GlobalEnums.h"
#include <map>
#include <string>
#include <vector>


struct VCL_config {
  std::map<SCL_gameVersion, std::string> default_zips;
  std::vector<std::string> default_jsons;
};

bool load_config(std::string_view filename, VCL_config &cfg) noexcept;

#endif //