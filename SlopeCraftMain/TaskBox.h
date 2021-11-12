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

private:
    Ui::TaskBox *ui;
    TokiTask task;
};

#endif // TASKBOX_H
