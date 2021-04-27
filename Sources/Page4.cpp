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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mcmap.h"
//#include "ColorAdjust.cpp"

#include <QColorSpace>
#include <QColorTransform>
#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <time.h>

using namespace Eigen;

void MainWindow::on_isColorSpaceXYZ_clicked()
{
    Data.adjStep=0;
    Data.Mode='X';
    qDebug("调整颜色空间为XYZ");
}

void MainWindow::on_isColorSpaceLab_clicked()
{
    Data.adjStep=0;
    Data.Mode='L';
    qDebug("调整颜色空间为Lab");
}

void MainWindow::on_isColorSpaceHSV_clicked()
{
    Data.adjStep=0;
    Data.Mode='H';
    qDebug("调整颜色空间为HSV");
}

void MainWindow::on_isColorSpaceRGB_clicked()
{
    Data.adjStep=0;
    Data.Mode='R';
    qDebug("调整颜色空间为RGB");
}

void MainWindow::on_isColorSpaceRGBOld_clicked()
{
    Data.adjStep=0;
    Data.Mode='r';
    qDebug("调整颜色空间为旧版RGB");
}

void f(float &I)
{
    if (I>0.008856f)
    I=std::pow(I,1.0f/3.0f);
    else
    I=7.787f*I+16.0f/116.0f;
    return;
}

void RGB2HSV(float r, float g, float b,  float &h, float &s, float &v)
{
    float K = 0.0f;
    if (g < b)
    {
        std::swap(g, b);
        K = -1.0f;
    }
    if (r < g)
    {
        std::swap(r, g);
        K = -2.0f / 6.0f - K;
    }
    float chroma = r - std::min(g, b);
    h = fabs(K + (g - b) / (6.0f * chroma + 1e-20f));
    s = chroma / (r + 1e-20f);
    v = r;

    return;
}

void RGB2XYZ(float R, float G, float B, float &X, float &Y, float &Z)
{
    X = 0.412453f * R + 0.357580f * G + 0.180423f * B;
    Y = 0.212671f * R + 0.715160f * G + 0.072169f * B;
    Z = 0.019334f * R + 0.119193f * G + 0.950227f * B;
    return;
}

void XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b)
{
    X/=0.9504f;f(X);
    Y/=1.0f;   f(Y);
    Z/=1.0888f;f(Z);
    L=116.0f*X-16.0f;
    a=500.0f*(X-Y);
    b=200.0f*(Y-Z);
    return;
}

void MainWindow::transQim2Float()
{
    if(Data.adjStep<0)return;
    if(Data.isTransed2Float)return;

    QRgb *CurrentLine;
    QRgb CurrentColor;
    float multiper=1.0f/255.0f;
    //float r,g,b;

    for(short r=0;r<Data.sizePic[0];r++)
    {
        CurrentLine=(QRgb*)Data.rawPic.scanLine(r);
        for(short c=0;c<Data.sizePic[1];c++)
        {
            CurrentColor=CurrentLine[c];
            Data.rawPicRGBc3[0](r,c)=multiper*qRed(CurrentColor);
            Data.rawPicRGBc3[1](r,c)=multiper*qGreen(CurrentColor);
            Data.rawPicRGBc3[2](r,c)=multiper*qBlue(CurrentColor);
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
        //qDebug("rua!");
    }
    //qDebug("成功将图像转变为浮点图像");
    Data.adjStep=1;
}

void MainWindow::transPic2RGB()
{
    if(!(Data.Mode=='R'||Data.Mode=='r'))
        return;
    if(Data.adjStep<1)return;
    Data.rawPicRHLXc3[0]<<Data.rawPicRGBc3[0];
    Data.rawPicRHLXc3[1]<<Data.rawPicRGBc3[1];
    Data.rawPicRHLXc3[2]<<Data.rawPicRGBc3[2];
    ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+Data.sizePic[0]);
   // qDebug("成功将浮点图片转换为RGB");
    Data.adjStep=2;
}

void MainWindow::transPic2HSV()
{
    if(Data.Mode!='H')
        return;
    if(Data.adjStep<1)return;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            RGB2HSV(Data.rawPicRGBc3[0](r,c),Data.rawPicRGBc3[1](r,c),Data.rawPicRGBc3[2](r,c),Data.rawPicRHLXc3[0](r,c),Data.rawPicRHLXc3[1](r,c),Data.rawPicRHLXc3[2](r,c));
            //Data.rawPicRHLXc3[0](r,c)
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
    }
    //qDebug("成功将浮点图片转换为HSV");
    Data.adjStep=2;
}

void MainWindow::transPic2XYZ()
{
    if(Data.Mode!='X')
        return;
    if(Data.adjStep<1)return;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            RGB2XYZ(Data.rawPicRGBc3[0](r,c),Data.rawPicRGBc3[1](r,c),Data.rawPicRGBc3[2](r,c),Data.rawPicRHLXc3[0](r,c),Data.rawPicRHLXc3[1](r,c),Data.rawPicRHLXc3[2](r,c));
            //Data.rawPicRHLXc3[0](r,c)
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
    }
    //qDebug("成功将浮点图片转换为XYZ");
    Data.adjStep=2;
}

