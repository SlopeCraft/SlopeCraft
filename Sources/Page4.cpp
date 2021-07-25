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


#ifndef Page4_H
#define Page4_H
#define reportRate 50
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "mcmap.h"
//#include "ColorAdjust.cpp"

#include <QColorSpace>
#include <QColorTransform>
#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
//#include <QRgb>
#include <time.h>

using namespace Eigen;
void MainWindow::on_ExImage_clicked()
{
    QString ImagePath=QFileDialog::getSaveFileName(this,tr("保存当前显示图片"),"",tr("图片(*.png *.bmp *.jpg *.tif *.GIF )"));
    if(ImagePath.isEmpty())return;
    ui->ShowPic->pixmap().save(ImagePath);
}
void MainWindow::AdjPro(int step)
{
    ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+step);
    //qDebug("调用了AdjPro");
}

void MainWindow::on_isColorSpaceXYZ_clicked()
{
    Data.adjStep=0;
    if(Data.step>=4)Data.step=4;
    else
    {updateEnables();
        return;}
    updateEnables();
    Data.Mode='X';
    qDebug("调整颜色空间为XYZ");
}

void MainWindow::on_isColorSpaceLab94_clicked()
{
    Data.adjStep=0;
    if(Data.step>=4)Data.step=4;
    else
    {updateEnables();
        return;}
    updateEnables();
    Data.Mode='l';
    qDebug("调整颜色空间为Lab94");
}

void MainWindow::on_isColorSpaceLab00_clicked()
{
    Data.adjStep=0;
    if(Data.step>=4)Data.step=4;
    else
    {updateEnables();
        return;}
    updateEnables();
    Data.Mode='L';
    qDebug("调整颜色空间为Lab00");
}

void MainWindow::on_isColorSpaceHSV_clicked()
{
    Data.adjStep=0;
    if(Data.step>=4)Data.step=4;
    else
    {updateEnables();
        return;}
    updateEnables();
    Data.Mode='H';
    qDebug("调整颜色空间为HSV");
}

void MainWindow::on_isColorSpaceRGB_clicked()
{
    Data.adjStep=0;
    if(Data.step>=4)Data.step=4;
    else
    {updateEnables();
        return;}
    updateEnables();
    Data.Mode='R';
    qDebug("调整颜色空间为RGB");
}

void MainWindow::on_isColorSpaceRGBOld_clicked()
{
    Data.adjStep=0;
    if(Data.step>=4)Data.step=4;
    else
    {updateEnables();
        return;}
    updateEnables();
    Data.Mode='r';
    qDebug("调整颜色空间为旧版RGB");
}

void MainWindow::pushToHash(AdjT*R)
{
    if(Data.adjStep<0)return;
    R->colorAdjuster.clear();
    QRgb *CurrentLine;
    int ColorCount=0;
    TokiColor::Allowed=&Data.Allowed;
    TokiColor::Basic=&Data.Basic;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        CurrentLine=(QRgb*)Data.rawPic.scanLine(r);
        for(short c=0;c<Data.sizePic[1];c++)
            if(!R->colorAdjuster.contains(CurrentLine[c]))
            {
                ColorCount++;
                R->colorAdjuster[CurrentLine[c]]=TokiColor(CurrentLine[c],Data.Mode);
            }
        AdjPro(Data.sizePic[1]);
        //qDebug("rua!");
    }
    //qDebug("成功将所有颜色装入QHash");
    //qDebug()<<"总颜色数量"<<ColorCount;
    qDebug()<<"总颜色数量："<<R->colorAdjuster.count();
    Data.adjStep=1;
}

void MainWindow::applyTokiColor(AdjT*R)
{//int ColorCount=0;
    if(Data.adjStep<1)return;


    qDebug("即将开始子线程");
    R->start();
    qDebug("已经开始子线程");
    int step=reportRate*Data.sizePic[0]*Data.sizePic[1]/R->colorAdjuster.count()/2;
    int itered=1;
    auto mid=R->colorAdjuster.begin();
    for(int count=0;count*2>=R->colorAdjuster.count();)
        if(R->colorAdjuster.contains(mid.key()))
        {
                mid++;
                count++;
        }
    //qDebug()<<"step="<<step;
    for(auto i=R->colorAdjuster.begin();i!=R->colorAdjuster.end();i++)//前部遍历
    {
        //if(i==mid)break;
        if(R->colorAdjuster.contains(i.key()))
        {
            if (i.value().Result)continue;//发现有处理过的颜色则跳过
            i.value().apply(i.key());
            //parent->AdjPro(step);
            itered++;
            if(itered%reportRate==0)
            AdjPro(step);
        }
    }
    R->wait();
    qDebug("子线程执行完毕");
    /*int step=Data.sizePic[0]/R->colorAdjuster.count();
    for(auto i=R->colorAdjuster.begin();i!=R->colorAdjuster.end();i++)
    {
        if(R->colorAdjuster.contains(i.key()))
        {
            i.value().apply();
            ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+step);
        }
    }
    //qDebug("成功将hash中所有颜色匹配为地图色");
    //qDebug()<<"调色工作量："<<ColorCount;
    Data.adjStep=2;*/
    Data.adjStep=2;
}

