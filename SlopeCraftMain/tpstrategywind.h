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

#ifndef TPSTRATEGYWIND_H
#define TPSTRATEGYWIND_H

#include "MainWindow.h"
#include <QMainWindow>

namespace Ui {
class tpStrategyWind;
}
class MainWindow;

#ifndef TPS__
#define TPS__
class tpS {
public:
  tpS(char _pTpS = 'B', char _hTpS = 'C', QRgb _BGC = qRgb(220, 220, 220)) {
    pTpS = _pTpS;
    hTpS = _hTpS;
    BGC = _BGC;
  }
  ~tpS();
  char pTpS;
  char hTpS;
  QRgb BGC;
};
#endif

class tpStrategyWind : public QMainWindow {
  Q_OBJECT

public:
  explicit tpStrategyWind(QWidget *parent = nullptr);
  ~tpStrategyWind();
  MainWindow *parent;
  char pTpS;
  char hTpS;
  QRgb BackGroundColor;
  void setVal(tpS);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::tpStrategyWind *ui;
signals:
  void Confirm(tpS);
public slots:
  void grabTpSInfo();
private slots:
  void on_Confirm_clicked();
  void on_isBGCCustom_clicked();
  void on_Reset_clicked();
};

#endif // TPSTRATEGYWIND_H
