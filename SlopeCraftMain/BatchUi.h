#ifndef BATCHUI_H
#define BATCHUI_H

#include <QMainWindow>
#include <vector>

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
    explicit BatchUi(QWidget *parent = nullptr);
    ~BatchUi();

private slots:
    void erased(TaskBox*);
    void modified(TaskBox*);

private:
    Ui::BatchUi *ui;
    std::vector<TaskBox*> taskBoxes;


};

#endif // BATCHUI_H
