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


#ifndef Pages_H
#define Pages_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

void MainWindow::updateEnables()
{
    if(true)//用来锁住暂且没有实现的界面
    {
        ui->ExMcF->setEnabled(false);
        ui->ExData->setEnabled(false);
    }
    if(Data.step<1)
    {
        ui->NextPage->setEnabled(false);
    }
    if(Data.step<2)
    {
        ui->NextPage2->setEnabled(false);
    }
    if(Data.step<3)
    {
        ui->NextPage3->setEnabled(false);
        Data.adjStep=-1;
        ui->ShowProgressABbar->setValue(0);
        ui->ShowProgressABbar->setMaximum(0);
    }
    if(Data.step<4)
    {
        ui->ExLite->setEnabled(false);
        ui->ExMcF->setEnabled(false);
        ui->ExData->setEnabled(false);
        Data.ExLitestep=-1;
        ui->ShowProgressExLite->setValue(0);
        ui->ShowProgressExLite->setMaximum(0);
        Data.ExMcFstep=-1;
        ui->ShowExMcFProgress->setValue(0);
        ui->ShowExMcFProgress->setValue(0);
    }
    if(Data.step<5)
    {
        ui->seeExported->setEnabled(false);
    }

}

void MainWindow::turnToPage(int page)
{
    page%=9;
    updateEnables();
    switch (page)
    {
        case 0:
            if(Data.step>=page)
            {
                ui->ShowStep->setText("Step 0 / 6");
                ui->ShowStepName->setText("开始");
ui->stackedWidget->setCurrentIndex(page);
            }return;

    case 1:
        if(Data.step>=page)
        {
            ui->ShowStep->setText("Step 1 / 6");
            ui->ShowStepName->setText("导入图片");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    case 2:
        if(Data.step>=page)
        {
            ui->ShowStep->setText("Step 2 / 6");
            ui->ShowStepName->setText("设置地图画类型");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    case 3:
        if(Data.step>=page)
        {
            ui->ShowStep->setText("Step 3 / 6");
            ui->ShowStepName->setText("设置方块列表");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    case 4:
        if(Data.step>=page)
        {
            ui->ShowStep->setText("Step 4 / 6");
            ui->ShowStepName->setText("调整颜色");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    case 5:
        if(Data.step>=5)
        {
            ui->ShowStep->setText("Step 5 / 6");
            ui->ShowStepName->setText("导出为投影");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    case 6:
        if(Data.step>=5)
        {
            ui->ShowStep->setText("Step 5 / 6");
            ui->ShowStepName->setText("导出为mcfunction");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    case 7:
        if(Data.step>=5)
        {
            ui->ShowStep->setText("Step 5 / 6");
            ui->ShowStepName->setText("导出为地图文件");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    case 8:
        if(true)
        {
            ui->ShowStep->setText("Step 6 / 6");
            ui->ShowStepName->setText("结束");
ui->stackedWidget->setCurrentIndex(page);
        }return;
    default:
        qDebug("尝试翻页错误");
        return;
    }



    return;
}


void MainWindow::turnToPage0()
{
    turnToPage(0);
    return;
}

void MainWindow::turnToPage1()
{
    turnToPage(1);
    return;
}

void MainWindow::turnToPage2()
{
    turnToPage(2);
    return;
}

void MainWindow::turnToPage3()
{
    turnToPage(3);
    return;
}

void MainWindow::turnToPage4()
{
    turnToPage(4);
    return;
}

void MainWindow::turnToPage5()
{
    turnToPage(5);
    return;
}

void MainWindow::turnToPage6()
{
    turnToPage(6);
    return;
}

void MainWindow::turnToPage7()
{
    turnToPage(7);
    return;
}

void MainWindow::turnToPage8()
{
    turnToPage(8);
    return;
}

#endif
