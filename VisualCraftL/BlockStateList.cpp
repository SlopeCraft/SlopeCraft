#include "BlockStateList.h"

#include <json.hpp>

#include <fstream>

#include <iostream>

version_set parse_version_set(const nlohmann::json &jo,
                              bool *const ok) noexcept {
  if (jo.is_string() && jo == "all") {
    *ok = true;
    return version_set::all();
  }

  if (jo.is_number_unsigned()) {
    version_set ret;

    for (SCL_gameVersion v = SCL_gameVersion(int(jo)); v <= max_version;
         v = SCL_gameVersion(int(v) + 1)) {
      ret[v] = true;
    }

    *ok = true;
    return ret;
  }

  if (jo.is_array() && jo[0].is_number_unsigned()) {
    version_set ret;

    const nlohmann::json::array_t &ja = jo;

    for (const auto &val : ja) {
      if (!val.is_number_unsigned()) {
        break;
      }

      SCL_gameVersion v = SCL_gameVersion(int(val));

      if (v > max_version) {
        break;
      }

      ret[v] = true;
    }

    *ok = true;
    return ret;
  }

  *ok = false;

  return {};
}

bool VCL_block_state_list::add(std::string_view filename) noexcept {

  using njson = nlohmann::json;

  njson jo;

  try {
    std::ifstream ifs(filename.data());

    ifs >> jo;

    ifs.close();
  }

  catch (std::runtime_error re) {
    std::cout << "Failed to parse " << filename << ", detail : " << re.what()
              << std::endl;
    return false;
  }

  bool ok = true;
  for (const auto pair : jo.items()) {
    version_set vs = parse_version_set(pair.value(), &ok);

    if (!ok) {
      std::cout << "Failed to parse " << filename
                << " : invalid value for block state " << pair.key() << " : "
                << pair.value() << std::endl;
      return false;
    }

    this->states.emplace(pair.key(), vs);
  }

  return true;
}

void VCL_block_state_list::available_block_states(
    SCL_gameVersion v,
    std::vector<const std::string *> *const str_list) const noexcept {
  str_list->clear();

  for (const auto &pair : this->states) {
    if (pair.second.contains(v)) {
      str_list->emplace_back(&pair.first);
    }
  }
}