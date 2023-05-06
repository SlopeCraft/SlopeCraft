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

#include "BlockListManager.h"
#include <json.hpp>
#include <QFile>

using njson = nlohmann::json;

basecolorOption parse_single(const njson& jo) noexcept(false);

blockListPreset load_preset(QString filename, QString& err) noexcept {
  err.clear();

  blockListPreset ret;

  try {
    QFile file(filename);
    file.open(QFile::OpenMode::enum_type::ReadOnly);
    auto qba = file.readAll();

    njson jo = njson::parse(qba.begin(), qba.end(), nullptr, true, true);

    ret.values.resize(jo.size());

    for (size_t idx = 0; idx < jo.size(); idx++) {
      basecolorOption opt = parse_single(jo[idx]);

      if (!ret.values[opt.baseColor].second.isEmpty()) {
        err = QObject::tr("基色 %1 的预设被重复定义。一个基色只能被定义一次。")
                  .arg(int(opt.baseColor));
        return {};
      }

      ret.values[opt.baseColor] = {opt.enabled, opt.blockId};
    }

  } catch (std::exception& e) {
    err = QObject::tr("解析预设 json 时发生异常：\"%1\"").arg(e.what());
    return {};
  }

  return ret;
}

basecolorOption parse_single(const njson& jo) noexcept(false) {
  basecolorOption ret;
  ret.baseColor = jo.at("baseColor");
  std::string id = jo.at("blockId");
  ret.blockId = QString::fromUtf8(id.data());
  ret.enabled = jo.at("enabled");
  return ret;
}

QString serialize_preset(const blockListPreset& preset) noexcept {
  njson::array_t arr;
  arr.resize(preset.values.size());

  for (size_t idx = 0; idx < preset.values.size(); idx++) {
    njson obj;
    obj["baseColor"] = idx;
    obj["blockId"] = preset.values[idx].second.toUtf8();
    obj["enabled"] = preset.values[idx].first;

    arr[idx] = obj;
  }

  std::string str = njson(arr).dump(2);

  return QString::fromUtf8(str);
}

blockListPreset load_preset(QString filename) noexcept(false) {
  QString err;
  auto temp = load_preset(filename, err);

  if (!err.isEmpty()) {
    throw std::runtime_error{err.toUtf8()};
  }

  return temp;
}