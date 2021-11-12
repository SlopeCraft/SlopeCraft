#ifndef BATCHUI_H
#define BATCHUI_H

#include <QMainWindow>
#include <vector>
#include <QCloseEvent>

#include "TokiTask.h"
#include "TaskBox.h"
#include "ui_BatchUi.h"


namespace Ui {
class BatchUi;
}

class BatchUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit BatchUi(BatchUi **,QWidget *parent = nullptr);
    ~BatchUi();
    void setTasks(const QStringList & ,
                            bool exportLite,
                            bool exportData);

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void erased(TaskBox*);
    void onBoxSeqNumChanged(TaskBox *);

    void on_BtnAddTask_clicked();

    void on_BtnClearTask_clicked();

    void on_BtnExecute_clicked();

private:
    Ui::BatchUi *ui;
    std::vector<TaskBox*> taskBoxes;
    BatchUi ** self;

    ushort ptr2Index(TaskBox * ) const;
    auto ptr2It(TaskBox * ) const;

    void allocateMapSeqNum(ushort);

    void dispTasks() const;

};

#endif // BATCHUI_H
