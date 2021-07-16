#include "mainwindow.h"
#include "tpstrategywind.h"
#include "ui_tpstrategywind.h"
#include <QDialog>
#include <QColorDialog>
tpStrategyWind::tpStrategyWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tpStrategyWind)
{
    ui->setupUi(this);
    BackGroundColor=qRgb(220,220,220);
}

void tpStrategyWind::setVal(tpS t)
{
    if(t.pTpS=='B')ui->isPureB->setChecked(true);
    else ui->isPureA->setChecked(true);

    if(t.hTpS=='B')
        ui->isHB->setChecked(true);
    else if(t.hTpS=='C')
        ui->isHS->setChecked(true);
    else ui->isHR->setChecked(true);

    QPalette temp;
    QColor Temp(qRed(t.BGC),qGreen(t.BGC),qBlue(t.BGC));
    temp.setColor(QPalette::Window,Temp);
    ui->ShowBGCCustom->setPalette(temp);
    ui->ShowBGCCustom->setAutoFillBackground(true);
}

void tpStrategyWind::closeEvent(QCloseEvent*event)
{
    emit destroyed();
    delete this;
}

tpStrategyWind::~tpStrategyWind()
{
    qDebug("子窗口析构");
    delete ui;
}


void tpStrategyWind::on_Confirm_clicked()
{
    grabTpSInfo();
    emit Confirm(tpS(pTpS,hTpS,BackGroundColor));
    qDebug("传值完成");
    close();
}

void tpStrategyWind::grabTpSInfo()
{
    pTpS=(ui->isPureB->isChecked())?'B':'A';

    hTpS=(ui->isHB->isChecked())?'B':((ui->isHR->isChecked())?'R':'C');
    if(ui->isBGCWhite->isChecked())BackGroundColor=qRgb(255,255,255);
    else if(ui->isBGCGray->isChecked())BackGroundColor=qRgb(220,220,220);
    qDebug("抓取完成");
}

void tpStrategyWind::on_isBGCCustom_clicked()
{
    QColor Temp=QColorDialog::getColor();
    if(!Temp.isValid())return;
    BackGroundColor=qRgb(Temp.red(),Temp.green(),Temp.blue());
    QPalette temp;
    temp.setColor(QPalette::Window,Temp);
    ui->ShowBGCCustom->setPalette(temp);
    ui->ShowBGCCustom->setAutoFillBackground(true);
}

