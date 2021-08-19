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

void MainWindow::pushToHash(QHash<QRgb,TokiColor>*R)
{
    if(Data.adjStep<0)return;
    R->clear();
    QRgb *CurrentLine;
    int ColorCount=0;
    TokiColor::Allowed=&Data.Allowed;
    TokiColor::Basic=&Data.Basic;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        CurrentLine=(QRgb*)Data.rawPic.scanLine(r);
        for(short c=0;c<Data.sizePic[1];c++)
            if(!R->contains(CurrentLine[c]))
            {
                ColorCount++;
                R->operator[](CurrentLine[c])=TokiColor(CurrentLine[c],Data.Mode);
            }
        AdjPro(Data.sizePic[1]);
        //qDebug("rua!");
    }
    //qDebug("成功将所有颜色装入QHash");
    //qDebug()<<"总颜色数量"<<ColorCount;
    qDebug()<<"总颜色数量："<<R->count();
    Data.adjStep=1;
}

void matchColor(TokiColor * tColor,QRgb qColor) {
    tColor->apply(qColor);
}

void MainWindow::applyTokiColor(QHash<QRgb,TokiColor>*R)
{//int ColorCount=0;
    if(Data.adjStep<1)return;

    int step=Data.sizePic[0]*Data.sizePic[1]/R->count();
    queue<QFuture<void>> taskTracker;
    for(auto it=R->begin();it!=R->end();it++)
        taskTracker.push(QtConcurrent::run(matchColor,&it.value(),it.key()));


    while(!taskTracker.empty()) {
        taskTracker.front().waitForFinished();
        this->AdjPro(step);
        taskTracker.pop();
    }

    qDebug("子线程执行完毕");
    Data.adjStep=2;
}

void MainWindow::fillMapMat(QHash<QRgb,TokiColor>*R)
{
    if(Data.adjStep<2)return;
    QRgb*CurrentLine;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        CurrentLine=(QRgb*)Data.rawPic.scanLine(r);
        for(short c=0;c<Data.sizePic[1];c++)
        {
            Data.mapPic(r,c)=R->operator[](CurrentLine[c]).Result;
        }
        AdjPro(Data.sizePic[1]);
    }
    //qDebug("成功将原图转为地图画");
    Data.adjStep=3;
}

