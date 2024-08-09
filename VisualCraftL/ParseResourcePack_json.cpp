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

#include <json.hpp>
#include <ranges>
#include <string>
#include <unordered_map>

#include <process_block_id.h>
#include "ParseResourcePack.h"
#include "VCL_internal.h"

using namespace resource_json;

using njson = nlohmann::json;

size_t resource_json::state_list::num_1() const noexcept {
  size_t counter = 0;
  for (const auto &s : *this) {
    if (s.value.empty()) {
      counter++;
    }
  }
  return counter;
}

bool resource_json::state_list::euqals(
    const state_list &another) const noexcept {
  if (this->size() <= 0) return true;
  if (this->size() != another.size()) return false;
  int match_num = 0;
  for (const state &sa : *this) {
    for (const state &sb : another) {
      if ((sa.key == sb.key) && (sa.value == sb.value)) {
        match_num++;
      }
    }
  }

  if (match_num < int(this->size())) {
    return false;
  } else {
    return true;
  }
}

bool resource_json::state_list::contains(
    const state_list &another) const noexcept {
  if (another.size() > this->size()) {
    return false;
  }

  for (const state &s_json : another) {
    bool is_current_state_matched = false;
    for (const state &s_block : *this) {
      if (s_json.key != s_block.key) {
        continue;
      }

      if (s_block.value == s_json.value) {
        is_current_state_matched = true;
        break;
      }
    }
    if (!is_current_state_matched) {
      return false;
    }
  }

  return true;
}

bool resource_json::criteria_list_and::match(
    const state_list &sl) const noexcept {
  const auto &cl = *this;
  int match_num = 0;
  for (const criteria &c : cl) {
    std::string_view key = c.key;

    const char *value = nullptr;
    for (const state &s : sl) {
      if (s.key == key) {
        value = s.value.data();
        break;
      }
    }
    // if value is not set, it is not considered as match
    if (value == nullptr) {
      break;
    }

    if (c.match(value)) {
      match_num++;
    }
  }

  if (match_num < int(cl.size())) {
    return false;
  }
  return true;
}

bool resource_json::match_criteria_list(const criteria_list_and &cl,
                                        const state_list &sl) noexcept {
  return cl.match(sl);
}

bool resource_json::multipart_pair::match(const state_list &sl) const noexcept {
  const resource_json::criteria *when =
      std::get_if<resource_json::criteria>(&this->criteria_variant);
  if (when != nullptr) {
    std::string_view key = when->key;
    const char *slvalue = nullptr;
    for (const state &s : sl) {
      if (s.key == key) {
        slvalue = s.value.data();
        break;
      }
    }
    // if sl don't have a value for the key of criteria, it is considered as
    // mismatch
    if (slvalue == nullptr) {
      return false;
    }

    return when->match(slvalue);
  }

  if (std::get_if<criteria_all_pass>(&this->criteria_variant) != nullptr) {
    return true;
  }

  const auto &when_or = std::get<criteria_list_or_and>(this->criteria_variant);

  size_t counter = 0;
  for (const criteria_list_and &cl : when_or.components) {
    if (cl.match(sl)) {
      counter++;
    }
  }

  if (when_or.is_or) {
    return counter > 0;
  } else {
    return counter >= when_or.components.size();
  }
}

model_pass_t block_states_variant::block_model_name(
    const state_list &sl_blk) const noexcept {
  model_pass_t res;
  res.model_name = nullptr;
  for (const auto &pair : this->LUT) {
    if (sl_blk.contains(pair.first)) {
      res = model_pass_t(pair.second);
      return res;
    }
  }

  return res;
}

void block_states_variant::sort() noexcept {
  std::sort(LUT.begin(), LUT.end(),
            [](const std::pair<state_list, model_store_t> &a,
               const std::pair<state_list, model_store_t> &b) -> bool {
              const size_t a_1 = a.first.num_1();
              const size_t b_1 = b.first.num_1();
              if (a_1 != b_1) {
                return a_1 < b_1;
              }
              return a.first.size() > b.first.size();
            });
}

