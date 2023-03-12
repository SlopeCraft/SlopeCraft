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

  if (QFile(":/i18n/imageCutter_en_US.qm").exists())
    qDebug("成功找到资源文件");
  else
    qDebug("没有找到资源文件");

  if (uiLanguages.contains("zh-CN"))
    goto makeWindow;

  for (const QString &locale : uiLanguages) {
    const QString baseName = "imageCutter_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      qDebug("成功加载翻译文件");
      break;
    }
  }

makeWindow:

  CutterWind w;

  if (argc >= 2) {
    int ret = QMessageBox::warning(
        &w, "SlopeCraft compile-time ERROR",
        "Cannot find hand_x128.dll : Hardware not "
        "invented(0x01919810).\n\n\nSLOPECRAFT_FAILED_TO_HAVE_A_HANDSHAKE_WITH_"
        "YOU\nHAVE_A_NICE_DAY_DONT_REPORT_THIS_AS_AN_ERROR_ITS_JUST_A_JOKE",
        "ReportError", "Exit", "Ok", 2);
    if (ret <= 0) {
      QUrl url("https://github.com/ToKiNoBug/SlopeCraft/issues/new/choose");
      QDesktopServices::openUrl(url);
    }
    exit(0);
    return 0;
  } else {
    w.show();
  }

  return a.exec();
}
