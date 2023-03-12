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

#include "VersionDialog.h"
#include "ui_VersionDialog.h"

#include <QDesktopServices>

VersionDialog::VersionDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::VersionDialog) {
  this->ui->setupUi(this);

  // connect(this->ui->pb_close, &QPushButton::click, this,
  // &QWidget::deleteLater);
}

VersionDialog::~VersionDialog() { delete this->ui; }

void VersionDialog::setup_text(QString title, QString content,
                               QString markdown_content,
                               QString url_download) noexcept {
  this->url_download = url_download;
  this->setWindowTitle(title);
  this->ui->label->setText(content);
  this->ui->tb->setMarkdown(markdown_content);
}

void VersionDialog::on_pb_download_clicked() noexcept {
  QDesktopServices::openUrl(QUrl(this->url_download));
}

uint64_t version_string_to_u64(const char *str) noexcept {
  std::vector<int> numbers;

  if (str[0] == 'v') {
    str++;
  }

  auto qsl = QString(str).split('.');

  for (auto &qs : qsl) {
    numbers.emplace_back(qs.toInt());
  }

  while (numbers.size() < 4) {
    numbers.emplace_back(0);
  }

  return SC_MAKE_VERSION_U64(numbers[0], numbers[1], numbers[2], numbers[3]);
}

#include <json.hpp>

version_info
extract_latest_version(std::string_view json_all_releaese) noexcept(false) {
  using njson = nlohmann::json;

  njson jo = njson::parse(json_all_releaese);

  version_info ret;

  njson latest;

  uint64_t latest_version = 0;

  for (size_t i = 0; i < jo.size(); i++) {
    njson &cur_jo = jo.at(i);

    std::string tag = cur_jo.at("tag_name");

    const uint64_t ver = version_string_to_u64(tag.data());

    if ((ver >> 48) != SC_VERSION_MAJOR_U16) {
      continue;
    }

    if (cur_jo.at("prerelease")) {
      continue;
    }

    if (latest_version >= ver) {
      continue;
    }

    latest = cur_jo;
    latest_version = ver;
  }

  if (latest.empty()) {
    throw std::runtime_error(
        "Failed to find any release that matched current major version.");
    return {};
  }

  ret.tag_name = QString::fromStdString(latest.at("tag_name"));
  ret.html_url = QString::fromStdString(latest.at("html_url"));

  std::string body = latest.at("body");

  ret.body = QString::fromUtf8(body.c_str());
  ret.version_u64 = latest_version;
  return ret;
}