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


#ifndef Page1_H
#define Page1_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mcmap.cpp"
#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>


void MainWindow::on_ImportPic_clicked()
{
    static bool needInitialize=true;
    QString Path =QFileDialog::getOpenFileName(this,tr("选择图片"),"",tr("图片(*.png *.bmp *.jpg *.tif *.GIF )"));
    if (Path.isEmpty())
    {//Data.step=1;
        return;}


    if(! ( Data.rawPic.load(Path) ) )
    {
        QMessageBox::information(this,tr("打开图片失败"),tr("要不试试换一张图片吧！"));
        //delete rawPic;
                    return;
    }

    Data.adjedPic.load(Path);
    Data.isTransed2Float=false;
    if(needInitialize)
    {

        GetBLCreative(BLCreative);
        GetBLCheaper(BLCheaper);
        GetBLBetter(BLBetter);
        GetBLGlowing(BLGlowing);
        qDebug("成功初始化四个预设方块列表");

        readFromFile("RGB.Toki",Data.Basic._RGB);
        readFromFile("HSV.Toki",Data.Basic.HSV);
        readFromFile("Lab.Toki",Data.Basic.Lab);
        readFromFile("XYZ.Toki",Data.Basic.XYZ);
        qDebug("成功载入颜色");

        qDebug("导入图片按钮处的初始化部分完成");


        showColorColors();
        qDebug("成功为Colors赋予颜色");
        needInitialize=false;
    }


    Data.sizePic[0]=Data.rawPic.height();//z
    Data.sizePic[1]=Data.rawPic.width();//x

    ui->IntroPicInfo->setText("图片尺寸："+QString::number(Data.sizePic[0])+"×"+QString::number(Data.sizePic[1])+"像素");


    Data.size3D[0]=Data.sizePic[1]+2;
    Data.size3D[2]=Data.sizePic[0]+2;

    Data.rawPicRGBc3[0].setZero(Data.sizePic[0],Data.sizePic[1]);
    Data.rawPicRGBc3[1].setZero(Data.sizePic[0],Data.sizePic[1]);
    Data.rawPicRGBc3[2].setZero(Data.sizePic[0],Data.sizePic[1]);
    Data.rawPicRHLXc3[0].setZero(Data.sizePic[0],Data.sizePic[1]);
    Data.rawPicRHLXc3[1].setZero(Data.sizePic[0],Data.sizePic[1]);
    Data.rawPicRHLXc3[2].setZero(Data.sizePic[0],Data.sizePic[1]);
    Data.mapPic.setZero(Data.sizePic[0],Data.sizePic[1]);

    ui->ShowRawPic->setPixmap(QPixmap::fromImage(Data.rawPic));
    ui->ShowPic->setPixmap(QPixmap::fromImage(Data.rawPic));
    //这里要将图片信息加入到Data
    Data.step=2;
    ui->NextPage->setEnabled(true);

    checkBlockIds();

    return;
}




#endif
