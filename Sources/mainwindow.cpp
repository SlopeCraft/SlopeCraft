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





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug("成功setupUi");
    Collected=false;

    Kernel=new TokiSlopeCraft(this);
    Manager=new BlockListManager(
                (QHBoxLayout*)ui->scrollArea->layout());

    //connect(Kernel,SIGNAL(convertProgressSetRange(int,int,int)));

    transSubWind=nullptr;

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
/*
    connect(ui->isGame12,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame13,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame14,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame15,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame16,SIGNAL(clicked()),this,SLOT(grabGameVersion()));
    connect(ui->isGame17,SIGNAL(clicked()),this,SLOT(grabGameVersion()));*/

    turnToPage(0);

}

MainWindow::~MainWindow()
{
    delete Kernel;
    delete Manager;
    delete ui;
}

void MainWindow::showPreview()
{
    if(Kernel->queryStep()<Kernel->builded)return;

    PreviewWind*preWind=new PreviewWind(this);
    preWind->Src.resize(62);
    preWind->BlockCount.resize(62);

    preWind->Src=Manager->getQRadioButtonList();

    Kernel->getBlockCounts(preWind->BlockCount);

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
    Kernel->get3DSize(preWind->size[0],preWind->size[1],preWind->size[2]);

    qDebug()<<"去重后有："<<preWind->Src.size()<<"个元素";
    //preWind->Water=Blocks[12][0];
    preWind->Water=Manager->getQRadioButtonList()[12];
    //preWind->Src[1]=Blocks[1][0];preWind->BlockCount[1]=1919810;

    preWind->ShowMaterialList();
    preWind->show();
}

void MainWindow::loadColormap() {

}

void MainWindow::loadBlockList() {
    QString FixedPath="./Blocks/FixedBlocks.json";
    QString FixedDir="./Blocks/FixedBlocks";
    QJsonDocument jd;
    QJsonParseError error;
    jd.fromJson(QFile(FixedPath).readAll(),&error);
    if(error.error!=QJsonParseError::NoError) {
        qDebug()<<error.errorString();
        return;
    }

    QJsonArray ja=jd.array();

    Manager->addBlocks(ja,FixedDir);

}


void MainWindow::InitializeAll()
{
    ui->LeftScroll->verticalScrollBar()->setStyleSheet("QScrollBar{width: 7px;margin: 0px 0 0px 0;background-color: rgba(255, 255, 255, 64);color: rgba(255, 255, 255, 128);}");
    if(!Collected)
    {
        loadBlockList();
        Collected=true;
        qDebug("StartWithSlope中的初始化部分完成");
    }
    static bool needInitialize=true;
    if(needInitialize)
    {
        qDebug()<<"当前运行路径："<<QCoreApplication::applicationDirPath();
        //QString DirPath=QCoreApplication::applicationDirPath()+'/';
        QDir::setCurrent(QCoreApplication::applicationDirPath());

        QString ColorFilePath;
        ColorFilePath="./Colors/RGB.TokiColor";

        QByteArray R,H,L,X;
        while(true) {
            QFile temp(ColorFilePath);
            if(temp.exists()) {
                R=temp.readAll();
                if(QCryptographicHash::hash(R,QCryptographicHash::Algorithm::Md5).toStdString()
                        =="ba56d5af2ba89d9ba3362a72778e1624") {
                    break;
                }
            }
            qDebug("未找到颜色文件RGB.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                tr("颜色表文件")+"RGB.TokiColor"+tr("不存在或被篡改，请手动寻找")
                                ,"./Colors","RGB.TokiColor");
            temp.close();
        }

        ColorFilePath="./Colors/HSV.TokiColor";
        while(true) {
            QFile temp(ColorFilePath);
            if(temp.exists()) {
                H=temp.readAll();
                if(QCryptographicHash::hash(H,QCryptographicHash::Algorithm::Md5).toStdString()
                        =="db47a74d0b32fa682d1256cce60bf574") {
                    break;
                }
            }
            qDebug("未找到颜色文件HSV.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                tr("颜色表文件")+"HSV.TokiColor"+tr("不存在或被篡改，请手动寻找")
                                ,"./Colors","HSV.TokiColor");
            temp.close();
        }

        ColorFilePath="./Colors/Lab.TokiColor";
        while(true) {
            QFile temp(ColorFilePath);
            if(temp.exists()) {
                L=temp.readAll();
                if(QCryptographicHash::hash(L,QCryptographicHash::Algorithm::Md5).toStdString()
                        =="2aec9d79b920745472c0ccf56cbb7669") {
                    break;
                }
            }
            qDebug("未找到颜色文件Lab.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                tr("颜色表文件")+"Lab.TokiColor"+tr("不存在或被篡改，请手动寻找")
                                ,"./Colors","Lab.TokiColor");
            temp.close();
        }

        ColorFilePath="./Colors/XYZ.TokiColor";
        while(true) {
            QFile temp(ColorFilePath);
            if(temp.exists()) {
                X=temp.readAll();
                if(QCryptographicHash::hash(X,QCryptographicHash::Algorithm::Md5).toStdString()
                        =="6551171faf62961e3ae6bc3c2ee8d051") {
                    break;
                }
            }
            qDebug("未找到颜色文件XYZ.TokiColor");
            ColorFilePath=QFileDialog::getOpenFileName(this,
                                tr("颜色表文件")+"XYZ.TokiColor"+tr("不存在或被篡改，请手动寻找")
                                ,"./Colors","XYZ.TokiColor");
            temp.close();
        }

        if(Kernel->setColorSet(R.data(),H.data(),L.data(),X.data()))
            qDebug("成功载入颜色");
        else
            qDebug("载入颜色失败");

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
    static string Toki="";
    if(Toki=="")
    {
        const short sizePic[]={1229, 150, 150, 44, 40, 69, 204, 204, 40, 44, 922, 173, 364, 142, 182, 114, 166, 114, 182, 114, 166, 114, 142, 173, 110, 238, 204, 80, 218, 380, 56, 28, 286, 28, 80, 380};
        Toki=this->Noder(sizePic,sizeof(sizePic)/2);
    }
    QDesktopServices::openUrl(QUrl(QString::fromStdString(Toki)));
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
