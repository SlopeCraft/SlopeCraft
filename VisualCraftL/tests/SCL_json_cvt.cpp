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

#include <fstream>
#include <iostream>

using std::cout, std::endl;
using njson = nlohmann::json;

njson &get_scl_block_list(njson &scl) noexcept {

  if (scl.contains("FixedBlocks") && scl.at("FixedBlocks").is_array()) {
    return scl.at("FixedBlocks");
  }

  return scl.at("CustomBlocks");
}

void try_copy_trait(std::string_view trait_name, const njson &obj_scl,
                    njson &obj_vcl) {

  if (obj_scl.contains(trait_name)) {
    obj_vcl.emplace(trait_name, obj_scl.at(trait_name));
  }
}

int main(int argc, char **argv) {

  if (argc != 3) {
    cout << "Usage : SCL_json_cvt [SCL json file] [VCL json file]" << endl;
    return 1;
  }

  njson scl, vcl;

  try {
    std::ifstream ifs(argv[1]);
    scl = njson::parse(ifs, nullptr, true, true);
    ifs.close();
  } catch (std::exception &re) {
    cout << "Failed to parse SCL json. Detail : " << re.what() << endl;
    return 1;
  }

  njson &scl_blocks = get_scl_block_list(scl);

  for (size_t i = 0; i < scl_blocks.size(); i++) {
    const njson &obj_scl = scl_blocks.at(i);

    njson obj_vcl;

    obj_vcl.emplace("nameZH", obj_scl.at("nameZH"));
    obj_vcl.emplace("nameEN", obj_scl.at("nameEN"));

    // other properties

    try_copy_trait("burnable", obj_scl, obj_vcl);
    try_copy_trait("endermanPickable", obj_scl, obj_vcl);
    try_copy_trait("isGlowing", obj_scl, obj_vcl);
    try_copy_trait("needGlass", obj_scl, obj_vcl);
    try_copy_trait("wallUseable", obj_scl, obj_vcl);
    try_copy_trait("burnable", obj_scl, obj_vcl);
    try_copy_trait("burnable", obj_scl, obj_vcl);

    int version_scl = obj_scl.at("version");

    if (obj_scl.contains("idOld")) {
      obj_vcl["version"] = 13;
    }

    if (version_scl == 0) {
      obj_vcl.emplace("version", "all");
    } else {
      obj_vcl.emplace("version", version_scl);
    }

    if (obj_scl.contains("idOld")) {
      vcl.emplace(obj_scl.at("id"), obj_vcl);
    } else {
      vcl.emplace(obj_scl.at("id"), std::move(obj_vcl));
      continue;
    }

    obj_vcl["version"] = njson::array({12});
    vcl.emplace(obj_scl.at("idOld"), std::move(obj_vcl));
  }

  std::ofstream ofs(argv[2]);

  ofs << vcl;

  ofs.close();

  return 0;
}