void MainWindow::Dither(QHash<QRgb,TokiColor> *R)
{
#ifdef putDitheredImg
    QImage DitheredImg(Data.sizePic[0],Data.sizePic[1],QImage::Format_ARGB32);
#endif
    cout<<"DitherMapLR="<<endl;
    cout<<DitherMapLR<<endl;
    cout<<"DitherMapRL="<<endl;
    cout<<DitherMapRL<<endl;
    bool isDirLR=true;
    QRgb*RCL=nullptr;
#ifdef putDitheredImg
    QRgb*OCL=nullptr;
#endif
    Data.Dither[0].setZero(Data.sizePic[0]+2,Data.sizePic[1]+2);
    Data.Dither[1].setZero(Data.sizePic[0]+2,Data.sizePic[1]+2);
    Data.Dither[2].setZero(Data.sizePic[0]+2,Data.sizePic[1]+2);

    ArrayXXf *ColorMap=nullptr;
    QRgb Current;
    QRgb (*CvtFun)(float,float,float);
    switch (Data.Mode) {
    case 'R':
        ColorMap=&Data.Basic._RGB;        
        CvtFun=RGB2QRGB;
        break;
    case 'r':
        ColorMap=&Data.Basic._RGB;
        CvtFun=RGB2QRGB;
        break;
    case 'H':
        ColorMap=&Data.Basic.HSV;
        CvtFun=HSV2QRGB;
        break;
    case 'L':
        ColorMap=&Data.Basic.Lab;
        CvtFun=Lab2QRGB;
        break;
    case 'l':
        ColorMap=&Data.Basic.Lab;
        CvtFun=Lab2QRGB;
        break;
    default:
        ColorMap=&Data.Basic.XYZ;
        CvtFun=XYZ2QRGB;
        break;
    }
    ArrayXXf &CM=*ColorMap;
    int index=0;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        RCL=(QRgb*)Data.rawPic.scanLine(r);
        for(short c=0;c<Data.sizePic[1];c++)
        {
            //index=mcMap::mapColor2Index(R->colorAdjuster[RCL[c]].Result);
            Data.Dither[0](r+1,c+1)=R->operator[](RCL[c]).c3[0];
            Data.Dither[1](r+1,c+1)=R->operator[](RCL[c]).c3[1];
            Data.Dither[2](r+1,c+1)=R->operator[](RCL[c]).c3[2];
        }
    }
    qDebug("成功填充了待抖动的矩阵Dither");
    float Error[3];
    int newCount=0;
    TokiColor* oldColor=nullptr;
    for(short r=0;r<Data.sizePic[0];r++)//底部一行、左右两侧不产生误差扩散，只接受误差
    {
        QCoreApplication::processEvents();
        RCL=(QRgb*)Data.rawPic.scanLine(r);
#ifdef putDitheredImg
        OCL=(QRgb*)DitheredImg.scanLine(r);
#endif
        if(isDirLR)//从左至右遍历
        {
            for(short c=0;c<Data.sizePic[1];c++)
            {
                if(qAlpha(RCL[c])<=0)continue;

                Current=CvtFun(Data.Dither[0](r+1,c+1),Data.Dither[1](r+1,c+1),Data.Dither[2](r+1,c+1));
#ifdef putDitheredImg
      OCL[c]=Current;
#endif
                if(!R->contains(Current))
                {
                    R->operator[](Current)=TokiColor(Current,Data.Mode);
                    R->operator[](Current).apply(Current);
                    //装入了一个新颜色并匹配为地图色
                    newCount++;
                }
                Data.mapPic(r,c)=R->operator[](Current).Result;
                index=mcMap::mapColor2Index(Data.mapPic(r,c));

                oldColor=&R->operator[](Current);

                Error[0]=oldColor->c3[0]-CM(index,0);
                Error[1]=oldColor->c3[1]-CM(index,1);
                Error[2]=oldColor->c3[2]-CM(index,2);

                Data.Dither[0].block(r+1,c+1-1,2,3)+=Error[0]*DitherMapLR;
                Data.Dither[1].block(r+1,c+1-1,2,3)+=Error[1]*DitherMapLR;
                Data.Dither[2].block(r+1,c+1-1,2,3)+=Error[2]*DitherMapLR;
            }
            //qDebug("从左至右遍历了一行");
        }
        else
        {
            for(short c=Data.sizePic[1]-1;c>=0;c--)
            {
                if(qAlpha(RCL[c])<=0)continue;

                Current=CvtFun(Data.Dither[0](r+1,c+1),Data.Dither[1](r+1,c+1),Data.Dither[2](r+1,c+1));
#ifdef putDitheredImg
      OCL[c]=Current;
#endif
                if(!R->contains(Current))
                {
                    R->operator[](Current)=TokiColor(Current,Data.Mode);
                    R->operator[](Current).apply(Current);
                    //装入了一个新颜色并匹配为地图色
                    newCount++;
                }
                Data.mapPic(r,c)=R->operator[](Current).Result;
                index=mcMap::mapColor2Index(Data.mapPic(r,c));

                oldColor=&R->operator[](Current);

                Error[0]=oldColor->c3[0]-CM(index,0);
                Error[1]=oldColor->c3[1]-CM(index,1);
                Error[2]=oldColor->c3[2]-CM(index,2);

                Data.Dither[0].block(r+1,c+1-1,2,3)+=Error[0]*DitherMapRL;
                Data.Dither[1].block(r+1,c+1-1,2,3)+=Error[1]*DitherMapRL;
                Data.Dither[2].block(r+1,c+1-1,2,3)+=Error[2]*DitherMapRL;
            }
            //qDebug("从左至右遍历了一行");
        }
        isDirLR=!isDirLR;
        AdjPro(Data.sizePic[1]);
    }
    qDebug("完成了误差扩散");
    qDebug()<<"Hash中共新插入了"<<newCount<<"个颜色";
    /*Data.Dither[0]=Data.Dither[0].block(1,1,Data.sizePic[0],Data.sizePic[1]);
    Data.Dither[1]=Data.Dither[1].block(1,1,Data.sizePic[0],Data.sizePic[1]);
    Data.Dither[2]=Data.Dither[2].block(1,1,Data.sizePic[0],Data.sizePic[1]);

    qDebug("去除了Dither的零边");*/

