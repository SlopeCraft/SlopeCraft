/*
 Copyright © 2021-2026  TokiNoBug
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
#include <algorithm>

#include "MCDataVersion.h"

#include <magic_enum/magic_enum.hpp>

MCDataVersion::MCDataVersion_t MCDataVersion::string_to_data_version(
    const char* str, bool* ok) noexcept {
  const auto mcdv = magic_enum::enum_cast<MCDataVersion::MCDataVersion_t>(str);

  if (mcdv.has_value()) {
    if (ok != nullptr) {
      *ok = false;
    }
    return mcdv.value();
  }
  if (ok != nullptr) {
    *ok = true;
  }
  return {};
}

std::string MCDataVersion::data_version_to_string(
    MCDataVersion::MCDataVersion_t v) noexcept {
  std::string_view mcdv = magic_enum::enum_name(v);

  return std::string{mcdv};
}

MCDataVersion::MCDataVersion_t MCDataVersion::max_supported_version(
    SCL_gameVersion v) noexcept {
  switch (v) {
    case SCL_gameVersion::ANCIENT:
    case SCL_gameVersion::MC12:
      return MCDataVersion_t::Java_1_12_2;
    case SCL_gameVersion::MC13:
      return MCDataVersion_t::Java_1_13_2;
    case SCL_gameVersion::MC14:
      return MCDataVersion_t::Java_1_14_4;
    case SCL_gameVersion::MC15:
      return MCDataVersion_t::Java_1_15_2;
    case SCL_gameVersion::MC16:
      return MCDataVersion_t::Java_1_16_5;
    case SCL_gameVersion::MC17:
      return MCDataVersion_t::Java_1_17_1;
    case SCL_gameVersion::MC18:
      return MCDataVersion_t::Java_1_18_2;
    case SCL_gameVersion::MC19:
      return MCDataVersion_t::Java_1_19_3;
    case SCL_gameVersion::MC20:
      return MCDataVersion_t::Java_1_20_6;
    case SCL_gameVersion::MC21:
      return MCDataVersion_t::Java_1_21_1;
    case SCL_gameVersion::MC26_1:
      return MCDataVersion_t::Java_26_1_2;
    case SCL_gameVersion::MC26_2:
      return MCDataVersion_t::Java_26_2;
    case SCL_gameVersion::MC26_3:
    case SCL_gameVersion::FUTURE:
      return MCDataVersion_t::Java_26_3;
  }
  abort();
}

MCDataVersion::MCDataVersion_t MCDataVersion::min_supported_version(
    SCL_gameVersion v) noexcept {
  switch (v) {
    case SCL_gameVersion::ANCIENT:
    case SCL_gameVersion::MC12:
      return MCDataVersion_t::Java_1_12;
    case SCL_gameVersion::MC13:
      return MCDataVersion_t::Java_1_13;
    case SCL_gameVersion::MC14:
      return MCDataVersion_t::Java_1_14;
    case SCL_gameVersion::MC15:
      return MCDataVersion_t::Java_1_15;
    case SCL_gameVersion::MC16:
      return MCDataVersion_t::Java_1_16;
    case SCL_gameVersion::MC17:
      return MCDataVersion_t::Java_1_17;
    case SCL_gameVersion::MC18:
      return MCDataVersion_t::Java_1_18;
    case SCL_gameVersion::MC19:
      return MCDataVersion_t::Java_1_19;
    case SCL_gameVersion::MC20:
      return MCDataVersion_t::Java_1_20;
    case SCL_gameVersion::MC21:
      return MCDataVersion_t::Java_1_21;
    case SCL_gameVersion::MC26_1:
      return MCDataVersion_t::Java_26_1;
    case SCL_gameVersion::MC26_2:
      return MCDataVersion_t::Java_26_2;
    case SCL_gameVersion::MC26_3:
    case SCL_gameVersion::FUTURE:
      return MCDataVersion_t::Java_26_3;
  }
  abort();
}

MCDataVersion::MCDataVersion_t MCDataVersion::suggested_version(
    SCL_gameVersion v) noexcept {
  return max_supported_version(v);
}

std::vector<SCL_gameVersion> MCDataVersion::valid_major_versions() noexcept {
  const auto full_v = magic_enum::enum_values<SCL_gameVersion>();
  std::vector<SCL_gameVersion> versions;
  for (auto v : full_v) {
    if (v >= SCL_gameVersion::MIN_VALID and v <= SCL_gameVersion::MAX_VALID) {
      versions.emplace_back(v);
    }
  }
  std::sort(versions.begin(), versions.end());
  return versions;
}

std::string MCDataVersion::major_version_to_string(SCL_gameVersion v) noexcept {
  switch (v) {
    case SCL_gameVersion::ANCIENT:
      return "ANCIENT";
    case SCL_gameVersion::MC12:
      return "1.12";
    case SCL_gameVersion::MC13:
      return "1.13";
    case SCL_gameVersion::MC14:
      return "1.14";
    case SCL_gameVersion::MC15:
      return "1.15";
    case SCL_gameVersion::MC16:
      return "1.16";
    case SCL_gameVersion::MC17:
      return "1.17";
    case SCL_gameVersion::MC18:
      return "1.18";
    case SCL_gameVersion::MC19:
      return "1.19";
    case SCL_gameVersion::MC20:
      return "1.20";
    case SCL_gameVersion::MC21:
      return "1.21";
    case SCL_gameVersion::MC26_1:
      return "26.1";
    case SCL_gameVersion::MC26_2:
      return "26.2";
    case SCL_gameVersion::MC26_3:
      return "26.3";
    case SCL_gameVersion::FUTURE:
      return "FUTURE";
  }
}
