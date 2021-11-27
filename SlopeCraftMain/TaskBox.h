/*
 Copyright © 2021  TokiNoBug
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

#ifndef TASKBOX_H
#define TASKBOX_H

#include <QWidget>
#include <QFileDialog>
#include "TokiTask.h"
#include "ui_TaskBox.h"

namespace Ui{
class TaskBox;
}

class BatchUi;

class TaskBox : public QWidget
{
    Q_OBJECT
    friend class BatchUi;
public:
    explicit TaskBox(QWidget *parent = nullptr);

    void updateTask();

signals:
    void erase(TaskBox *);
    void seqNumChanged(TaskBox *);

private slots:
    void on_BtnErase_clicked();

    void onImageChanged(QString);

    void on_browseImage_clicked();

    void on_ifExport3D_stateChanged(int arg1);

    void on_ifExportData_stateChanged(int arg1);

    void on_setMapBegSeq_valueChanged(int arg1);

    void on_BtnBrowseData_clicked();

    void onTaskTypeUpdated();

private:
    Ui::TaskBox *ui;
    TokiTask task;
};

#endif // TASKBOX_H
