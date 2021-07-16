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

void MainWindow::on_isColorSpaceLab_clicked()
{
    Data.adjStep=0;
    if(Data.step>=4)Data.step=4;
    else
    {updateEnables();
        return;}
    updateEnables();
    Data.Mode='L';
    qDebug("调整颜色空间为Lab");
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
/*
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
            float w_r=1.0f,w_g=2.0f,w_b=1.0f;
            auto SqrModSquare=((R*R+g*g+b*b)*(allowedColors.col(0).array().square()+allowedColors.col(1).array().square()+allowedColors.col(2).array().square())).sqrt();
            auto deltaR=(R-allowedColors.col(0).array());
            auto deltaG=(g-allowedColors.col(1).array());
            auto deltaB=(b-allowedColors.col(2).array());
            auto SigmaRGB=(R+g+b+allowedColors.col(0).array()+allowedColors.col(1).array()+allowedColors.col(2).array())/3.0f;
            auto S_r=((allowedColors.col(0).array()+R)<SigmaRGB).select((allowedColors.col(0).array()+R)/SigmaRGB,1.0f);
            auto S_g=((allowedColors.col(1).array()+g)<SigmaRGB).select((allowedColors.col(1).array()+g)/SigmaRGB,1.0f);
            auto S_b=((allowedColors.col(2).array()+b)<SigmaRGB).select((allowedColors.col(2).array()+b)/SigmaRGB,1.0f);
            auto sumRGBsquare=R*allowedColors.col(0).array()+g*allowedColors.col(1).array()+b*allowedColors.col(2).array();
            auto theta=0.6366197724f*(sumRGBsquare/SqrModSquare).acos();
            auto OnedDeltaR=deltaR.abs()/(R+allowedColors.col(0).array());
            auto OnedDeltaG=deltaG.abs()/(g+allowedColors.col(1).array());
            auto OnedDeltaB=deltaB.abs()/(b+allowedColors.col(2).array());
            auto sumOnedDelta=OnedDeltaR+OnedDeltaG+OnedDeltaB+1e-10f;
            auto S_tr=OnedDeltaR/sumOnedDelta*S_r.square();
            auto S_tg=OnedDeltaG/sumOnedDelta*S_g.square();
            auto S_tb=OnedDeltaB/sumOnedDelta*S_b.square();
            auto S_theta=S_tr+S_tg+S_tb;
            auto Rmax=(allowedColors.col(0).array()>R).select(allowedColors.col(0).array(),R);
            auto Gmax=(allowedColors.col(1).array()>g).select(allowedColors.col(1).array(),g);
            auto Bmax=(allowedColors.col(2).array()>b).select(allowedColors.col(2).array(),b);
            auto maxRGmax=(Rmax>Gmax).select(Rmax,Gmax);
            auto S_ratio=(maxRGmax>Bmax).select(maxRGmax,Bmax);

            auto dist=(S_r.square()*w_r*deltaR.square()+S_g.square()*w_g*deltaG.square()+S_b.square()*w_b*deltaB.square())/(w_r+w_g+w_b)+S_theta*S_ratio*theta.square();//+S_theta*S_ratio*theta.square()

            dist.minCoeff(&tempIndex);

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
}*/


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
ui->isColorSpaceRGB->setEnabled(false);;
ui->isColorSpaceLab->setEnabled(false);;
ui->isColorSpaceXYZ->setEnabled(false);;
ui->isColorSpaceRGBOld->setEnabled(false);;
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
ui->isColorSpaceLab->setEnabled(true);
ui->isColorSpaceXYZ->setEnabled(true);
ui->isColorSpaceRGBOld->setEnabled(true);
ui->ShowAdjed->setEnabled(true);

ui->AdjPicColor->setText(tr("调整颜色"));

}

#endif
