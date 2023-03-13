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

#ifndef BATCHUI_H
#define BATCHUI_H

#include <QCloseEvent>
#include <QMainWindow>
#include <vector>

#include "TaskBox.h"
#include "ui_BatchUi.h"

namespace Ui {
class BatchUi;
}

class MainWindow;

namespace SlopeCraft {
class Kernel;
}

class BatchUi : public QMainWindow {
  Q_OBJECT

public:
  explicit BatchUi(BatchUi **, QWidget *parent);
  ~BatchUi();
  void setTasks(const QStringList &);

signals:

protected:
  void closeEvent(QCloseEvent *) override;

private slots:
  void erased(TaskBox *);
  void onBoxSeqNumChanged();
  void onTaskTypeChanged();

  void on_BtnExecute_clicked();

  void on_BtnBrowseDataFolder_clicked();

  void checkExecutable();

  void eraseAllTasks();

private:
  Ui::BatchUi *ui;
  std::vector<TaskBox *> taskBoxes;
  BatchUi **self;

  auto ptr2It(TaskBox *widgetPtr) const;
  MainWindow *wind() const;
  SlopeCraft::Kernel *kernel() const;
};

#endif // BATCHUI_H
