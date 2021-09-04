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

#include "Collecter.cpp"
#include "Pages.cpp"

#include "mcmap.cpp"

#include "Page0.cpp"
#include "Page1.cpp"
#include "Page2.cpp"
#include "Page3.cpp"
#include "Page4.cpp"
#include "Page5.cpp"
#include "Page7.cpp"
#include "Page8.cpp"

ColorSet* TokiColor::Allowed=NULL;
ColorSet*TokiColor::Basic=NULL;
short TokiColor::DepthIndexEnd[4]={63,127,191,255};
unsigned char TokiColor::DepthCount[4]={64,64,64,64};
bool TokiColor::needFindSide=false;
Matrix<float,2,3> DitherMapLR,DitherMapRL;
const short WaterColumnSize[3]={11,6,1};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug("成功setupUi");
    Collected=false;
    Data.step=0;
    connect(ui->progressStart,SIGNAL(clicked()),this,SLOT(turnToPage0()));
    connect(ui->progressImPic,SIGNAL(clicked()),this,SLOT(turnToPage1()));
    connect(ui->progressType,SIGNAL(clicked()),this,SLOT(turnToPage2()));
    connect(ui->progressBL,SIGNAL(clicked()),this,SLOT(turnToPage3()));
    connect(ui->progressAdjPic,SIGNAL(clicked()),this,SLOT(turnToPage4()));
    connect(ui->progressExLite,SIGNAL(clicked()),this,SLOT(turnToPage5()));
    connect(ui->progressExStructure,SIGNAL(clicked()),this,SLOT(turnToPage5()));
    //connect(ui->menuExMcF,SIGNAL(triggered()),this,SLOT(turnToPage6()));
    connect(ui->progressExData,SIGNAL(clicked()),this,SLOT(turnToPage7()));
    connect(ui->progressAbout,SIGNAL(clicked()),this,SLOT(turnToPage8()));
    connect(ui->actionAboutSlopeCraft,SIGNAL(triggered()),this,SLOT(turnToPage8()));    
    connect(ui->actionChinese,SIGNAL(triggered()),this,SLOT(turnCh()));
    connect(ui->actionEnglish,SIGNAL(triggered()),this,SLOT(turnEn()));
    connect(ui->progressChinese,SIGNAL(clicked()),this,SLOT(turnCh()));
    connect(ui->progressEnglish,SIGNAL(clicked()),this,SLOT(turnEn()));
    connect(ui->actionToki,SIGNAL(triggered()),this,SLOT(contactG()));
    connect(ui->actionDoki,SIGNAL(triggered()),this,SLOT(contactB()));
    connect(ui->progressG,SIGNAL(clicked()),this,SLOT(contactG()));
    connect(ui->progressB,SIGNAL(clicked()),this,SLOT(contactB()));

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

    Blocks.resize(64);
    for(int i=0;i<64;i++)Blocks[i].resize(12);
    Enables.resize(64);
    ShowColors.resize(64);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showPreview()
{
    if(Data.ExLitestep<2)return;
    if(Data.step<5)return;

    PreviewWind*preWind=new PreviewWind(this);
    preWind->Src.resize(61);
    preWind->BlockCount.resize(61);

    for(int i=0;i<61;i++)
    {
        preWind->BlockCount[i]=0;
        preWind->Src[i]=Blocks[i][Data.SelectedBlockList[i]];
    }


    for(int y=0;y<Data.size3D[1];y++)
        for(int z=0;z<Data.size3D[2];z++)
        {
            for(int x=0;x<Data.size3D[0];x++)
            {
                if(Data.Build(x,y,z)<=0)continue;
                preWind->BlockCount[Data.Build(x,y,z)-1]++;
            }
        }
    qDebug()<<"去重前有："<<preWind->Src.size()<<"个元素";
    auto iS=preWind->Src.begin();
    for(auto ib=preWind->BlockCount.begin();ib!=preWind->BlockCount.end();)
    {
        if(*ib>0)
        {
            ib++;
            iS++;
            continue;
        }
        ib=preWind->BlockCount.erase(ib);
        iS=preWind->Src.erase(iS);
    }

    preWind->size[0]=Data.size3D[0];
    preWind->size[1]=Data.size3D[1];
    preWind->size[2]=Data.size3D[2];

    qDebug()<<"去重后有："<<preWind->Src.size()<<"个元素";
    preWind->Water=Blocks[12][0];

    //preWind->Src[1]=Blocks[1][0];preWind->BlockCount[1]=1919810;

    preWind->ShowMaterialList();
    preWind->show();
}


