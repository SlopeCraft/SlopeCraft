#ifndef BATCHUI_H
#define BATCHUI_H

#include <QMainWindow>

namespace Ui {
class BatchUi;
}

class BatchUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit BatchUi(QWidget *parent = nullptr);
    ~BatchUi();

private:
    Ui::BatchUi *ui;
};

#endif // BATCHUI_H
