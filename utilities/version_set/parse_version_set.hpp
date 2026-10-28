/*
    Copyright © 2021-2026 year  TokiNoBug
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
*/

#ifndef SLOPECRAFT_PARSE_VERSION_SET_HPP
#define SLOPECRAFT_PARSE_VERSION_SET_HPP

#include <optional>

#include <nlohmann/json.hpp>

#include <parse_game_version.hpp>

#include "version_set.hpp"

inline std::optional<version_set> parse_version_set(
    const nlohmann::json& jo) noexcept {
  if (jo.is_string() and jo == "all") {
    return version_set::all();
  }

  if (jo.is_number_unsigned() or jo.is_string()) {
    const auto version_opt = MCDataVersion::parse_version_from_njson(jo);
    if (not version_opt) {
      return std::nullopt;
    }
    const auto version = version_opt.value();
    version_set ret{0};
    for (SCL_gameVersion v : magic_enum::enum_values<SCL_gameVersion>()) {
      // invalid version
      if (v > SCL_gameVersion::MAX_VALID or v < SCL_gameVersion::MIN_VALID)
        continue;
      // game version less than block
      if (v < version) continue;
      ret[v] = true;
    }

    return ret;
  }

  if (jo.is_array()) {
    version_set ret;

    const nlohmann::json::array_t& ja = jo;

    for (const auto& val : ja) {
      const auto version_opt = MCDataVersion::parse_version_from_njson(val);
      if (not version_opt) {
        const auto msg =
            std::format("{} is not a valid major game version", val.dump());
        VCL_report(VCL_report_type_t::error, msg.c_str());
        return std::nullopt;
      }
      const SCL_gameVersion v = version_opt.value();

      if (v > SCL_gameVersion::MAX_VALID) {
        break;
      }
      ret[v] = true;
    }

    return ret;
  }

  return std::nullopt;
}
#endif  // SLOPECRAFT_PARSE_VERSION_SET_HPP
