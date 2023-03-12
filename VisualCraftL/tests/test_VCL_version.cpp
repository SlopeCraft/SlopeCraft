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

#include <VisualCraftL.h>
#include <iostream>
#include <string>
#include <string_view>

int main() {

  std::string_view ver_str_1 = VCL_version_string();

  std::string ver_str_2;

  for (int i = 0; i < 3; i++) {
    ver_str_2 += std::to_string(VCL_version_component(i));
    ver_str_2.push_back('.');
  }
  ver_str_2.pop_back();

  if (ver_str_1 != ver_str_2) {
    std::cout << "Failed." << std::endl;
    return 1;
  }
  std::cout << "Pass." << std::endl;

  return 0;
}