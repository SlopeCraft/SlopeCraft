#ifndef TASKBOX_H
#define TASKBOX_H

#include <QWidget>
#include "ui_TaskBox.h"

namespace Ui{
class TaskBox;
}

class TaskBox : public QWidget
{
    Q_OBJECT
public:
    explicit TaskBox(QWidget *parent = nullptr);

signals:
private:
    Ui::TaskBox *ui;
};

#endif // TASKBOX_H
