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

#include <QDesktopServices>

#include "ui_VersionDialog.h"

VersionDialog::VersionDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::VersionDialog) {
  this->ui->setupUi(this);

  // connect(this->ui->pb_close, &QPushButton::click, this,
  // &QWidget::deleteLater);
}

VersionDialog::~VersionDialog() { delete this->ui; }

void VersionDialog::setup_text(QString title, QString content,
                               QString markdown_content,
                               QString url_download_) noexcept {
  this->url_download = url_download_;
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

version_info extract_latest_version(std::string_view json_all_release) noexcept(
    false) {
  using njson = nlohmann::json;

  njson jo = njson::parse(json_all_release);

  version_info ret;

  njson latest;

  uint64_t latest_version = 0;

  for (size_t i = 0; i < jo.size(); i++) {
    njson &cur_jo = jo.at(i);

    std::string tag = cur_jo.at("tag_name");

    const uint64_t ver = version_string_to_u64(tag.data());

    if ((ver >> 48) not_eq SC_VERSION_MAJOR_U16) {
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
        "Failed to find any release that matches current major version.");
  }

  ret.tag_name = QString::fromStdString(latest.at("tag_name"));
  ret.html_url = QString::fromStdString(latest.at("html_url"));

  std::string body = latest.at("body");

  ret.body = QString::fromUtf8(body.c_str());
  ret.version_u64 = latest_version;
  return ret;
}

#include <QNetworkReply>

void version_dialog_private_fun_when_network_finished(
    QWidget *window, QNetworkReply *reply, bool is_manually,
    QString software_name) noexcept;

void VersionDialog::start_network_request(
    [[maybe_unused]] QWidget *window, QString software_name, const QUrl &url,
    QNetworkAccessManager &manager,
    [[maybe_unused]] bool is_manually) noexcept {
  QNetworkRequest request(url);

  QNetworkReply *reply = manager.get(request);

  connect(reply, &QNetworkReply::finished,
          [window, reply, is_manually, software_name]() {
            version_dialog_private_fun_when_network_finished(
                window, reply, is_manually, software_name);
          });
}

#include <QDir>
#include <QFile>
#include <QMessageBox>

void version_dialog_private_fun_when_network_finished(
    QWidget *window, QNetworkReply *reply, bool is_manually,
    QString software_name) noexcept {
  const QByteArray content_qba = reply->readAll();
  version_info info;
  try {
    info = extract_latest_version(content_qba.data());
  } catch (std::exception &e) {
#if WIN32
    const QString home_path = QDir::homePath() + "/AppData/Local";
    const QString data_dir_name = "SlopeCraft";
#else
    const QString home_path = QDir::homePath();
    const QString data_dir_name = ".SlopeCraft";
#endif
    QString data_dir = home_path + "/" + data_dir_name;
    QString log_file = data_dir.append("/UpdateCheckFailure.log");
    {
      if (not QDir(data_dir).exists()) {
        QDir{home_path}.mkpath(data_dir_name);
      }

      QFile log(log_file);
      log.open(QFile::OpenMode::enum_type::WriteOnly);
      log.write(content_qba);
      log.close();
    }
    if (is_manually) {
      QMessageBox::warning(
          window, QWidget::tr("获取最新版本失败"),
          QWidget::tr("解析 \"%1\" "
                      "返回的结果时出现错误：\n\n%"
                      "2\n\n这不是一个致命错误，不影响软件使用。\n解析失败的信"
                      "息已经存储在日志文件中 (%3)。")
              .arg(reply->url().toString(), e.what(), log_file),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore});
    }
    reply->deleteLater();
    return;
  }

  reply->deleteLater();

  const uint64_t latest_ver = info.version_u64;

  const auto &tag_name = info.tag_name;

  const uint64_t ui_ver = SC_VERSION_U64;

  if (ui_ver == latest_ver) {
    if (is_manually) {
      QMessageBox::information(window, QWidget::tr("检查更新成功"),
                               QWidget::tr("您在使用的是最新版本"));
    }

    return;
  }

  if (ui_ver > latest_ver) {
    if (is_manually) {
      QMessageBox::information(
          window, QWidget::tr("检查更新成功"),
          QWidget::tr("您使用的版本 (%1) 比已发布的 (%2) 更新，可能是测试版。")
              .arg(SC_VERSION_STR, tag_name));
    }
    return;
  }

  {
    VersionDialog *vd = new VersionDialog(window);
    vd->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    vd->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
    vd->setWindowFlag(Qt::WindowType::Window, true);

    vd->setup_text(
        QWidget::tr("%1 已更新").arg(software_name),
        QWidget::tr("最新版本为%1，当前版本为%2").arg(tag_name, SC_VERSION_STR),
        info.body, info.html_url);

    vd->show();
    return;
  }
}