void MainWindow::fillMapMat(AdjT*R)
{
    if(Data.adjStep<2)return;
    QRgb*CurrentLine;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        CurrentLine=(QRgb*)Data.rawPic.scanLine(r);
        for(short c=0;c<Data.sizePic[1];c++)
        {
            Data.mapPic(r,c)=R->colorAdjuster[CurrentLine[c]].Result;
        }
        AdjPro(Data.sizePic[1]);
    }
    //qDebug("成功将原图转为地图画");
    Data.adjStep=3;
}

void MainWindow::getAdjedPic()
{
    if(Data.adjStep<3)return;
    MatrixXi RGBint=(255.0f*Data.Basic._RGB).cast<int>();
    short Index;
    QRgb *CurrentLine=NULL;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        CurrentLine=(QRgb*)Data.adjedPic.scanLine(r);
        for(short c=0;c<Data.sizePic[1];c++)
        {
            if(Data.mapPic(r,c)<=3)
            {
                CurrentLine[c]=qRgba(0,0,0,0);
                continue;
            }
            Index=Data.mapColor2Index(Data.mapPic(r,c));
           // Index=Data.mapPic(r,c);

           CurrentLine[c]=qRgb(RGBint(Index,0),RGBint(Index,1),RGBint(Index,2));
        }
        AdjPro(Data.sizePic[1]);
    }
    Data.adjStep=4;
    //qDebug("成功生成调整后图像");
    return;
}

void MainWindow::on_ShowRaw_clicked()
{
    ui->ShowPic->setPixmap(QPixmap::fromImage(Data.rawPic));
}

void MainWindow::on_ShowAdjed_clicked()
{
    if(Data.adjStep<4)return;
    ui->ShowPic->setPixmap((QPixmap::fromImage(Data.adjedPic)));
}


void MainWindow::on_AdjPicColor_clicked()
{
ui->ExData->setEnabled(false);
ui->ExLite->setEnabled(false);
//ui->ExMcF->setEnabled(false);

ui->isColorSpaceHSV->setEnabled(false);
ui->isColorSpaceRGB->setEnabled(false);
ui->isColorSpaceLab94->setEnabled(false);
ui->isColorSpaceLab00->setEnabled(false);
ui->isColorSpaceXYZ->setEnabled(false);
ui->isColorSpaceRGBOld->setEnabled(false);
ui->AdjPicColor->setEnabled(false);
ui->ShowAdjed->setEnabled(false);

ui->AdjPicColor->setText("请稍等");

Data.adjStep=0;
ui->ShowProgressABbar->setRange(0,4*Data.sizePic[0]*Data.sizePic[1]+1);
//第一步，装入hash顺便转换颜色空间;第二步，遍历hash并匹配颜色;第三步，从hash中检索出对应的匹配结果;第四步，生成调整后图片，显示(1)
ui->ShowProgressABbar->setValue(0);

//Data.CurrentColor.setZero(Data.Allowed._RGB.rows(),3);

if(Data.isCreative())
{
    ui->ShowDataCols->setText(QString::number(ceil(Data.mapPic.cols()/128.0f)));
    ui->ShowDataRows->setText(QString::number(ceil(Data.mapPic.rows()/128.0f)));
    ui->ShowDataCounts->setText(QString::number(ceil(Data.mapPic.cols()/128.0f)*ceil(Data.mapPic.rows()/128.0f)));
    ui->InputDataIndex->setText("0");
}
/*
qDebug("即将开始子线程");
Runner.start();
qDebug("已经开始子线程");
Runner.wait();
qDebug("子线程执行完毕");*/
AdjT Runner(this);
clock_t start;
start=clock();
//t=GetCycleCount();
pushToHash(&Runner);
qDebug()<<"装入qHash用时："<<clock()-start;

int lastValue=ui->ShowProgressABbar->value();
start=clock();
applyTokiColor(&Runner);
qDebug()<<"applyTokiColor用时："<<clock()-start;

ui->ShowProgressABbar->setValue(lastValue+Data.sizePic[0]*Data.sizePic[1]);
start=clock();
fillMapMat(&Runner);
qDebug()<<"fillMapMat用时："<<clock()-start;
getAdjedPic();

Data.adjStep=4;
on_ShowAdjed_clicked();

qDebug()<<"生成调整后图像用时："<<clock()-start;
//start=clock();

ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->maximum());

Data.step=5;
updateEnables();
Data.ExLitestep=0;
Data.ExMcFstep=0;
//qDebug("已显示调整后图像并允许翻页");


ui->ExData->setEnabled(true);
ui->ExLite->setEnabled(!Data.isCreative());
//ui->ExMcF->setEnabled(true);

ui->isColorSpaceHSV->setEnabled(true);
ui->isColorSpaceRGB->setEnabled(true);
ui->isColorSpaceLab94->setEnabled(true);
ui->isColorSpaceLab00->setEnabled(false);
ui->isColorSpaceXYZ->setEnabled(true);
ui->isColorSpaceRGBOld->setEnabled(true);
ui->AdjPicColor->setEnabled(true);
ui->ShowAdjed->setEnabled(true);

ui->AdjPicColor->setText(tr("调整颜色"));

}

#endif