#ifdef putDitheredImg
      DitheredImg.save("D:\\DitheredRawImage.png");
#endif

}

void MainWindow::getAdjedPic()
{
    if(Data.adjStep<4)return;
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
    Data.adjStep=5;
    //qDebug("成功生成调整后图像");
    return;
}

void MainWindow::on_ShowRaw_clicked()
{
    ui->ShowPic->setPixmap(QPixmap::fromImage(Data.rawPic));
}

void MainWindow::on_ShowAdjed_clicked()
{
    if(Data.adjStep<5)return;
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

ui->AdjPicColor->setText(QObject::tr("请稍等"));

Data.adjStep=0;
ui->ShowProgressABbar->setRange(0,5*Data.sizePic[0]*Data.sizePic[1]);
//第一步，装入hash顺便转换颜色空间;（一次遍历
//第二步，遍历hash并匹配颜色;（一次遍历
//第三步，从hash中检索出对应的匹配结果;（一次遍历
//第四步，抖动（包含四个函数）（一次遍历
//第五步，生成调整后图片，显示
ui->ShowProgressABbar->setValue(0);

//Data.CurrentColor.setZero(Data.Allowed._RGB.rows(),3);

if(Data.isCreative())
{
    ui->ShowDataCols->setText(QString::number(ceil(Data.mapPic.cols()/128.0f)));
    ui->ShowDataRows->setText(QString::number(ceil(Data.mapPic.rows()/128.0f)));
    ui->ShowDataCounts->setText(QString::number(ceil(Data.mapPic.cols()/128.0f)*ceil(Data.mapPic.rows()/128.0f)));
    ui->InputDataIndex->setText("0");
}

auto colorHash=new QHash<QRgb,TokiColor>;
clock_t start;
start=clock();
//t=GetCycleCount();
pushToHash(colorHash);
qDebug()<<"装入qHash用时："<<clock()-start;

QCoreApplication::processEvents();
int lastValue=ui->ShowProgressABbar->value();
start=clock();
applyTokiColor(colorHash);
qDebug()<<"applyTokiColor用时："<<clock()-start;

QCoreApplication::processEvents();
ui->ShowProgressABbar->setValue(lastValue+Data.sizePic[0]*Data.sizePic[1]);
start=clock();
fillMapMat(colorHash);
qDebug()<<"fillMapMat用时："<<clock()-start;

QCoreApplication::processEvents();
start=clock();
if(ui->AllowDither->isChecked())
    Dither(colorHash);
else
    AdjPro(Data.sizePic[0]*Data.sizePic[1]);
Data.adjStep=4;
qDebug()<<"抖动用时："<<clock()-start;


QCoreApplication::processEvents();
start=clock();
getAdjedPic();
Data.adjStep=5;
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
ui->isColorSpaceLab00->setEnabled(true);
ui->isColorSpaceXYZ->setEnabled(true);
ui->isColorSpaceRGBOld->setEnabled(true);
ui->AdjPicColor->setEnabled(true);
ui->ShowAdjed->setEnabled(true);

ui->AdjPicColor->setText(tr("调整颜色"));

delete colorHash;

}

#endif