void MainWindow::transPic2Lab()
{
    if(Data.Mode!='L')
        return;
    if(Data.adjStep<1)return;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            RGB2XYZ(Data.rawPicRGBc3[0](r,c),Data.rawPicRGBc3[1](r,c),Data.rawPicRGBc3[2](r,c),Data.rawPicRHLXc3[0](r,c),Data.rawPicRHLXc3[1](r,c),Data.rawPicRHLXc3[2](r,c));
            XYZ2Lab(Data.rawPicRHLXc3[0](r,c),Data.rawPicRHLXc3[1](r,c),Data.rawPicRHLXc3[2](r,c),Data.rawPicRHLXc3[0](r,c),Data.rawPicRHLXc3[1](r,c),Data.rawPicRHLXc3[2](r,c));
            //Data.rawPicRHLXc3[0](r,c)
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
    }
    //qDebug("成功将浮点图片转换为Lab");
    Data.adjStep=2;
}

void MainWindow::Pic2Map(MatrixXf&allowedColors)
{

    if(Data.adjStep<2)return;
    int tempIndex=0;
     //VectorXf Diff;
    //Diff.setZero(allowedColors.rows());
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            float c0=Data.rawPicRHLXc3[0](r,c);
            float c1=Data.rawPicRHLXc3[1](r,c);
            float c2=Data.rawPicRHLXc3[2](r,c);

            auto Diff0_2=(allowedColors.col(0).array()-c0).square();
            auto Diff1_2=(allowedColors.col(1).array()-c1).square();
            auto Diff2_2=(allowedColors.col(2).array()-c2).square();

            auto Diff=Diff0_2+Diff1_2+Diff2_2;
            //Data.CurrentColor-=allowedColors;

            Diff.minCoeff(&tempIndex);
            //Diff.minCoeff(tempIndex,u);


            Data.mapPic(r,c)=Data.Allowed.Map(tempIndex,0);
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
    }
    Data.adjStep=3;
    //qDebug("成功生成mapPic");
    return;
}

void MainWindow::Pic2Map4RGB(MatrixXf &allowedColors)
{
    if(Data.adjStep<2)return;
    int tempIndex=0;
     //VectorXf Diff;
    //Diff.setZero(allowedColors.rows());
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            float R=Data.rawPicRHLXc3[0](r,c);
            float g=Data.rawPicRHLXc3[1](r,c);
            float b=Data.rawPicRHLXc3[2](r,c);

            auto tao=(R+allowedColors.col(0).array())/2;
            auto deltaRsquare=(R-allowedColors.col(0).array()).square();
            auto deltaGsquare=(g-allowedColors.col(1).array()).square();
            auto deltaBsquare=(b-allowedColors.col(2).array()).square();

            auto Diff=(tao*0.99609375f+2)*deltaRsquare+4*deltaGsquare+((1.0f-tao)/1.003922f)*deltaBsquare;
            Diff.minCoeff(&tempIndex);
            //((Data.CurrentColor-allowedColors).rowwise().squaredNorm()).minCoeff(&tempIndex);




            Data.mapPic(r,c)=Data.Allowed.Map(tempIndex,0);
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
    }
    Data.adjStep=3;
    //qDebug("成功生成mapPic");
    return;
}

void MainWindow::Pic2Map4HSV(MatrixXf &allowedColors)
{

    //Pic2Map(allowedColors);
    //return;

    if(Data.adjStep<2)return;
    int tempIndex=0;
     //VectorXf Diff;
    //Diff.setZero(allowedColors.rows());
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            float h=Data.rawPicRHLXc3[0](r,c);
            float s=Data.rawPicRHLXc3[1](r,c);
            float v=Data.rawPicRHLXc3[2](r,c);


            auto deltaX=50.0f*allowedColors.col(1).array()*allowedColors.col(2).array()*((6.283185f*allowedColors.col(0).array()).cos())-50.0f*v*s*cos(6.283185f*h);
            auto deltaY=50.0f*allowedColors.col(1).array()*allowedColors.col(2).array()*((6.283185f*allowedColors.col(0).array()).sin())-50.0f*v*s*sin(6.283185f*h);
            auto deltaZ=86.60254f*(1.0f-allowedColors.col(2).array())-86.60254f*(1.0f-v);
            auto Diff=deltaX.square()+deltaY.square()+deltaZ.square();
            Diff.minCoeff(&tempIndex);
            Data.mapPic(r,c)=Data.Allowed.Map(tempIndex,0);
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
    }
    Data.adjStep=3;
    //qDebug("成功生成mapPic");
    return;
}

