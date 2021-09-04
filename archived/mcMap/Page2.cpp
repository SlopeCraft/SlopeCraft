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


#ifndef Page2_CPP
#define Page2_CPP

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#pragma once

void MainWindow::grabGameVersion()
{
    if(ui->isGame12->isChecked())        Data.gameVersion=12;
    if(ui->isGame13->isChecked())        Data.gameVersion=13;
    if(ui->isGame14->isChecked())        Data.gameVersion=14;
    if(ui->isGame15->isChecked())        Data.gameVersion=15;
    if(ui->isGame16->isChecked())        Data.gameVersion=16;
    if(ui->isGame17->isChecked())        Data.gameVersion=17;
}

void MainWindow::on_isMapSurvival_clicked()
{
    Data.mapType='S';
}

void MainWindow::on_isMapFlat_clicked()
{
    Data.mapType='F';
}

void MainWindow::on_isMapCreative_clicked()
{
    Data.mapType='C';
}

void MainWindow::on_confirmType_clicked()
{
    static bool needInitialize=true;
    Data.step=3;
    updateEnables();

    TokiColor::needFindSide=false&&Data.isSurvival();
    if(needInitialize)
    {
        IniBL();
        qDebug("IniBL");

        IniEnables();
        qDebug("IniEnables");

        IniNeedGlass();
        qDebug("IniNeedGlass");

        InidoGlow();
        qDebug("InidoGlow");

        for(short base=0;base<64;base++)
            if(Blocks[base][1]==NULL&&Blocks[base][0]!=NULL)
            {
                disconnect(Blocks[base][0],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
                Blocks[base][0]->setChecked(true);
                //Blocks[base][0]->setEnabled(false);
                connect(Blocks[base][0],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
            }

        ui->isBLSurvivalBetter->setChecked(true);
        applyElegant();
        qDebug("applyElegant");

        connect(ui->isBLCreative,SIGNAL(clicked()),this,SLOT(applyVanilla()));
        connect(ui->isBLSurvivalCheaper,SIGNAL(clicked()),this,SLOT(applyCheap()));
        connect(ui->isBLSurvivalBetter,SIGNAL(clicked()),this,SLOT(applyElegant()));
        connect(ui->isBLGlowing,SIGNAL(clicked()),this,SLOT(applyShiny()));

        //connect(ui->isMapFlat,SIGNAL(clicked(bool)),ui->isMapCreative->,SLOT())
    }


    if(!Data.is16())
        for(short i=52;i<64;i++)
            if(Enables[i]!=NULL&&ui->isGame15->isChecked())
                Enables[i]->setChecked(false);

    ui->NextPage2->setEnabled(true);

    versionCheck();
    updateEnables();
    grabGameVersion();

    QString Info;
    if(Data.is17())
        Info=tr("你的地图画将适用1.17+版本\n");
    else if(Data.is16())
        Info=tr("你的地图画将适用1.16+版本\n");
    else
        Info=tr("你的地图画将适用1.12~1.15版本\n");

    if(Data.isFlat())
        Info+=tr("这是一个传统的平版地图画，易于建造\n");
    else
        Info+=tr("这是一个新型的立体地图画，颜色丰富\n");

    if(!Data.isCreative())
        Info+=tr("它可以生存实装");
    else
        Info+=tr("它不可以生存实装，只能导出为地图文件");

    ui->IntroType->setText(Info);

    Data.step=3;
    updateEnables();
    return;

}

#endif
