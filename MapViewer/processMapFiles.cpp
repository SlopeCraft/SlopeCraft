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

#include <iostream>
#include <fstream>
#include <nbt_tags.h>
#include <io/izlibstream.h>

#include "processMapFiles.h"
#include "MapViewerWind.h"

std::expected<std::unique_ptr<Eigen::Array<uint8_t, 128, 128, Eigen::RowMajor>>,
              QString>
load_map_file(const std::string& filename) noexcept {
  try {
    std::ifstream ifs{filename, std::ios::binary};
    zlib::izlibstream src{ifs};
    const auto [_, comp] = nbt::io::read_compound(src);
    auto& comp_data = comp->at("data").as<nbt::tag_compound>();
    auto& colors = comp_data.at("colors").as<nbt::tag_byte_array>().get();

    if (colors.size() != 128 * 128) {
      return std::unexpected{
        MapViewerWind::tr("地图文件错误：colors应有16384字节，但实际上有 %1")
            .arg(colors.size())};
    }
    std::unique_ptr<Eigen::Array<uint8_t, 128, 128, Eigen::RowMajor>> ret{
      new Eigen::Array<uint8_t, 128, 128, Eigen::RowMajor>};
    ret->fill(0);
    std::memcpy(ret->data(), colors.data(), colors.size());

    return std::move(ret);
  } catch (const std::exception& e) {
    return std::unexpected{MapViewerWind::tr("地图文件错误：%1").arg(e.what())};
  }
}
