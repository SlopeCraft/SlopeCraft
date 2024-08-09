/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "BlockStateList.h"

#include <fstream>
#include <json.hpp>

#include "ParseResourcePack.h"
#include "VCL_internal.h"
#include <magic_enum.hpp>

VCL_block::VCL_block() { this->initialize_attributes(); }

VCL_block::VCL_block(const std::string *full_id_ptr) : full_id_p(full_id_ptr) {
  this->initialize_attributes();
}

void VCL_block::initialize_attributes() noexcept {
  this->attributes.reset();
  this->set_transparency(false);
  this->set_is_background(false);
  for (size_t sz = idx_face_up; sz <= idx_face_west; sz++) {
    this->attributes[sz] = true;
  }
  this->set_attribute(attribute::is_air, false);
  this->set_attribute(attribute::reproducible, true);
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

#define VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(key_str, key_enum)            \
  if (jo.contains(#key_str)) {                                          \
    if (!jo.at(#key_str).is_boolean()) {                                \
      return std::nullopt;                                              \
    }                                                                   \
                                                                        \
    ret.set_attribute(VCL_block::attribute::key_enum, jo.at(#key_str)); \
  }

std::optional<VCL_block> parse_block(const nlohmann::json &jo) {
  if (!jo.contains("version")) {
    return std::nullopt;
  }

  VCL_block ret;
  bool ok = false;
  ret.version_info = parse_version_set(jo.at("version"), &ok);

  if (not ok) {
    return std::nullopt;
  }

  ret.set_transparency(false);

  if (!jo.contains("class") || !jo.at("class").is_string()) {
    return std::nullopt;
  } else {
    const std::string &str = jo.at("class");
    ret.block_class = string_to_block_class(str, &ok);

    if (not ok) {
      return std::nullopt;
    }
  }

  if (jo.contains("id_replace_list")) {
    if (!jo.at("id_replace_list").is_array()) {
      return std::nullopt;
    }

    const nlohmann::json &ja = jo.at("id_replace_list");

    for (size_t i = 0; i < ja.size(); i++) {
      const nlohmann::json &jaa = ja.at(i);
      if (!jaa.is_array() || jaa.size() != 2) {
        return std::nullopt;
      }

      if (!jaa[0].is_number_integer() || !jaa[1].is_string()) {
        return std::nullopt;
      }

      int val = jaa[0];
      std::string idr = jaa[1];
      ret.id_replace_list.emplace_back(
          std::make_pair<SCL_gameVersion, std::string>(SCL_gameVersion(val),
                                                       std::move(idr)));
    }
  }

  if (jo.contains("nameZH")) {
    if (jo.at("nameZH").is_string()) {
      ret.name_ZH = jo.at("nameZH");
    } else {
      return std::nullopt;
    }
  }

  if (jo.contains("nameEN")) {
    if (jo.at("nameEN").is_string()) {
      ret.name_EN = jo.at("nameEN");
    } else {
      return std::nullopt;
    }
  }

  if (jo.contains("transparent")) {
    if (jo.at("transparent").is_boolean()) {
      ret.set_transparency(jo.at("transparent"));
    } else {
      return std::nullopt;
    }
  }

  if (jo.contains("faces")) {
    if (jo.at("faces").is_array()) {
      ret.disable_all_faces();
      const nlohmann::json &ja = jo.at("faces");

      for (size_t i = 0; i < ja.size(); i++) {
        if (!ja.at(i).is_string()) {
          return std::nullopt;
        }
        bool _ok = true;

        const std::optional<VCL_face_t> f =
            string_to_face_idx(ja.at(i).get<nlohmann::json::string_t>());
        if (not f) {
          return std::nullopt;
        }

        ret.set_face_avaliablity(f.value(), true);
      }
    } else {
      return std::nullopt;
    }
  }

  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(burnable, burnable);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(isGlowing, is_glowing);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(endermanPickable, enderman_pickable);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(background, background);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(is_air, is_air);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(is_grass, is_grass);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(is_foliage, is_foliage);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(reproducible, reproducible);
  VCL_PRIVATE_MACRO_PARSE_ATTRIBUTE(rare, rare);

  return ret;
}

bool VCL_block_state_list::add(std::string_view filename) noexcept {
  using njson = nlohmann::json;

  njson jo;

  try {
    std::ifstream ifs(filename.data());

    jo = njson::parse(ifs, nullptr, true, true);

    ifs.close();
  } catch (std::exception &e) {
    std::string msg =
        fmt::format("Failed to parse {}, detail : {}", filename, e.what());
    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  for (const auto &pair : jo.items()) {
    auto vb = parse_block(pair.value());

    if (not vb) {
      std::string msg = fmt::format(
          "Failed to parse {},  : invalid value for block state {} : {}",
          filename, pair.key().c_str(), to_string(pair.value()));
      ::VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    auto it = this->states.emplace(pair.key(), std::move(vb.value()));

    // This statement requires that VCL_block_state_list is a friend class of
    // VCL_block
    it.first->second.full_id_p = &it.first->first;
  }

  return true;
}

void VCL_block_state_list::available_block_states(
    SCL_gameVersion v, VCL_face_t f,
    std::vector<VCL_block *> *const str_list) noexcept {
  str_list->clear();

  for (auto &pair : this->states) {
    if (pair.second.match(v, f)) {
      str_list->emplace_back(&pair.second);
    }
  }
}

void VCL_block_state_list::avaliable_block_states_by_transparency(
    SCL_gameVersion v, VCL_face_t f,
    std::vector<VCL_block *> *const list_non_transparent,
    std::vector<VCL_block *> *const list_transparent) noexcept {
  list_non_transparent->clear();
  list_transparent->clear();

  for (auto &pair : this->states) {
    if (pair.second.match(v, f)) {
      if (pair.second.is_air()) {
        continue;
      }

      if (pair.second.is_transparent()) {
        list_transparent->emplace_back(&pair.second);
      } else {
        list_non_transparent->emplace_back(&pair.second);
      }
    }
  }
}

void VCL_block_state_list::update_foliages(
    bool is_foliage_transparent) noexcept {
  for (auto &pair : this->states) {
    if (pair.second.get_attribute(VCL_block_attribute_t::is_foliage)) {
      pair.second.set_attribute(VCL_block_attribute_t::transparency,
                                is_foliage_transparent);
    }
  }
}

VCL_block_class_t string_to_block_class(std::string_view str,
                                        bool *ok) noexcept {
  auto ret = magic_enum::enum_cast<VCL_block_class_t>(str);
  if (ok != nullptr) {
    *ok = ret.has_value();
  }
  if (!ret.has_value()) {
    return {};
  }

  return ret.value();
}