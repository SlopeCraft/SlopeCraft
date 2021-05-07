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


#ifndef Page0_H
#define Page0_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

void MainWindow::on_StartWithSlope_clicked()
{
    if(!Collected)
    {
        Collect();
        qDebug("Collected");

        Collected=true;
        qDebug("StartWithSlope中的初始化部分完成");
    }
    ui->isMapSurvival->setChecked(true);
    on_isMapSurvival_clicked();
    qDebug("成功调整了page2的方块列表设定");
    Data.step=1;
    updateEnables();
    turnToPage(1);
}

void MainWindow::on_StartWithFlat_clicked()
{
    if(!Collected)
    {
        Collect();
        qDebug("Collected");

        Collected=true;
        qDebug("StartWithSlope中的初始化部分完成");
    }
    //ui->isMapSurvival->setChecked(true);
    ui->isMapFlat->setChecked(true);
    on_isMapFlat_clicked();
    qDebug("成功调整了page2的方块列表设定");
    Data.step=1;
    updateEnables();
    turnToPage(1);
}

void MainWindow::on_StartWithNotVanilla_clicked()
{
    if(!Collected)
    {
        Collect();
        qDebug("Collected");

        Collected=true;
        qDebug("StartWithSlope中的初始化部分完成");
    }
    ui->isMapCreative->setChecked(true);
    on_isMapCreative_clicked();
    qDebug("成功调整了page2的方块列表设定");
    Data.step=1;
    updateEnables();
    turnToPage(1);
}



#endif
