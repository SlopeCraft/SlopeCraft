#ifndef TASKBOX_H
#define TASKBOX_H

#include <QWidget>

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

    void setTask(TokiTask *);

signals:
    void erase(TaskBox *);
    void modified(TaskBox *);

private slots:
    void on_BtnErase_clicked();

    void onUserUsed();

    void onImageChanged();

    void on_browseImage_clicked();

private:
    Ui::TaskBox *ui;
    TokiTask * task;
};

#endif // TASKBOX_H
