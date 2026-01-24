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

#include "MapViewerWind.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "processMapFiles.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();

  bool en_US = !uiLanguages.contains("zh");

  for (int i = 0; i < argc; i++) {
    if (std::string_view(argv[i]) == "--lang-force-to-en") {
      en_US = true;
      break;
    }

    if (std::string_view(argv[i]) == "--lang-force-to-zh") {
      en_US = true;
      break;
    }
  }

  if (en_US) {
    if (translator.load(":/i18n/MapViewer_en_US.qm")) {
      a.installTranslator(&translator);
    } else {
      qDebug("Failed to load \":/i18n/MapViewer_en_US.qm\"");
    }
  }

  MapViewerWind w;
  w.show();
  return a.exec();
}