void MainWindow::Pic2Map4Lab(MatrixXf &allowedColors)
{
    if(Data.adjStep<2)return;
    int tempIndex=0;
     //VectorXf Diff;
    //Diff.setZero(allowedColors.rows());
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            float L=Data.rawPicRHLXc3[0](r,c);
            float a=Data.rawPicRHLXc3[1](r,c);
            float b=Data.rawPicRHLXc3[2](r,c);

            auto deltaL_2=(allowedColors.col(0).array()-L).square();
            float C1_2=a*a+b*b;
            auto C2_2=allowedColors.col(1).array().square()+allowedColors.col(2).array().square();
            auto deltaCab_2=(sqrt(C1_2)-C2_2.array().sqrt()).square();
            auto deltaHab_2=(allowedColors.col(1).array()-a).square()+(allowedColors.col(2).array()-b).square()-deltaCab_2;

            //SL=1,kL=1
            //K1=0.045f
            //K2=0.015f

            float SC_2=(sqrt(C1_2)*0.045f+1.0f)*(sqrt(C1_2)*0.045f+1.0f);
            auto SH_2=(C2_2.sqrt()*0.015f+1.0f).square();

            auto Diff=deltaL_2+deltaCab_2/SC_2+deltaHab_2/SH_2;


            Diff.minCoeff(&tempIndex);
            Data.mapPic(r,c)=Data.Allowed.Map(tempIndex,0);
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
    }
    Data.adjStep=3;
    //qDebug("成功生成mapPic");
    return;
}


void MainWindow::getAdjedPic()
{
    if(Data.adjStep<3)return;
    MatrixXi RGBint=(255*Data.Basic._RGB).cast<int>();
    short Index;
    for(short r=0;r<Data.sizePic[0];r++)
    {
        for(short c=0;c<Data.sizePic[1];c++)
        {
            Index=Data.mapColor2Index(Data.mapPic(r,c));


            Data.adjedPic.setPixel(c,r,qRgb(RGBint(Index,0),RGBint(Index,1),RGBint(Index,2)));
        }
        ui->ShowProgressABbar->setValue(ui->ShowProgressABbar->value()+1);
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
ui->ExMcF->setEnabled(false);

ui->isColorSpaceHSV->setEnabled(false);
ui->isColorSpaceRGB->setEnabled(false);;
ui->isColorSpaceLab->setEnabled(false);;
ui->isColorSpaceXYZ->setEnabled(false);;
ui->isColorSpaceRGBOld->setEnabled(false);;
ui->ShowAdjed->setEnabled(false);

ui->AdjPicColor->setText("请稍等");

Data.adjStep=0;
ui->ShowProgressABbar->setRange(0,4*Data.sizePic[0]+1);
//第一步，转为浮点图像(r)；第二步，调整颜色空间(r)；第三步，调整为地图色(r)；第四步，生成调整后图片(r)，显示(1)
ui->ShowProgressABbar->setValue(0);

//Data.CurrentColor.setZero(Data.Allowed._RGB.rows(),3);


clock_t start;
start=clock();
//t=GetCycleCount();


if(Data.isTransed2Float)
    ui->ShowProgressABbar->setValue(Data.sizePic[0]);
else
{
    transQim2Float();
    Data.isTransed2Float=true;
}
                    qDebug()<<"转换为浮点图像用时："<<clock()-start;
                    start=clock();
MatrixXf*allowedColor;
Data.adjStep=1;
//qDebug("开始调整颜色空间");
switch (Data.Mode)
{
case 'r':
    transPic2RGB();
    allowedColor=&(Data.Allowed._RGB);
    break;
case 'R':
    transPic2RGB();
    allowedColor=&(Data.Allowed._RGB);
    break;
case 'H':
    transPic2HSV();
    allowedColor=&(Data.Allowed.HSV);
    break;
case 'X':
    transPic2XYZ();
    allowedColor=&(Data.Allowed.XYZ);
    break;
default:
    transPic2Lab();
    allowedColor=&(Data.Allowed.Lab);
    break;
}

                    qDebug()<<"调整颜色空间用时："<<clock()-start;
                    start=clock();
Data.adjStep=2;
//qDebug("开始调整为地图色");

switch (Data.Mode)
{
case 'r':
    Pic2Map(*allowedColor);
    break;
case 'R':
    Pic2Map4RGB(*allowedColor);//RGB优化
    break;
case 'L':
    Pic2Map4Lab(*allowedColor);
case 'H':
    Pic2Map4HSV(*allowedColor);
default:
    Pic2Map(*allowedColor);
    break;
}
qDebug()<<"调整为地图色用时："<<clock()-start;
start=clock();

Data.adjStep=3;
//qDebug("开始生成调整后图像");
getAdjedPic();
Data.adjStep=4;
emit on_ShowAdjed_clicked();

qDebug()<<"生成调整后图像用时："<<clock()-start;
//start=clock();

ui->ShowProgressABbar->setValue(4*Data.sizePic[0]+1);

Data.step=5;
Data.ExLitestep=0;
Data.ExMcFstep=0;
//qDebug("已显示调整后图像并允许翻页");
ui->ExLite->setEnabled(true);
//ui->ExMcF->setEnabled(true);

ui->isColorSpaceHSV->setEnabled(true);
ui->isColorSpaceRGB->setEnabled(true);
ui->isColorSpaceLab->setEnabled(true);
ui->isColorSpaceXYZ->setEnabled(true);
ui->isColorSpaceRGBOld->setEnabled(true);
ui->ShowAdjed->setEnabled(true);

ui->AdjPicColor->setText("调整颜色");

}

#endif