std::vector<model_pass_t> block_state_multipart::block_model_names(
    const state_list &sl) const noexcept {
  std::vector<model_pass_t> res;

  for (const multipart_pair &pair : this->pairs) {
    if (pair.match(sl)) {
      for (const auto &ms : pair.apply_blockmodel) {
        res.emplace_back(model_pass_t(ms));
      }
    }
    // res.emplace_back(model_pass_t(pair.apply_blockmodel));
  }

  return res;
}

struct parse_bs_buffer {
  std::vector<std::pair<blkid::char_range, blkid::char_range>> attributes;
};

bool parse_block_state_variant(const njson::object_t &obj,
                               block_states_variant *const dest_variant);

bool parse_block_state_multipart(const njson::object_t &obj,
                                 block_state_multipart *const dest_variant);

bool resource_json::parse_block_state(
    const char *const json_str_beg, const char *const json_str_end,
    std::variant<block_states_variant, block_state_multipart> *dest,
    bool *const is_dest_variant) noexcept {
  njson::object_t obj;
  try {
    obj = njson::parse(json_str_beg, json_str_end);
  } catch (...) {
    std::string msg = "nlohmann json failed to parse json string : ";
    msg.append(json_str_beg, json_str_end);
    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  const bool has_variant =
      obj.contains("variants") && obj.at("variants").is_object();
  const bool has_multipart =
      obj.contains("multipart") && obj.at("multipart").is_array();

  if (has_variant == has_multipart) {
    std::string msg = fmt::format(
        "Function parse_block_state failed to parse json : "
        "has_variant = {}, has_multipart = {}.",
        has_variant, has_multipart);
    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  if (has_variant) {
    if (is_dest_variant != nullptr) *is_dest_variant = true;

    block_states_variant variant;
    const bool ok = parse_block_state_variant(obj, &variant);
    *dest = std::move(variant);
    return ok;
  }

  if (has_multipart) {
    // parsing multipart is not supported yet.
    if (is_dest_variant != nullptr) *is_dest_variant = false;

    block_state_multipart multipart;
    const bool ok = parse_block_state_multipart(obj, &multipart);
    *dest = std::move(multipart);
    return ok;
    // return parse_block_state_multipart(obj, dest_multipart);
  }
  // unreachable
  return false;
}

bool parse_block_state_list(std::string_view str, state_list *const sl,
                            parse_bs_buffer &buffer) noexcept {
  sl->clear();
  if (str.size() <= 1) return true;

  if (str == "normal") {
    return true;
  }

  if (str == "all") {
    return true;
  }

  if (str == "map") {
    return true;
  }

  if (!blkid::process_state_list({str.data(), str.data() + str.size()},
                                 &buffer.attributes, nullptr)) {
    std::string msg = fmt::format(
        " Function parse_block_state_list failed to parse block state "
        "list : {}",
        str);
    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  sl->reserve(buffer.attributes.size());

  for (const auto &pair : buffer.attributes) {
    state strpair;
    strpair.key.assign(pair.first.begin(), pair.first.end());
    strpair.value.assign(pair.second.begin(), pair.second.end());

    sl->emplace_back(strpair);
  }

  return true;
}

bool parse_block_state_list(std::string_view str,
                            state_list *const sl) noexcept {
  parse_bs_buffer buffer;

  return parse_block_state_list(str, sl, buffer);
}

model_store_t json_to_model(const njson &obj) noexcept {
  model_store_t res;

  res.model_name = obj.at("model");

  if (obj.contains("x") && obj.at("x").is_number()) {
    const int val = obj.at("x");

    if (!block_model::is_0_90_180_270(val)) {
      std::string msg;
      msg = fmt::format(
          "Invalid x rotation value : {}. Invalid values : 0, 90, 180, 270.",
          val);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return {};
    }

    res.x = block_model::int_to_face_rot(val);
  }

  if (obj.contains("y") && obj.at("y").is_number()) {
    const int val = obj.at("y");
    if (!block_model::is_0_90_180_270(val)) {
      std::string msg;
      msg = fmt::format(
          "Invalid y rotation value : {}. Invalid values : 0, 90, 180, 270.",
          val);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return {};
    }
    res.y = block_model::int_to_face_rot(val);
  }

  if (obj.contains("uvlock") && obj.at("uvlock").is_boolean()) {
    res.uvlock = obj.at("uvlock");
  }

  return res;
}

bool parse_block_state_variant(const njson::object_t &obj,
                               block_states_variant *const dest) {
  const njson &variants = obj.at("variants");

  dest->LUT.clear();
  dest->LUT.reserve(variants.size());

  for (auto pair : variants.items()) {
    if (!pair.value().is_structured()) {
      std::string msg = fmt::format(
          "Function parse_block_state_variant failed to parse json : "
          "value for key \"{}\" is not an object or array.",
          pair.key());

      ::VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    if (pair.value().is_array() && pair.value().size() <= 0) {
      std::string msg = fmt::format(
          "Function parse_block_state_variant failed to parse json : "
          "value for key \"{}\" is an empty array.",
          pair.key().data());
      ::VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    const njson &obj =
        (pair.value().is_object()) ? (pair.value()) : (pair.value().at(0));

    if ((!obj.contains("model")) || (!obj.at("model").is_string())) {
      std::string msg = fmt::format(
          "Function parse_block_state_variant failed to parse json : no "
          "valid value for key \"model\"");

      ::VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    std::pair<state_list, model_store_t> p;

    parse_bs_buffer buffer;

    if (!parse_block_state_list(pair.key(), &p.first, buffer)) {
      std::string msg =
          fmt::format("Failed to parse block state list : {}", pair.key());
      ::VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    p.second = json_to_model(obj);

    dest->LUT.emplace_back(p);
  }

  dest->sort();

  return true;
}

void parse_single_criteria_split(std::string_view key, std::string_view values,
                                 criteria *const cr) noexcept {
  cr->key = key;
  cr->values.clear();

  size_t current_value_beg_idx = 0;

  for (size_t idx = 0;; idx++) {
    if (values.size() <= idx || values[idx] == '\0' || values[idx] == '|') {
      cr->values.emplace_back(
          values.substr(current_value_beg_idx, idx - current_value_beg_idx));
      current_value_beg_idx = idx + 1;
    }

    if (values.size() <= idx || values[idx] == '\0') {
      break;
    }
  }
}

model_store_t parse_single_apply(const njson &single_obj) noexcept(false) {
  model_store_t ms;

  ms.model_name = single_obj.at("model");
  if (single_obj.contains("x")) {
    ms.x = block_model::int_to_face_rot(single_obj.at("x"));
  }
  if (single_obj.contains("y")) {
    ms.y = block_model::int_to_face_rot(single_obj.at("y"));
  }
  if (single_obj.contains("uvlock")) {
    ms.uvlock = single_obj.at("uvlock");
  }

  return ms;
}

std::vector<model_store_t> parse_multipart_apply(const njson &apply) noexcept(
    false) {
  std::vector<model_store_t> ret;

  if (apply.is_object()) {
    ret.emplace_back(parse_single_apply(apply));
    return ret;
  }

  if (apply.is_array()) {
    for (size_t i = 0; i < apply.size(); i++) {
      ret.emplace_back(parse_single_apply(apply.at(i)));
    }
    return ret;
  }
  throw std::runtime_error("Invalid value for \"apply\" in a multipart.");
}

std::variant<criteria, criteria_list_or_and, criteria_all_pass>
parse_multipart_when(const njson &when) noexcept(false) {
  const bool is_or = when.contains("OR");
  const bool is_and = when.contains("AND");
  if (is_or || is_and) {
    const njson &list_or_and = (is_or) ? (when.at("OR")) : (when.at("AND"));
    criteria_list_or_and when_or_and;

    when_or_and.components.reserve(list_or_and.size());
    when_or_and.is_or = is_or;

    for (size_t idx = 0; idx < list_or_and.size(); idx++) {
      criteria_list_and and_list;

      for (auto it = list_or_and[idx].begin(); it != list_or_and[idx].end();
           ++it) {
        criteria cr;
        if (it.value().is_boolean()) {
          cr.key = it.key();
          cr.values.emplace_back((it.value()) ? ("true") : ("false"));

        } else {
          parse_single_criteria_split(it.key(), it.value().get<std::string>(),
                                      &cr);
        }
        // const std::string &v_str = ;
        and_list.emplace_back(std::move(cr));
      }

      when_or_and.components.emplace_back(std::move(and_list));
    }

    return when_or_and;
  }

  if (when.size() == 1) {
    criteria cr;

    auto it = when.begin();

    if (it.value().is_boolean()) {
      cr.key = it.key();
      cr.values.emplace_back((it.value()) ? ("true") : ("false"));
    } else {
      parse_single_criteria_split(it.key(), it.value().get<std::string>(), &cr);
    }

    return cr;
  }
  criteria_list_and and_list;

  for (auto it = when.begin(); it != when.end(); ++it) {
    criteria cr;

    if (it.value().is_boolean()) {
      cr.key = it.key();
      cr.values.emplace_back((it.value()) ? ("true") : ("false"));
    } else {
      parse_single_criteria_split(it.key(), it.value().get<std::string>(), &cr);
    }
    and_list.emplace_back(std::move(cr));
  }

  criteria_list_or_and when_or;
  when_or.components.emplace_back(std::move(and_list));

  return when_or;
}

bool parse_block_state_multipart(const njson::object_t &obj,
                                 block_state_multipart *const dest) {
  const njson &multiparts = obj.at("multipart");

  if (!multiparts.is_array()) {
    std::string msg = fmt::format("Fatal error : multipart must be an array.");

    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  dest->pairs.clear();

  for (size_t i = 0; i < multiparts.size(); i++) {
    const njson &part = multiparts[i];

    multipart_pair mpp;

    // parse apply
    try {
      const njson &apply = part.at("apply");
      mpp.apply_blockmodel = parse_multipart_apply(apply);
    } catch (const std::exception &err) {
      std::string msg = fmt::format(
          "An error occurred when parsing the value of apply. Details : {}",
          err.what());
      ::VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    // parse when
    if (!part.contains("when")) {
      mpp.criteria_variant = criteria_all_pass();
      dest->pairs.emplace_back(std::move(mpp));
      continue;
    }

    try {
      const njson &when = part.at("when");

      mpp.criteria_variant = parse_multipart_when(when);

    } catch (const std::exception &err) {
      std::string msg = fmt::format(
          "\nFatal error : failed to parse \"when\" for a multipart blockstate "
          "file. Details : {}\n",
          err.what());
      return false;
    }

    dest->pairs.emplace_back(std::move(mpp));
    /*
        if (!part.is_object()) {
          printf("\nFatal error : multipart must an array of objects.\n");
          return false;
        }

        if (!part.contains("apply") || !part.contains("when")) {
          printf("\nFatal error : element in multipart must contains \"apply\"
       and "
                 "\"when\"\n");
          return false;
        }

        if (!apply.contains("model") || !apply.at("model").is_string()) {
          printf("\nFatal error : multipart should apply a model.\n");
          return false;
        }
        */
  }

  return true;
}

struct face_json_temp {
  std::string texture{""};
  std::array<float, 4> uv{0, 0, 16, 16};
  block_model::face_idx cullface_face;
  bool have_cullface{false};
  bool is_hidden{true};  ///< note that by default, is_hidden is true.
};

struct element_json_temp {
  std::array<float, 3> from;
  std::array<float, 3> to;
  std::array<face_json_temp, 6> faces;
};

struct block_model_json_temp {
  std::string parent{""};
  std::map<std::string, std::string> textures;
  std::vector<element_json_temp> elements;
  bool is_inherited{false};
};

std::optional<block_model::face_idx> string_to_face_idx(
    std::string_view str) noexcept {
  if (str == "up") {
    return block_model::face_idx::face_up;
  }
  if (str == "down") {
    return block_model::face_idx::face_down;
  }
  if (str == "bottom") {
    return block_model::face_idx::face_down;
  }
  if (str == "north") {
    return block_model::face_idx::face_north;
  }
  if (str == "south") {
    return block_model::face_idx::face_south;
  }
  if (str == "east") {
    return block_model::face_idx::face_east;
  }
  if (str == "west") {
    return block_model::face_idx::face_west;
  }

  return std::nullopt;
}

const char *face_idx_to_string(block_model::face_idx f) noexcept {
  switch (f) {
    case block_model::face_idx::face_up:
      return "up";
    case block_model::face_idx::face_down:
      return "down";
    case block_model::face_idx::face_north:
      return "north";
    case block_model::face_idx::face_south:
      return "south";
    case block_model::face_idx::face_east:
      return "east";
    case block_model::face_idx::face_west:
      return "west";
  }

  return nullptr;
}

bool parse_single_model_json(const char *const json_beg,
                             const char *const json_end,
                             block_model_json_temp *const dest) {
  dest->textures.clear();
  dest->elements.clear();
  // disable exceptions, and ignore comments.
  njson obj = njson::parse(json_beg, json_end, nullptr, false, true);
  if (obj.is_null()) {
    // this may be unsafe but just keep it currently.
    std::string msg = "Failed to parse block model json : ";
    msg.append(json_beg, json_end);
    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  if (obj.contains("parent") && obj.at("parent").is_string()) {
    std::string p_str = obj.at("parent");
    if (p_str.starts_with("minecraft:")) {
      dest->parent = p_str.substr(sizeof("minecraft:") / sizeof(char) - 1);
    } else {
      dest->parent = p_str;
    }
  }

  if (obj.contains("textures") && obj.at("textures").is_object()) {
    const njson &textures = obj.at("textures");
    // dest->textures.reserve(textures.size());
    for (auto temp : textures.items()) {
      if (!temp.value().is_string()) {
        continue;
      }

      auto it = dest->textures.emplace(temp.key(), temp.value());

      if (it.first->second.starts_with("block/") ||
          it.first->second.starts_with("blocks/")) {
        it.first->second = "minecraft:" + it.first->second;
      }
    }
  }
  // finished textures

  if (obj.contains("elements") && obj.at("elements").is_array()) {
    const njson::array_t &elearr = obj.at("elements");

    dest->elements.reserve(obj.size());
    for (const auto &e : elearr) {
      if (!e.is_object()) {
        return false;
      }

      element_json_temp ele;
      if (!e.contains("from") || !e.at("from").is_array()) {
        ::VCL_report(VCL_report_type_t::error,
                     "\"from\" doesn't exist, or is not an array.");
        return false;
      }
      // from
      {
        const njson::array_t &arr_from = e.at("from");
        if (arr_from.size() != 3 || !arr_from.front().is_number()) {
          ::VCL_report(VCL_report_type_t::error, "size of \"from\" is not 3");
          return false;
        }

        for (int idx = 0; idx < 3; idx++) {
          if (!arr_from[idx].is_number()) {
            ::VCL_report(
                VCL_report_type_t::error,
                "one or more element in array \"from\" is not number.");
            return false;
          }
          ele.from[idx] = arr_from[idx];
        }
      }
      if (!e.contains("to") || !e.at("to").is_array()) {
        ::VCL_report(VCL_report_type_t::error,
                     "\"to\" doesn't exist, or is not an array.");
        return false;
      }
      // to
      {
        const njson::array_t &arr_to = e.at("to");
        if (arr_to.size() != 3) {
          ::VCL_report(VCL_report_type_t::error, "size of \"to\" is not 3.");
          return false;
        }

        for (int idx = 0; idx < 3; idx++) {
          if (!arr_to[idx].is_number()) {
            ::VCL_report(VCL_report_type_t::error,
                         "one or more element in array \"to\" is not number.");
            return false;
          }
          ele.to[idx] = arr_to[idx];
        }
      }

      // faces
      {
        if (!e.contains("faces") || !e.at("faces").is_object()) {
          ::VCL_report(VCL_report_type_t::error,
                       "\"faces\" doesn't exist, or is not an object.");
          return false;
        }

        const njson &faces = e.at("faces");
        for (auto temp : faces.items()) {
          // if the face is not object, skip current face.
          if (!temp.value().is_object()) continue;
          face_json_temp f;
          block_model::face_idx fidx;
          {
            auto fidx_opt = string_to_face_idx(temp.key());
            if (not fidx_opt) {
              std::string msg = fmt::format(
                  "Error while parsing block model json : invalid key {} "
                  "doesn't refer to any face.",
                  temp.key());
              ::VCL_report(VCL_report_type_t::error, msg.c_str());
              return false;
            }
            fidx = fidx_opt.value();
          }

          const njson &curface = temp.value();

          if (!curface.contains("texture") ||
              !curface.at("texture").is_string()) {
            ::VCL_report(
                VCL_report_type_t::error,
                "Error while parsing block model json : face do not have "
                "texture.");
            return false;
          }

          f.texture = curface.at("texture");
          if (f.texture.starts_with("block/")) {
            f.texture = ("minecraft:") + f.texture;
          }
          // finished texture

          // cullface
          {
            std::string cullface_temp("");
            if (curface.contains("cullface") &&
                curface.at("cullface").is_string()) {
              cullface_temp = curface.at("cullface");
            }

            if (!cullface_temp.empty()) {
              auto cullface_fidx = string_to_face_idx(cullface_temp);

              if (not cullface_fidx) {
                std::string msg = fmt::format("Invalid value for cullface : {}",
                                              cullface_temp);
                ::VCL_report(VCL_report_type_t::error, msg.c_str());
                return false;
              }
              f.cullface_face = cullface_fidx.value();
              f.have_cullface = true;
            }
          }
          // finished cullface

          // uv
          if (curface.contains("uv") && curface.at("uv").is_array()) {
            const njson::array_t &uvarr = curface.at("uv");

            if (uvarr.size() != 4) {
              ::VCL_report(VCL_report_type_t::error,
                           "Invalid value for uv array : the size must be 4.");
              return false;
            }

            for (int idx = 0; idx < 4; idx++) {
              if (!uvarr.at(idx).is_number()) {
                ::VCL_report(VCL_report_type_t::error,
                             "Invalid value for uv array : the value must be "
                             "numbers.");
                return false;
              }
              f.uv[idx] = uvarr[idx];
            }
          }
          // finished uv

          f.is_hidden = false;
          // finished is_hidden

          // write in this face
          ele.faces[int(fidx)] = f;
        }
      }
      // finished all faces

      dest->elements.emplace_back(ele);
    }
  }

  return true;
}

const char *dereference_texture_name(
    std::map<std::string, std::string>::iterator it,
    std::map<std::string, std::string> &text) noexcept {
  if (it == text.end()) {
    return nullptr;
  }

  if (!it->second.starts_with('#')) {
    return it->second.data();
  }

  // here it->second must be a # reference.

  auto next_it = text.find(it->second.data() + 1);

  // This line is added as a patch, to fix error when parsing 1.19.3 data packs.
  // I'm not sure whether models that triggered this can be parsed correctly, it
  // is only introduced to prevent endless recursion, so that errors can be
  // reported
  if (next_it == it) {
    // found a self-reference value
    return nullptr;
  }

  const char *const ret = dereference_texture_name(next_it, text);

  if (ret != nullptr) {
    // found a non-reference value
    it->second = ret;
    return ret;
  } else {
    // it->second is the the farest reference and no further link
    return it->second.data();
  }
}

void dereference_texture_name(
    std::map<std::string, std::string> &text) noexcept {
  for (auto it = text.begin(); it != text.end(); ++it) {
    if (!it->second.starts_with('#')) continue;
    dereference_texture_name(it, text);
  }
}

void dereference_model(block_model_json_temp &model) {
  // dereference_texture_name(model.textures);

  for (auto &ele : model.elements) {
    for (auto &face : ele.faces) {
      if (face.is_hidden) continue;
      if (face.texture.starts_with('#')) {
        auto it = model.textures.find(face.texture.data() + 1);

        if (it == model.textures.end()) {
          continue;
        }
        face.texture = it->second;
      }
    }
    // finished current face
  }
  // finished current element
}

bool model_json_inherit_new(block_model_json_temp &child,
                            block_model_json_temp &parent, const bool) {
  if (child.parent.empty()) {
    ::VCL_report(VCL_report_type_t::error, "child has no parent.");
    return false;
  }

  parent.is_inherited = true;

  // child.textures.reserve(child.textures.size() + parent.textures.size());
  // merge textures
  for (const auto &pt : parent.textures) {
    if (!child.textures.contains(pt.first)) {
      child.textures.emplace(pt.first, pt.second);
    }
  }

  dereference_texture_name(child.textures);

  // if child have a parent, and child doesn't define its own element, child
  // inherit parent's elements.
  if (child.elements.size() <= 0) {
    child.elements = parent.elements;
  }

  dereference_model(child);

  // parent
  child.parent = parent.parent;
  return true;
}

bool inherit_recrusively(std::string_view childname,
                         block_model_json_temp &child,
                         std::unordered_map<std::string, block_model_json_temp>
                             &temp_models) noexcept {
  if (child.parent.empty()) return true;

  // #warning This function is not finished yet. I hope to inherit from the
  // root, which measn to find the root and inherit from root to leaf

  // find parent till the root
  auto it = temp_models.find(child.parent);

  if (it == temp_models.end()) {
    std::string msg = fmt::format(
        "Failed to inherit. Undefined reference to model {}, "
        "required by {}.",
        child.parent.data(), childname.data());
    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  if (!it->second.parent.empty()) {
    // find root
    const bool success =
        inherit_recrusively(it->first, it->second, temp_models);
    if (!success) {
      return false;
    }
  }
  /*
  printf("\ninhering : parent : %s, child : %s,\n", child.parent.data(),
         childname.data());
         */
  const bool success = model_json_inherit_new(child, it->second, false);

  // dereference_texture_name(child.textures);

  if (!success) {
    std::string msg = fmt::format("Failed to inherit. Child : {}, parent : {}.",
                                  childname.data(), child.parent.data());
    ::VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  return true;
}

bool resource_pack::add_block_models(
    const zipped_folder &resource_pack_root,
    const bool on_conflict_replace_old) noexcept {
  const std::unordered_map<std::string, zipped_file> *files;
  // find assets/minecraft/models/block
  {
    const zipped_folder *temp = resource_pack_root.subfolder("assets");
    if (temp == nullptr) return false;
    temp = temp->subfolder("minecraft");
    if (temp == nullptr) return false;
    temp = temp->subfolder("models");
    if (temp == nullptr) return false;
    temp = temp->subfolder("block");
    if (temp == nullptr) return false;

    files = &temp->files;
  }

  // the name of model is : block/<model-name>
  std::unordered_map<std::string, block_model_json_temp> temp_models;

  temp_models.reserve(files->size());

  std::array<char, 1024> buffer;

  for (const auto &file : *files) {
    if (!file.first.ends_with(".json")) continue;
    buffer.fill('\0');
    std::strcpy(buffer.data(), "block/");
    {
      const int end = file.first.find_last_of('.');
      char *const dest = buffer.data() + std::strlen(buffer.data());
      for (int idx = 0; idx < end; idx++) {
        dest[idx] = file.first[idx];
      }
    }

    block_model_json_temp bmjt;

    const bool ok = parse_single_model_json(
        (const char *)file.second.data(),
        (const char *)file.second.data() + file.second.file_size(), &bmjt);

    if (!ok) {
      std::string msg = fmt::format(
          "Failed to parse assets/minecraft/models/block/{}.", file.first);
      ::VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    temp_models.emplace(buffer.data(), bmjt);
  }
  // parsed all jsons
  /*
  printf("Loaded %i model jsons.\n", int(temp_models.size()));

  for (const auto &file : temp_models) {
    printf("%s, ", file.first.data());
  }
  printf("\n\n");

  */

  // inherit
  for (auto &model : temp_models) {
    const bool ok = inherit_recrusively(model.first, model.second, temp_models);
    if (!ok) {
      model.second.parent = "INVALID";
      std::string msg = fmt::format(
          "Failed to inherit model {}. This model will be "
          "skipped, but it may cause further errors.",
          model.first);
      ::VCL_report(VCL_report_type_t::warning, msg.c_str());
      // #warning following line should be commented.
      // return false;
      continue;
    }

    dereference_texture_name(model.second.textures);
    dereference_model(model.second);
  }
  // remove invalid
  for (auto it = temp_models.begin(); it != temp_models.end();) {
    if (it->second.parent == "INVALID") {
      it = temp_models.erase(it);
      continue;
    }

    ++it;
  }

  // convert temp models to block_models
  this->block_models.reserve(this->block_models.size() + temp_models.size());
  for (auto &tmodel : temp_models) {
    if (this->block_models.contains(tmodel.first) && !on_conflict_replace_old) {
      continue;
    }

    block_model::model md;
    bool skip_this_model = false;

    md.elements.reserve(tmodel.second.elements.size());
    for (auto &tele : tmodel.second.elements) {
      if (skip_this_model) break;
      block_model::element ele;

      // ele._from = tele.from;
      for (int idx = 0; idx < 3; idx++) {
        ele._from[idx] = tele.from[idx];
        ele._to[idx] = tele.to[idx];
      }

      for (uint8_t faceidx = 0; faceidx < 6; faceidx++) {
        if (skip_this_model) break;
        auto &tface = tele.faces[faceidx];
        ele.faces[faceidx].is_hidden = tface.is_hidden;
        if (tface.is_hidden) {
          ele.faces[faceidx].texture = nullptr;
          continue;
        }
        ele.faces[faceidx].uv_start[0] = tface.uv[0];
        ele.faces[faceidx].uv_start[1] = tface.uv[1];
        ele.faces[faceidx].uv_end[0] = tface.uv[2];
        ele.faces[faceidx].uv_end[1] = tface.uv[3];

        // try to find the image in texture/block
        auto imgptr = this->find_texture(tface.texture, false);
        if (imgptr == nullptr) {  // try to resolve the name of this texture
          auto it = tmodel.second.textures.find(tface.texture);
          if (it not_eq tmodel.second.textures.end()) {
            imgptr = this->find_texture(it->second, false);
          }
        }

        if (imgptr == nullptr) {
          if (tface.texture.starts_with('#') && tmodel.second.is_inherited) {
            // This model is considered to be abstract
            skip_this_model = true;
            continue;
          }
          std::string msg = fmt::format(
              "Undefined reference to texture \"{}\", required by "
              "model {} but no such image.\nThe textures are : \n",
              tface.texture, tmodel.first);
          for (const auto &pair : tmodel.second.textures) {
            msg.push_back('{');
            std::string temp =
                fmt::format("{}, {}\n", pair.first.data(), pair.second.data());
            msg.append(temp);
            msg.push_back('}');
          }
          ::VCL_report(VCL_report_type_t::error, msg.c_str());
          return false;

          // if managed to find, go on
        }
        ele.faces[faceidx].texture = imgptr;
      }
      // finished all faces

      md.elements.emplace_back(ele);
    }
    // finished current model
    if (!skip_this_model) {
      this->block_models.emplace(tmodel.first, std::move(md));
    }
  }

  for (const auto &pair : this->block_models) {
    for (const auto &ele : pair.second.elements) {
      for (const auto &face : ele.faces) {
        if (!face.is_hidden && face.texture == nullptr) {
          std::string msg = fmt::format(
              "Found an error while examining all block models : "
              "face.texture==nullptr in model {}",
              pair.first);
          ::VCL_report(VCL_report_type_t::error, msg.c_str());
          return false;
        }
      }
    }
  }

  return true;
}

bool resource_pack::add_block_states(
    const zipped_folder &resourece_pack_root,
    const bool on_conflict_replace_old) noexcept {
  const std::unordered_map<std::string, zipped_file> *files = nullptr;
  {
    const zipped_folder *temp = resourece_pack_root.subfolder("assets");
    if (temp == nullptr) {
      return false;
    }
    temp = temp->subfolder("minecraft");
    if (temp == nullptr) {
      return false;
    }
    temp = temp->subfolder("blockstates");
    if (temp == nullptr) {
      return false;
    }
    files = &temp->files;
  }

  this->block_states.reserve(this->block_states.size() + files->size());

  for (const auto &file : *files) {
    if (this->block_states.contains(file.first) && !on_conflict_replace_old) {
      continue;
    }
    std::variant<resource_json::block_states_variant,
                 resource_json::block_state_multipart>
        bs;
    bool is_dest_variant;

    const bool success = parse_block_state(
        (const char *)file.second.data(),
        (const char *)file.second.data() + file.second.file_size(), &bs,
        &is_dest_variant);

    if (!success) {
      std::string msg = fmt::format(
          "Failed to parse block state json file "
          "assets/minecraft/blockstates/{}. This will be "
          "skipped but may cause further errors.\n",
          file.first);

      ::VCL_report(VCL_report_type_t::warning, msg.c_str());
      continue;
    }

    const int substrlen = file.first.find_last_of('.');
    this->block_states.emplace(file.first.substr(0, substrlen), std::move(bs));
  }

  return true;
}
