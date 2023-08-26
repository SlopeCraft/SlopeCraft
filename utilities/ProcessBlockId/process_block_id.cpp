#include "process_block_id.h"
#include <string>

bool blkid::is_valid_id(std::string_view str) noexcept {
  if (str.find_first_of(':') != str.find_first_of(':')) {
    return false;
  }

  const bool have_left = str.find_first_of('[') != str.npos;
  const bool have_right = str.find_last_of(']') != str.npos;

  if (have_right != have_left) {
    return false;
  }

  if (!have_left) {
    return true;
  }

  if (str.find_first_of('[') != str.find_last_of('[')) {
    return false;
  }

  if (str.find_first_of(']') != str.find_last_of(']')) {
    return false;
  }

  if (str.find_first_of('[') >= str.find_first_of(']')) {
    return false;
  }

  return true;
}

bool blkid::process_blk_id(
    std::string_view str, std::string* namespace_name, std::string* pure_id,
    std::vector<std::pair<std::string, std::string>>* attributes) noexcept {
  blkid::char_range nn, pid;
  std::vector<std::pair<char_range, char_range>> atti;

  const bool ok =
      process_blk_id(str, &nn, &pid, (attributes == nullptr ? nullptr : &atti));
  if (!ok) {
    return false;
  }

  if (namespace_name != nullptr) {
    namespace_name->assign(nn.begin(), nn.end());
  }

  if (pure_id != nullptr) {
    pure_id->assign(pid.begin(), pid.end());
  }

  if (attributes != nullptr) {
    attributes->resize(atti.size());

    for (size_t i = 0; i < atti.size(); i++) {
      auto& pair = (*attributes)[i];
      pair.first.assign(atti[i].first.begin(), atti[i].first.end());
      pair.second.assign(atti[i].second.begin(), atti[i].second.end());
    }
  }
  return ok;
}

bool blkid::process_state_list(
    const char_range range,
    std::vector<std::pair<char_range, char_range>>* attributes,
    size_t* num) noexcept {
  constexpr const char* end_charp = "";

  if (attributes != nullptr) {
    attributes->clear();
  }

  size_t counter = 0;

  auto split = range | std::ranges::views::split(',');
  for (auto att_view : split) {
    std::pair<char_range, char_range> ret;

    // constexpr size_t sz = sizeof(ret);

    auto split_eq = att_view | std::ranges::views::split('=');

    size_t i = 0;
    for (auto component : split_eq) {
      if (i == 0) {
        ret.first = component;
      }
      if (i == 1) {
        ret.second = component;
      }
      if (i >= 2) {
        return false;
      }
      i++;
    }

    if (i < 1) {
      ret.second = std::ranges::subrange<const char*>(end_charp, end_charp + 1);
    }

    if (attributes != nullptr) {
      attributes->emplace_back(ret);
    }
    counter++;
  }

  if (num != nullptr) {
    *num = counter;
  }

  return true;
}

bool blkid::process_blk_id(
    std::string_view str, char_range* namespace_name, char_range* pure_id,
    std::vector<std::pair<char_range, char_range>>* attributes,
    id_info* info) noexcept {
  const char* const end_charp = str.data() + str.size();

  if (str.size() <= 0) {
    if (namespace_name != nullptr) {
      *namespace_name =
          std::ranges::subrange<const char*>(end_charp, end_charp + 1);
    }
    if (pure_id != nullptr) {
      *pure_id = std::ranges::subrange<const char*>(end_charp, end_charp + 1);
    }
    if (attributes != nullptr) {
      attributes->clear();
    }

    if (info != nullptr) {
      info->contains_namespace = false;
      info->num_attributes = 0;
    }

    return true;
  }

  if (!is_valid_id(str)) {
    return false;
  }

  const size_t idx_colon = str.find_first_of(':');

  if (idx_colon == str.npos) {
    if (namespace_name != nullptr) {
      *namespace_name =
          std::ranges::subrange<const char*>(end_charp, end_charp + 1);
      if (info != nullptr) {
        info->contains_namespace = false;
      }
    }
  } else {
    if (namespace_name != nullptr) {
      *namespace_name = std::ranges::subrange<const char*>(
          str.data(), str.data() + idx_colon);
      if (info != nullptr) {
        info->contains_namespace = true;
      }
    }
  }

  std::string_view str_no_namespace(str.data() + idx_colon + 1);

  const size_t idx_left = str_no_namespace.find_first_of('[');

  if (pure_id != nullptr) {
    const size_t offset = (idx_left == str_no_namespace.npos)
                              ? (str_no_namespace.size())
                              : (idx_left);

    *pure_id = std::ranges::subrange<const char*>(
        str_no_namespace.data(), str_no_namespace.data() + offset);
  }

  if (attributes != nullptr) {
    attributes->clear();
  }

  if (idx_left == str_no_namespace.npos) {
    if (info != nullptr) {
      info->num_attributes = 0;
    }

    return true;
  }

  const std::ranges::subrange<const char*> str_attribute(
      str_no_namespace.data() + idx_left + 1,
      str_no_namespace.data() + str_no_namespace.size() - 1);

  size_t counter = 0;

  if (!process_state_list(str_attribute, attributes, &counter)) {
    return false;
  }

  if (info != nullptr) {
    info->num_attributes = counter;
  }

  return true;
}