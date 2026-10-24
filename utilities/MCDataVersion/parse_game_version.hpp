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

#ifndef SLOPECRAFT_PARSE_GAME_VERSION_H
#define SLOPECRAFT_PARSE_GAME_VERSION_H

#include "MCDataVersion.h"
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace MCDataVersion {

inline std::optional<SCL_gameVersion> parse_version_from_njson(
    const nlohmann::json& jo) {
  if (jo.is_string()) {
    const std::string str = jo;
    return magic_enum::enum_cast<SCL_gameVersion>(str);
  }
  if (jo.is_number_integer()) {
    const int num = jo;
    return magic_enum::enum_cast<SCL_gameVersion>(num);
  }
  return std::nullopt;
}

}  // namespace MCDataVersion

#endif  // SLOPECRAFT_PARSE_GAME_VERSION_H
