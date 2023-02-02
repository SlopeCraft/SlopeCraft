#include "BlockStateList.h"

#include <json.hpp>

#include <fstream>

#include <iostream>

#include "ParseResourcePack.h"

using std::cout, std::endl;

VCL_block::VCL_block() { this->initialize_attributes(); }

VCL_block::VCL_block(const std::string *full_id_ptr) : full_id_p(full_id_ptr) {
  this->initialize_attributes();
}

void VCL_block::initialize_attributes() noexcept {
  this->attributes.reset();
  this->set_transparency(false);
  this->set_is_background(true);
  for (size_t sz = idx_face_up; sz < idx_face_west; sz++) {
    this->attributes[sz] = true;
  }
}

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

  if (jo.contains("faces")) {
    if (jo.at("faces").is_array()) {
      ret.disable_all_faces();
      const nlohmann::json &ja = jo.at("faces");

      for (size_t i = 0; i < ja.size(); i++) {
        if (!ja.at(i).is_string()) {
          *ok = false;
          return {};
        }
        bool _ok = true;

        const VCL_face_t f =
            string_to_face_idx(ja.at(i).get<nlohmann::json::string_t>(), &_ok);
        if (!_ok) {
          *ok = false;
          return {};
        }

        ret.set_face_avaliablity(f, true);
      }
    } else {

      *ok = false;
      return {};
    }
  }

  if (jo.contains("burnable")) {
    if (!jo.at("burnable").is_boolean()) {
      *ok = false;
      return {};
    }

    ret.set_attribute(VCL_block::attribute::burnable, jo.at("burnable"));
  }

  if (jo.contains("isGlowing")) {
    if (!jo.at("isGlowing").is_boolean()) {
      *ok = false;
      return {};
    }

    ret.set_attribute(VCL_block::attribute::is_glowing, jo.at("isGlowing"));
  }

  if (jo.contains("endermanPickable")) {
    if (!jo.at("endermanPickable").is_boolean()) {
      *ok = false;
      return {};
    }

    ret.set_attribute(VCL_block::attribute::enderman_pickable,
                      jo.at("endermanPickable"));
  }

  if (jo.contains("background")) {
    if (!jo.at("background").is_boolean()) {
      *ok = false;
      return {};
    }

    ret.set_attribute(VCL_block::attribute::background, jo.at("background"));
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

    auto it = this->states.emplace(pair.key(), std::move(vb));

    // This statement requires that VCL_block_state_list is a friend class of
    // VCL_block
    it.first->second.full_id_p = &it.first->first;
  }

  return true;
}

void VCL_block_state_list::available_block_states(
    SCL_gameVersion v, std::vector<VCL_block *> *const str_list) noexcept {
  str_list->clear();

  for (auto &pair : this->states) {
    if (pair.second.version_info.contains(v)) {
      str_list->emplace_back(&pair.second);
    }
  }
}

void VCL_block_state_list::avaliable_block_states_by_transparency(
    SCL_gameVersion v, std::vector<VCL_block *> *const list_non_transparent,
    std::vector<VCL_block *> *const list_transparent) noexcept {
  list_non_transparent->clear();
  list_transparent->clear();

  for (auto &pair : this->states) {
    if (pair.second.version_info.contains(v)) {
      if (pair.second.is_transparent()) {
        list_transparent->emplace_back(&pair.second);
      } else {
        list_non_transparent->emplace_back(&pair.second);
      }
    }
  }
}