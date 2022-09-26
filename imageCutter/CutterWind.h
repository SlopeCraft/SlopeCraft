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

#ifndef CUTTERWIND_H
#define CUTTERWIND_H

#include <QImage>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class CutterWind;
}
QT_END_NAMESPACE

class CutterWind : public QMainWindow {
  Q_OBJECT

public:
  CutterWind(QWidget *parent = nullptr);
  ~CutterWind();
private slots:
  void loadImg();
  void saveImg();

  void cutImg();

private:
  void updateImg() const;
  void resizeImg();
  QImage img;
  QString netRawFileName;
  QString rawFileSuffix;
  Ui::CutterWind *ui;
};
#endif // CUTTERWIND_H
