#include "ParseResourcePack.h"

#include <json.hpp>

using namespace resource_json;

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

const char *
block_states_variant::block_model_name(const state_list &sl) const noexcept {

  for (const auto &pair : this->LUT) {
    if (match_state_list(pair.first, sl)) {
      return pair.second.data();
    }
  }

  return nullptr;
}

std::vector<const char *>
block_state_multipart::block_model_names(const state_list &sl) const noexcept {
  std::vector<const char *> names;

  for (const multipart_pair &pair : this->pairs) {
    if (pair.match(sl))
      names.emplace_back(pair.apply_blockmodel.data());
  }

  return names;
}