#include "BlockStateList.h"

#include <json.hpp>

#include <fstream>

#include <iostream>

using std::cout, std::endl;

VCL_block::VCL_block() { this->set_transparency(false); }

version_set parse_version_set(const nlohmann::json &jo,
                              bool *const ok) noexcept {

  if (jo.is_string() && jo == "all") {
    *ok = true;
    // ret.version_info = version_set::all();
    return version_set::all();
  }

  if (jo.is_number_unsigned()) {
    version_set ret;

    for (SCL_gameVersion v = SCL_gameVersion(int(jo)); v <= max_version;
         v = SCL_gameVersion(int(v) + 1)) {
      ret[v] = true;
    }

    // ret.set_transparency(is_transparent);

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
    // ret.set_transparency(is_transparent);

    *ok = true;
    return ret;
  }

  *ok = false;

  return {};
}

VCL_block parse_block(const nlohmann::json &jo, bool *const ok) {
  if (!jo.contains("version")) {
    *ok = false;
    return {};
  }

  VCL_block ret;
  ret.version_info = parse_version_set(jo.at("version"), ok);

  if (!ok) {
    *ok = false;
    return {};
  }

  ret.set_transparency(false);

  if (jo.contains("nameZH")) {
    if (jo.at("nameZH").is_string()) {
      ret.name_ZH = jo.at("nameZH");
    } else {
      *ok = false;
      return {};
    }
  }

  if (jo.contains("nameEN")) {
    if (jo.at("nameEN").is_string()) {
      ret.name_EN = jo.at("nameEN");
    } else {
      *ok = false;
      return {};
    }
  }

  if (jo.contains("transparent")) {

    if (jo.at("transparent").is_boolean()) {
      ret.set_transparency(jo.at("transparent"));
    } else {
      *ok = false;
      return {};
    }
  }

  *ok = true;

  return ret;
}

bool VCL_block_state_list::add(std::string_view filename) noexcept {

  using njson = nlohmann::json;

  njson jo;

  try {
    std::ifstream ifs(filename.data());

    jo = njson::parse(ifs, nullptr, true, true);

    ifs.close();
  } catch (std::runtime_error e) {
    cout << "Failed to parse " << filename << ", detail : " << e.what() << endl;
    return false;
  }

  bool ok = true;

  for (const auto pair : jo.items()) {
    VCL_block vb = parse_block(pair.value(), &ok);

    if (!ok) {
      cout << "Failed to parse " << filename
           << " : invalid value for block state " << pair.key() << " : "
           << pair.value() << endl;
      return false;
    }

    this->states.emplace(pair.key(), vb);
  }

  return true;
}

void VCL_block_state_list::available_block_states(
    SCL_gameVersion v,
    std::vector<const std::string *> *const str_list) const noexcept {
  str_list->clear();

  for (const auto &pair : this->states) {
    if (pair.second.version_info.contains(v)) {
      str_list->emplace_back(&pair.first);
    }
  }
}

void VCL_block_state_list::avaliable_block_states_by_transparency(
    SCL_gameVersion v,
    std::vector<const std::string *> *const list_non_transparent,
    std::vector<const std::string *> *const list_transparent) const noexcept {
  list_non_transparent->clear();
  list_transparent->clear();

  for (const auto &pair : this->states) {
    if (pair.second.version_info.contains(v)) {
      if (pair.second.is_transparent()) {
        list_transparent->emplace_back(&pair.first);
      } else {
        list_non_transparent->emplace_back(&pair.first);
      }
    }
  }
}