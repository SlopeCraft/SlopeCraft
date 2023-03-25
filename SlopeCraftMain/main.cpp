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

  QJsonObject settings = loadIni(isZH);
  isZH = (settings.value("Language").toString() == "zh_CN");
  /*
  for (const QString &locale : uiLanguages) {
      const QString baseName = "SlopeCraft_" + QLocale(locale).name();
      if (translator.load(":/i18n/" + baseName)) {
          a.installTranslator(&translator);
          break;
      }
  }*/
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

  bool autoCheck = settings["autoCheckUpdates"].toBool();
  if (autoCheck) {
    w.grabVersion(true);
  }

  return a.exec();
}

QJsonObject loadIni(bool isLocalZH) {
  qDebug("开始寻找初始配置文件");
  if (QFile("./settings.json").exists()) {
    qDebug("初始配置文件存在");
    QFile ini("./settings.json");
    ini.open(QFile::OpenModeFlag::ReadWrite | QFile::OpenModeFlag::Text);
    QJsonParseError error;
    QJsonObject jo = QJsonDocument::fromJson(ini.readAll(), &error).object();
    ini.close();
    if (error.error == QJsonParseError::NoError && isValidIni(jo)) {
      qDebug("初始配置文件解析成功");
      return jo;
    } else {
      qDebug("初始配置文件格式无法解析，将会删除");
      qDebug() << error.errorString();
      qDebug() << (ini.remove() ? "删除成功" : "删除失败");
    }
  }
  qDebug("初始配置文件不存在，将重新创建");

  QJsonObject jo;
  jo["Language"] = (isLocalZH ? "zh_CN" : "en_US");
  jo["autoCheckUpdates"] = true;

  MainWindow::putSettings(jo);
  /*
  QFile ini("./settings.json");
  ini.open(QFile::OpenModeFlag::WriteOnly|QFile::OpenModeFlag::Text);
  QString js;
  if(isLocalZH) {
      js="{\n    \"Language\":\"zh_CN\",\n    \"autoCheckUpdates\":true\n}";
  } else
  {
      js="{\n    \"Language\":\"en_US\",\n    \"autoCheckUpdates\":true\n}";
  }
  ini.write(js.toUtf8());
  ini.close();
  */
  qDebug("创建初始配置文件");
  return loadIni(isLocalZH);
}

bool isValidIni(const QJsonObject &jo) {
  if (jo.isEmpty()) {
    qDebug("emptyObject");
    return false;
  }

  const QStringList langs = {"zh_CN", "en_US"};
  if (!jo.contains("Language") ||
      !langs.contains(jo.value("Language").toString())) {
    qDebug() << (jo.contains("Language")
                     ? "Language value=" + jo.value("Language").toString()
                     : "jo doesn't contains Language key");
    return false;
  }
  //
  if (!jo.contains("autoCheckUpdates")) {
    qDebug() << "jo doesn't contains key \"autoCheckUpdates\"";
    return false;
  }
  if (!jo["autoCheckUpdates"].isBool()) {
    qDebug() << "jo[\"autoCheckUpdates\"] is not a boolean value, but "
             << jo["autoCheckUpdates"];
    return false;
  }

  return true;
}
