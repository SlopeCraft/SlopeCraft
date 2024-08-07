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

#ifndef SLOPECRAFT_VISUALCRAFT_VERSION_DIALOG_H
#define SLOPECRAFT_VISUALCRAFT_VERSION_DIALOG_H

#include <QDialog>
#include <SC_version_buildtime.h>
#include <QUrl>
#include <QNetworkAccessManager>

class VersionDialog;

namespace Ui {
class VersionDialog;
}

class VersionDialog : public QDialog {
  Q_OBJECT
 private:
  Ui::VersionDialog *ui;
  QString url_download{""};

 public:
  explicit VersionDialog(QWidget *parent);
  ~VersionDialog();

  void setup_text(QString title, QString content, QString markdown_content,
                  QString url_download) noexcept;

  static void start_network_request(QWidget *window, QString software_name,
                                    const QUrl &url,
                                    QNetworkAccessManager &manager,
                                    bool is_manually) noexcept;

 private slots:
  void on_pb_download_clicked() noexcept;
};

struct version_info {
  QString tag_name;
  QString body;
  QString html_url;
  uint64_t version_u64;
};

version_info extract_latest_version(std::string_view json_all_release) noexcept(
    false);

#endif  // SLOPECRAFT_VISUALCRAFT_VERSION_DIALOG_H