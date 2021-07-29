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

    QString Path =QFileDialog::getOpenFileName(this,tr("选择图片"),"",tr("图片(*.png *.bmp *.jpg *.tif *.GIF )"));
    if (Path.isEmpty())return;


    if(!Data.OriginPic.load(Path))
    {
        QMessageBox::information(this,tr("打开图片失败"),tr("要不试试换一张图片吧！"));
                    return;
    }
    bool needSearch=Data.OriginPic.hasAlphaChannel();
    Data.OriginPic=Data.OriginPic.convertToFormat(QImage::Format_ARGB32);
    bool OriginHasTp=false;
    if(needSearch)
    {
        QRgb*CL=nullptr;
        for(short r=0;r<Data.OriginPic.height();r++)
        {
            CL=(QRgb*)Data.OriginPic.scanLine(r);
            for(short c=0;c<Data.OriginPic.width();c++)
            {
                if(qAlpha(CL[c])<255)
                {
                    r=Data.OriginPic.height()+1;
                    OriginHasTp=true;
                    break;
                }
            }
        }
    }

    //ui->ShowRawPic->setPixmap(QPixmap::fromImage(Data.OriginPic));

    ui->IntroPicInfo->setText(tr("图片尺寸：")+QString::number(Data.OriginPic.height())+"×"+QString::number(Data.OriginPic.width())+tr("像素"));
    if(OriginHasTp)
    {
        preProcess(Strategy.pTpS,Strategy.hTpS,Strategy.BGC);
        ui->IntroPicInfo->setText(ui->IntroPicInfo->text()+"\n"+tr("图片中存在透明/半透明像素，已处理，您可以点击“设置”重新选择处理透明/半透明像素的方式。\n重新设置处理方式后，需要重新导入一次。"));
    }
    else
    {
        Data.rawPic=Data.OriginPic.copy();
    }

    //Data.isTransed2Float=false;

    //这里要将图片信息加入到Data
    Data.sizePic[0]=Data.rawPic.height();
    Data.sizePic[1]=Data.rawPic.width();

    Data.size3D[0]=Data.sizePic[1]+2;
    Data.size3D[2]=Data.sizePic[0]+2;

    Data.mapPic.setZero(Data.sizePic[0],Data.sizePic[1]);
    ui->ShowRawPic->setPixmap(QPixmap::fromImage(Data.rawPic));
    Data.adjedPic=Data.rawPic.copy();
    ui->ShowPic->setPixmap(QPixmap::fromImage(Data.adjedPic));

    Data.step=2;
    updateEnables();
    ui->NextPage->setEnabled(true);

    return;
}

void MainWindow::preProcess(char pureTpStrategy,char halfTpStrategy,QRgb BGC)
{
    qDebug("调用了preProcess");
    //透明像素处理策略：B->替换为背景色；A->空气；W->暂缓，等待处理
    //半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色；W->暂缓，等待处理
    Data.rawPic=Data.OriginPic.copy();
    qDebug("Cpoied");
    bool hasTotalTrans=false;
    if(pureTpStrategy!='W'&&halfTpStrategy!='W')
    {
        QRgb*CL=nullptr;
        for(int r=0;r<Data.rawPic.height();r++)
        {
            CL=(QRgb*)Data.rawPic.scanLine(r);
            for(int c=0;c<Data.rawPic.width();c++)
            {
                if(qAlpha(CL[c])>=255)continue;
                if(qAlpha(CL[c])==0)
                switch (pureTpStrategy) {
                case 'B':
                    CL[c]=BGC;
                    continue;
                case 'A':
                    if(!hasTotalTrans)
                    {qDebug()<<"发现纯透明像素";
                    hasTotalTrans=true;}
                    CL[c]=qRgba(0,0,0,0);
                    continue;
                }

                //qDebug("neeeee");
                switch (halfTpStrategy) {
                case 'B':
                    CL[c]=BGC;
                    break;
                case 'C':
                    CL[c]=ComposeColor(CL[c],BGC);
                    break;
                case 'R':
                    CL[c]=qRgb(qRed(CL[c]),qGreen(CL[c]),qBlue(CL[c]));
                }
            }
        }
    }

    //qDebug()<<"图片预处理完毕";
}

void MainWindow::on_ImportSettings_clicked()
{
    if(transSubWind!=nullptr)
    {
        qDebug("子窗口已经打开，不能重复打开！");
        return;
    }
    transSubWind=new tpStrategyWind(this);
    transSubWind->show();
    connect(transSubWind,SIGNAL(destroyed()),this,SLOT(destroySubWindTrans()));
    connect(transSubWind,SIGNAL(Confirm(tpS)),this,SLOT(ReceiveTPS(tpS)));
    transSubWind->setVal(Strategy);
}

void MainWindow::destroySubWindTrans()
{
    disconnect(transSubWind,SIGNAL(Confirm(tpS)),this,SLOT(ReceiveTPS(tpS)));
    transSubWind=nullptr;
}

void MainWindow::ReceiveTPS(tpS t)
{
    this->Strategy=t;
    qDebug("接收成功");
    qDebug()<<"pTpS="<<t.pTpS<<"；hTpS="<<t.hTpS;
}

inline QRgb ComposeColor(const QRgb&front,const QRgb&back)
{
    int red=(qRed(front)*qAlpha(front)+qRed(back)*(255-qAlpha(front)))/255;
    int green=(qGreen(front)*qAlpha(front)+qGreen(back)*(255-qAlpha(front)))/255;
    int blue=(qBlue(front)*qAlpha(front)+qBlue(back)*(255-qAlpha(front)))/255;
    return qRgb(red,green,blue);
}

#endif
