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
#ifndef TpStrategyWind_CPP
#define TpStrategyWind_CPP
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


void tpStrategyWind::on_Reset_clicked()
{
    ui->isPureB->setChecked(true);
    ui->isHS->setChecked(true);
    ui->isBGCGray->setChecked(true);
}

#endif
