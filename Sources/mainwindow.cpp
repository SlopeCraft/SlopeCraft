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

//#pragma once
#include <QProcess>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tpstrategywind.h"
//#include "tokicolor.cpp"
#include "Collecter.cpp"
#include "Pages.cpp"

#include "mcmap.cpp"
#include "Page0.cpp"
#include "Page1.cpp"
#include "Page2.cpp"
#include "Page3.cpp"
#include "Page4.cpp"
#include "Page5.cpp"
#include "Page6.cpp"
#include "Page7.cpp"
#include "Page8.cpp"
ColorSet* TokiColor::Allowed=NULL;
ColorSet*TokiColor::Basic=NULL;
short TokiColor::DepthIndexEnd[4]={63,127,191,255};
unsigned char TokiColor::DepthCount[4]={64,64,64,64};
bool TokiColor::needFindSide=false;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug("成功setupUi");
    Collected=false;
    Data.step=0;
    connect(ui->menuStart,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage0()));
    connect(ui->menuImPic,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage1()));
    connect(ui->menuType,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage2()));
    connect(ui->menuBL,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage3()));
    connect(ui->menuAdjPic,SIGNAL(triggered(QAction*)),this,SLOT(turnToPage4()));
    connect(ui->menuExLite,SIGNAL(triggered()),this,SLOT(turnToPage5()));
    //connect(ui->menuExMcF,SIGNAL(triggered()),this,SLOT(turnToPage6()));
    connect(ui->menuExData,SIGNAL(triggered()),this,SLOT(turnToPage7()));
    connect(ui->actionAboutSlopeCraft,SIGNAL(triggered()),this,SLOT(turnToPage8()));    
    connect(ui->actionChinese,SIGNAL(triggered()),this,SLOT(turnCh()));
    connect(ui->actionEnglish,SIGNAL(triggered()),this,SLOT(turnEn()));
    qDebug("成功connect所有的菜单");

    connect(ui->NextPage,SIGNAL(clicked()),this,SLOT(turnToPage2()));
    connect(ui->NextPage2,SIGNAL(clicked()),this,SLOT(turnToPage3()));
    connect(ui->NextPage3,SIGNAL(clicked()),this,SLOT(turnToPage4()));
    connect(ui->ExLite,SIGNAL(clicked()),this,SLOT(turnToPage5()));
    //connect(ui->ExMcF,SIGNAL(clicked()),this,SLOT(turnToPage6()));
    connect(ui->ExData,SIGNAL(clicked()),this,SLOT(turnToPage7()));
    connect(ui->FinishExLite,SIGNAL(clicked()),this,SLOT(turnToPage8()));    
    connect(ui->FinshExData,SIGNAL(clicked()),this,SLOT(turnToPage8()));
    connect(ui->Exit,SIGNAL(clicked()),this,SLOT(close()));
    qDebug("成功connect所有的翻页按钮");

    connect(ui->isGame12,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame13,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame14,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame15,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame16,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame17,SIGNAL(clicked()),this,SLOT(grabGameVersion()));


    turnToPage(0);

    Data.parent=this;
    Data.ExLitestep=-1;
    Data.ExMcFstep=-1;
    //checkBlockIds();
    //QString FolderPath=QFileDialog::getExistingDirectory(this,tr("请选择导出的文件夹"));
    //qDebug()<<FolderPath;
    /*TokiColor::Allowed=&Data.Allowed;
    TokiColor::Basic=&Data.Basic;*/
    transSubWind=nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

#ifndef tpSDestroyer
#define tpSDestroyer
tpS::~tpS()
{

}
#endif


#ifdef dispDerivative
void MainWindow::checkBlockIds()
{
    Collect();
    qDebug("开始检查方块列表");
    fstream out("D:/check.mcfunction",ios::out);
    int offset[3]={-80,1,-80};
    //QString command="";
    if(out.is_open())qDebug("文件正常打开");else return;
    for(short r=0;r<64;r++)//x
        for(short c=0;c<9;c++)//z
        {
            if(Blocks[r][c]==NULL)continue;
            out<<"setblock ";
            out<<c+offset[2]<<' '<<offset[1]<<' '<<r+offset[0];
            out<<' '<<Data.BlockId[r][c].toLocal8Bit().data()<<'\n';
            //out<<command;
        }
    out.close();
}
void MainWindow::makeImage(int unitL)
{
    qDebug("开始makeImage");
    if(unitL<=0)unitL=16;
    MatrixXi PMat;
    PMat.setZero(4*unitL,60*unitL);
    int mapColor=0,index=0;
    QRgb CurrentColor;
    int cIndex;
    for(int Base=1;Base<61;Base++)
    {
        for(int depth=0;depth<4;depth++)
        {
            mapColor=4*Base+depth;
            index=mcMap::mapColor2Index(mapColor);
            CurrentColor=qRgb(255*Data.Basic._RGB(index,0),255*Data.Basic._RGB(index,1),255*Data.Basic._RGB(index,2));
            switch (depth)
            {
            case 0:
                cIndex=2;break;
            case 1:
                cIndex=1;break;
            case 2:
                cIndex=0;break;
            case 3:
                cIndex=3;break;
            }

            PMat.block(unitL*cIndex,unitL*(Base-1),unitL,unitL).array()=CurrentColor;

            //qDebug("rua!");
        }
    }
    qDebug("图像mat构建完毕");
    QRgb*CL=nullptr;

    //QSize size(PMat.rows(),PMat.cols());
    QSize size(PMat.cols(),PMat.rows());
    QImage Pic(size,QImage::Format_ARGB32);
    for(int r=0;r<PMat.rows();r++)
    {
        CL=(QRgb*)Pic.scanLine(r);
        for(int c=0;c<PMat.cols();c++)
        {
            CL[c]=PMat(r,c);
        }
    }
    Pic.save("D:\\游戏\\Minecraft\\SlopeCraft广告\\240Colors.png");
}
#endif
