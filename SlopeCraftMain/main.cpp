/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#include "MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  const QStringList uiLanguages = QLocale::system().uiLanguages();

  qDebug() << uiLanguages;

  qDebug() << "当前运行路径：" << QCoreApplication::applicationDirPath();
  // QString DirPath=QCoreApplication::applicationDirPath()+'/';
  QDir::setCurrent(QCoreApplication::applicationDirPath());

  const QStringList ZHLang = {"zh-CN", "zh", "zh-Hans-CN"};

  bool isZH = false;
  for (auto it = ZHLang.cbegin(); it != ZHLang.cend(); it++) {
    if (uiLanguages.contains(*it)) {
      isZH = true;
      break;
    }
  }

  MainWindow w;
  w.show();

  if (isZH)
    w.turnCh();
  else
    w.turnEn();

  w.InitializeAll();

  if (isZH)
    w.turnCh();
  else
    w.turnEn();

  w.grabVersion(true);

  return a.exec();
}