void MainWindow::InitializeAll()
{
    ui->LeftScroll->verticalScrollBar()->setStyleSheet("QScrollBar{width: 7px;margin: 0px 0 0px 0;background-color: rgba(255, 255, 255, 64);color: rgba(255, 255, 255, 128);}");
    if(!Collected)
    {
        Collect();
        qDebug("Collected");

        Collected=true;
        qDebug("StartWithSlope中的初始化部分完成");
    }
    static bool needInitialize=true;
    if(needInitialize)
    {

        GetBLCreative(BLCreative);
        GetBLCheaper(BLCheaper);
        GetBLBetter(BLBetter);
        GetBLGlowing(BLGlowing);
        qDebug("成功初始化四个预设方块列表");
        qDebug()<<"当前运行路径："<<QCoreApplication::applicationDirPath();
        //QString DirPath=QCoreApplication::applicationDirPath()+'/';
        QDir::setCurrent(QCoreApplication::applicationDirPath());

        string ColorFilePath;
        ColorFilePath="./Colors/RGB.TokiColor";
        while(!readFromTokiColor(ColorFilePath.data(),Data.Basic._RGB,
                                 "ba56d5af2ba89d9ba3362a72778e1624"))
        {
            qDebug("未找到颜色文件RGB.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                                       QObject::tr("颜色表文件")+"RGB.TokiColor"+QObject::tr("不存在或被篡改，请手动寻找")
                                                       ,"./Colors","RGB.TokiColor").toLocal8Bit().data();
        }

        ColorFilePath="./Colors/HSV.TokiColor";
        while(!readFromTokiColor(ColorFilePath.data(),Data.Basic.HSV,
                                 "db47a74d0b32fa682d1256cce60bf574"))
        {
            qDebug("未找到颜色文件HSV.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                                       QObject::tr("颜色表文件")+"HSV.TokiColor"+QObject::tr("不存在或被篡改，请手动寻找")
                                                       ,"./Colors","HSV.TokiColor").toLocal8Bit().data();
        }

        ColorFilePath="./Colors/Lab.TokiColor";
        while(!readFromTokiColor(ColorFilePath.data(),Data.Basic.Lab,
                                 "2aec9d79b920745472c0ccf56cbb7669"))
        {
            qDebug("未找到颜色文件Lab.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                                       QObject::tr("颜色表文件")+"Lab.TokiColor"+QObject::tr("不存在或被篡改，请手动寻找")
                                                       ,"./Colors","Lab.TokiColor").toLocal8Bit().data();
        }

        ColorFilePath="./Colors/XYZ.TokiColor";
        while(!readFromTokiColor(ColorFilePath.data(),Data.Basic.XYZ,
                                 "6551171faf62961e3ae6bc3c2ee8d051"))
        {
            qDebug("未找到颜色文件XYZ.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                                       QObject::tr("颜色表文件")+"XYZ.TokiColor"+QObject::tr("不存在或被篡改，请手动寻找")
                                                       ,"./Colors","XYZ.TokiColor").toLocal8Bit().data();
        }
        qDebug("成功载入颜色");

        qDebug("导入图片按钮处的初始化部分完成");


        showColorColors();
        qDebug("成功为Colors赋予颜色");
        needInitialize=false;
#ifdef dispDerivative
    //checkBlockIds();
    makeImage(1);
#endif
    }
}


void MainWindow::contactG()
{
    static string Toki="";
    if(Toki=="")
    {
        const short size3D[]={1229, 150, 150, 44, 40, 69, 204, 204, 376, 114, 150, 1229, 598, 182, 142, 173, 110, 238, 204, 132, 110, 117, 114, 882, 110, 7, 598, 376, 204, 101, 166, 110, 44, 364, 870, 169, 922, 134, 150,};
        Toki=this->Noder(size3D,sizeof(size3D)/2);
    }
    QDesktopServices::openUrl(QUrl(QString::fromStdString(Toki)));
}

void MainWindow::contactB()
{
    static string Doki="";
    if(Doki=="")
    {
        const short sizePic[]={1229, 150, 150, 44, 40, 69, 204, 204, 40, 44, 922, 173, 364, 142, 182, 114, 166, 114, 182, 114, 166, 114, 142, 173, 110, 238, 204, 80, 218, 380, 56, 28, 286, 28, 80, 380};
        Doki=this->Noder(sizePic,sizeof(sizePic)/2);
    }
    QDesktopServices::openUrl(QUrl(QString::fromStdString(Doki)));
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
    Pic.save("D:\\240Colors.png");
}
#endif

#ifdef putBlockList
void MainWindow::putBlockListInfo() {
    if(!Collected)return;
    QString jsonDest=QFileDialog::getSaveFileName(this,
                                                  "将方块列表保存为json","","*.json");
    if(jsonDest.isEmpty())return;

    QString imgDest=QFileDialog::getExistingDirectory(this,
                                                      "选择输出图片的文件夹","");
    if(imgDest.isEmpty())return;
    imgDest.replace("\\","/");


    switchLan(false);

    queue<TokiBlock> blockQueue;
    TokiBlock temp;

    for(int r=0;r<64;r++)
        for(int c=0;c<12;c++) {
            if(Blocks[r][c]==NULL)
                continue;
            temp.setBaseColor(r);
            temp.setId(Data.BlockId[r][c].toStdString());
            temp.setVersion(Data.BlockVersion[r][c]);
            temp.setIdOld(Data.BlockIdfor12[r][c].toStdString());
            temp.setNeedGlass(NeedGlass[r][c]);
            temp.setIsGlowing(doGlow[r][c]);

            temp.btn=Blocks[r][c];
            temp.nameZH=temp.btn->text();

            blockQueue.push(temp);
        }

    switchLan(true);

    fstream jsonFile;
    jsonFile.open(jsonDest.toLocal8Bit().data(),ios::out);
    if(!jsonFile) {
        qDebug("错误！MainWindow::putBlockListInfo中文件流jsonFile打开失败");
        return;
    }
    jsonFile<<"[\n";

    while(!blockQueue.empty()) {
        blockQueue.front().nameEN=blockQueue.front().btn->text();
        QString imgPath=imgDest+"/"+QString::fromStdString(blockQueue.front().toPureBlockId())+".png";
        blockQueue.front().iconPath=blockQueue.front().toPureBlockId();
        jsonFile<<blockQueue.front().toJSON().toUtf8().data();
        jsonFile<<",\n";
        blockQueue.front().btn->icon().pixmap(QSize(16,16)).save(imgPath);
        blockQueue.pop();
    }

    jsonFile.close();
    switchLan(false);

}
#endif
