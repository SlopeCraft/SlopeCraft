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

#include "CutterWind.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QLocale>
#include <QMessageBox>
#include <QTranslator>
#include <QUrl>

int main(int argc, char *argv[]) {

  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();

  bool en_US = !uiLanguages.contains("zh");

  bool pop_suprise = false;

  for (int i = 0; i < argc; i++) {
    if (std::string_view(argv[i]) == "--lang-force-to-en") {
      en_US = true;
      break;
    }

    if (std::string_view(argv[i]) == "--lang-force-to-zh") {
      en_US = true;
      break;
    }

    if (std::string_view(argv[i]) == "--pop-suprise") {
      pop_suprise = true;
      continue;
    }
  }

  if (en_US) {
    if (translator.load(":/i18n/imageCutter_en_US.qm")) {
      a.installTranslator(&translator);
    } else {
      qDebug("Failed to load \":/i18n/imageCutter_en_US.qm\"");
    }
  }

  CutterWind w;

  if (pop_suprise) {
    int ret = QMessageBox::warning(
        &w, "SlopeCraft compile-time ERROR",
        "Cannot find hand_x128.dll : Hardware not "
        "invented(0x01919810).\n\n\nSLOPECRAFT_FAILED_TO_HAVE_A_HANDSHAKE_WITH_"
        "YOU\nHAVE_A_NICE_DAY_DONT_REPORT_THIS_AS_AN_ERROR_ITS_JUST_A_JOKE",
        "ReportError", "Exit", "Ok", 2);
    if (ret <= 0) {
      QUrl url("https://github.com/SlopeCraft/SlopeCraft/issues/new/choose");
      QDesktopServices::openUrl(url);
    }
    exit(0);
    return 0;
  } else {
    w.show();
  }

  return a.exec();
}
