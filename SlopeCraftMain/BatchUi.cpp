#include "BatchUi.h"
#include "ui_BatchUi.h"

BatchUi::BatchUi(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BatchUi)
{
    ui->setupUi(this);
}

BatchUi::~BatchUi()
{
    delete ui;
}
