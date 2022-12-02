#include "ParseResourcePack.h"
#include "VisualCraftL/ParseResourcePack.h"

#include <json.hpp>

#include <utilities/Schem/bit_shrink.h>

using namespace resource_json;

using njson = nlohmann::json;

bool resource_json::match_state_list(const state_list &sla,
                                     const state_list &slb) noexcept {
  if (sla.size() <= 0)
    return true;
  if (sla.size() != slb.size())
    return false;
  int match_num = 0;
  for (const state &sa : sla) {
    for (const state &sb : slb) {
      if ((sa.key == sb.key) && (sa.value == sb.value)) {
        match_num++;
      }
    }
  }

  if (match_num < sla.size()) {
    return false;
  } else {
    return true;
  }
}

bool resource_json::match_criteria_list(const criteria_list_and &cl,
                                        const state_list &sl) noexcept {

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

  if (match_num < cl.size()) {
    return false;
  }
  return true;
}

model_pass_t
block_states_variant::block_model_name(const state_list &sl) const noexcept {
  model_pass_t res;
  res.model_name = nullptr;
  for (const auto &pair : this->LUT) {
    if (match_state_list(pair.first, sl)) {
      res = model_pass_t(pair.second);
      return res;
    }
  }

  return res;
}

std::vector<model_pass_t>
block_state_multipart::block_model_names(const state_list &sl) const noexcept {
  std::vector<model_pass_t> res;

  for (const multipart_pair &pair : this->pairs) {
    if (pair.match(sl))
      res.emplace_back(model_pass_t(pair.apply_blockmodel));
  }

  return res;
}

bool parse_block_state_variant(const njson::object_t &obj,
                               block_states_variant *const dest_variant);

bool parse_block_state_multipart(const njson::object_t &obj,
                                 block_state_multipart *const dest_variant);

bool parse_block_state(const std::string_view json_str,
                       block_states_variant *const dest_variant,
                       block_state_multipart *const dest_multipart,
                       bool *const is_dest_variant) noexcept {
  njson::object_t obj;
  try {
    obj = njson::parse(json_str);
  } catch (...) {
    return false;
  }

  const bool has_variant =
      obj.contains("variant") && obj.at("variant").is_object();
  const bool has_multipart =
      obj.contains("multipart") && obj.at("multipart").is_object();

  if (has_variant == has_multipart) {
    return false;
  }

  if (has_variant) {
    *is_dest_variant = true;
    return parse_block_state_variant(obj, dest_variant);
  }

  if (has_multipart) {
    *is_dest_variant = false;
    return parse_block_state_multipart(obj, dest_multipart);
  }
  // unreachable
  return false;
}

bool parse_block_state_list(std::string_view str,
                            state_list *const sl) noexcept {
  sl->clear();

  int substr_start = 0;
  // int substr_end = -1;
  int eq_pos = -1;

  for (int cur = 0;; cur++) {

    if (str[cur] == '=') {
      eq_pos = cur;
      continue;
    }

    if (str[cur] == ',' || str[cur] == '\0') {
      // substr_end = cur;
      if (eq_pos <= 0) {
        printf("\n Function parse_block_state_list failed to parse block state "
               "list : %s\n",
               str.data());
        return false;
      }

      state s;

      s.key = str.substr(substr_start, eq_pos - substr_start);
      s.value = str.substr(eq_pos + 1, cur - (eq_pos + 1));

      sl->emplace_back(s);

      substr_start = cur + 1;
      eq_pos = -1;

      // write in an element
    }

    if (str[cur] == '\0') {
      break;
    }
  }

  return true;
}

model_store_t json_to_model(const njson &obj) noexcept {
  model_store_t res;

  res.model_name = obj.at("model");

  if (obj.contains("x") && obj.at("x").is_number()) {
    const int val = obj.at("x");
    res.x = block_model::int_to_face_rot(val);
  }

  if (obj.contains("y") && obj.at("y").is_number()) {
    const int val = obj.at("y");
    res.y = block_model::int_to_face_rot(val);
  }

  if (obj.contains("uvlock") && obj.at("uvlock").is_boolean()) {
    res.uvlock = obj.at("uvlock");
  }

  return res;
}

bool parse_block_state_variant(const njson::object_t &obj,
                               block_states_variant *const dest) {
  const njson &variants = obj.at("variants").object();

  dest->LUT.clear();
  dest->LUT.reserve(variants.size());

  for (auto &pair : variants.items()) {

    if (!pair.value().is_object()) {
      printf("\nFunction parse_block_state_variant failed to parse json : "
             "value for key "
             "%s is not an object.\n",
             pair.key().data());
      return false;
    }

    const njson &obj = pair.value().object();

    if (!obj.contains("model") || !obj.at("model").is_string()) {
      printf("\nFunction parse_block_state_variant failed to parse json : no "
             "valid value for key \"model\"\n");
      return false;
    }

    std::pair<state_list, model_store_t> p;
    if (!parse_block_state_list(pair.key(), &p.first)) {
      return false;
    }

    p.second = json_to_model(obj);

    dest->LUT.emplace_back(p);
  }

  return true;
}

bool parse_block_state_multipart(const njson::object_t &obj,
                                 block_state_multipart *const dest_variant) {
#warning here
}