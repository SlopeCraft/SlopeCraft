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

#include <cstdint>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <QImageReader>
#include "CallbackFunctions.h"
#include "VCWind.h"

QNetworkAccessManager *global_manager{nullptr};

QString url_for_update{
    "https://api.github.com/repos/SlopeCraft/SlopeCraft/releases"};

bool parse_config_json(QString &err) noexcept;

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  QImageReader::setAllocationLimit(INT32_MAX);

  ::is_language_ZH = QLocale::system().uiLanguages().contains("zh");

  // this line is used to test the translation

  for (int i = 0; i < argc; i++) {
    if (std::string_view(argv[i]) == "--lang-force-to-en") {
      ::is_language_ZH = false;
      break;
    }
    if (std::string_view(argv[i]) == "--lang-force-to-zh") {
      ::is_language_ZH = true;
      break;
    }
  }

  //::is_language_ZH = false;

  QTranslator translator_self;
  QTranslator translator_version_dialog;
  if (!::is_language_ZH) {
    int err_counter = 0;
    if (translator_self.load(":/i18n/VisualCraft_en_US.qm")) {
      qapp.installTranslator(&translator_self);
    } else {
      err_counter++;
    }

    if (translator_version_dialog.load(":/i18n/VersionDialog_en_US.qm")) {
      qapp.installTranslator(&translator_version_dialog);
    } else {
      err_counter++;
    }

    // if (err_counter > 0) {
    qDebug() << err_counter << " qm file(s) failed to be loaded.";
    //}
  }

  if (!VCL_is_version_ok()) {
    QMessageBox::critical(
        nullptr, VCWind::tr("VisualCraftL 动态库版本不匹配"),
        VCWind::tr(
            "界面程序编译时使用的 VisualCraftL 版本为%"
            "1，而 VisualCraftL 动态库的版本为%2。通常这是因为动态库版本过低。")
            .arg(SC_VERSION_STR)
            .arg(VCL_version_string()));
    qapp.exit(1);
    return 1;
  }

  {
    QString err;
    if (!parse_config_json(err)) {
      QMessageBox::critical(nullptr, VCWind::tr("加载配置文件失败。"), err);
      qapp.exit(1);
      return 1;
    }
  }

  QNetworkAccessManager manager;
  global_manager = &manager;

  VCWind wind;

  VC_callback::wind = &wind;

  VCL_set_report_callback(VC_callback::callback_receive_report);

  wind.setWindowTitle(
      QStringLiteral("VisualCraft v%1  Copyright © 2021-2023 TokiNoBug")
          .arg(SC_VERSION_STR));

  wind.show();

  wind.retrieve_latest_version(::url_for_update, manager, false);

  int ret = qapp.exec();

  return ret;
}

bool parse_config_json(QString &err) noexcept {
  err = "";

  if (!load_config("./vc-config.json", VCWind::config)) {
    err = VCWind::tr("无法加载配置文件\"./vc-config.json\"。\n%1").arg("");
    return false;
  }

  return true;
}