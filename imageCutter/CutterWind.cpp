#include "CutterWind.h"
#include "ui_CutterWind.h"

CutterWind::CutterWind(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CutterWind)
{
    ui->setupUi(this);
}

CutterWind::~CutterWind()
{
    delete ui;
}

