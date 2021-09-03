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

const ushort MainWindow::BLCreative[64]={0,0,1,1,0,0,0,0,3,0,4,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const ushort MainWindow::BLCheaper[64]={0,0,0,0,1,0,5,2,3,0,4,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const ushort MainWindow::BLBetter[64]={0,1,1,0,0,1,0,2,0,0,3,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0};
const ushort MainWindow::BLGlowing[64]={0,1,2,0,0,2,4,2,0,0,3,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,1,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0};

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

    connect(ui->isGame12,SIGNAL(clicked()),this,SLOT(onGameVerClicked()));
    connect(ui->isGame13,SIGNAL(clicked()),this,SLOT(onGameVerClicked()));
    connect(ui->isGame14,SIGNAL(clicked()),this,SLOT(onGameVerClicked()));
    connect(ui->isGame15,SIGNAL(clicked()),this,SLOT(onGameVerClicked()));
    connect(ui->isGame16,SIGNAL(clicked()),this,SLOT(onGameVerClicked()));
    connect(ui->isGame17,SIGNAL(clicked()),this,SLOT(onGameVerClicked()));

    connect(ui->isMapCreative,SIGNAL(clicked()),this,SLOT(onMapTypeClicked()));
    connect(ui->isMapSurvival,SIGNAL(clicked()),this,SLOT(onMapTypeClicked()));
    connect(ui->isMapFlat,SIGNAL(clicked()),this,SLOT(onMapTypeClicked()));

    connect(Manager,SIGNAL(switchToCustom()),this,SLOT(ChangeToCustom()));
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
}

void MainWindow::loadBlockList() {
    QString Path="./Blocks/FixedBlocks.json";
    QString Dir="./Blocks/FixedBlocks";
    while(!QFile(Path).exists()) {
        qDebug()<<"错误！找不到固定的方块列表文件"<<Path;
        Path=QFileDialog::getOpenFileName(this,
                                          "找不到固定方块列表文件，请手动寻找",
                                          "./",
                                          "FixedBlocks.json");
    }
    while(!QDir(Dir).exists()) {
        qDebug()<<"错误！固定方块列表的图标路径"<<Dir<<"无效";
        Dir=QFileDialog::getExistingDirectory(this,
                                                 "找不到存放固定方块列表图片的文件夹，请手动寻找",
                                              "./",
                                              QFileDialog::Option::ReadOnly);

    }
    QJsonDocument jd;
    QJsonParseError error;
    jd.fromJson(QFile(Path).readAll(),&error);
    if(error.error!=QJsonParseError::NoError) {
        qDebug()<<"解析固定方块列表时出错："<<error.errorString();
        return;
    }

    QJsonArray ja=jd.array();

    Manager->addBlocks(ja,Dir);



//开始解析用户自定义的方块列表
    Dir="./Blocks/CustomBlocks";
    Path="./Blocks/CustomBlocks.json";

    while(!QFile(Path).exists()) {
        int choice=QMessageBox::question(this,
                              "找不到自定义方块列表文件CustomBlocks.json",
                              "这会导致你自定义的方块无法被加载。\n你可以手动寻找它（点确认），也可以忽略这个错误（点取消）。",
                              QMessageBox::Ok|QMessageBox::Ignore);
        if(choice==QMessageBox::Ok) {
            Path=QFileDialog::getOpenFileName(this,
                                              "找不到自定义列表文件，请手动寻找",
                                              "./",
                                              "FixedBlocks.json");
        } else
            return;
    }

    while(!QDir(Dir).exists()) {
        int choice=QMessageBox::question(this,
                              "找不到存放自定义方块图片的文件夹CustomBlocks",
                              "这会导致你自定义的方块图标无法被加载。\n你可以手动寻找它（点确认），也可以忽略这个错误（点取消）。",
                              QMessageBox::Ok|QMessageBox::Ignore);
        if(choice==QMessageBox::Ok) {
            Dir=QFileDialog::getExistingDirectory(this,
                                                     "找不到存放固定方块列表图片的文件夹，请手动寻找",
                                                  "./",
                                                  QFileDialog::Option::ReadOnly);
        } else
            break;
    }

    jd.fromJson(QFile(Path).readAll(),&error);

    while(error.error!=QJsonParseError::NoError) {
        qDebug()<<"自定义方块列表json格式错误："<<error.errorString();
        QMessageBox::warning(this,
                              "解析自定义方块列表json时出错",
                              error.errorString()+"\n这是因为json格式错误。\n你自定义的方块无法被加载。",
                              QMessageBox::Abort);
        return;
    }
    ja=jd.array();

    Manager->addBlocks(ja,Dir);

    if(Kernel->queryStep()>=TokiSlopeCraft::colorSetReady) {
        QRgb colors[64];
        Kernel->getARGB32(colors);
        Manager->setLabelColors(colors);
    }
}

void MainWindow::InitializeAll()
{
    ui->LeftScroll->verticalScrollBar()->setStyleSheet("QScrollBar{width: 7px;margin: 0px 0 0px 0;background-color: rgba(255, 255, 255, 64);color: rgba(255, 255, 255, 128);}");
    if(!Collected)
    {
        loadColormap();
        qDebug("颜色表加载完毕");
        loadBlockList();
        qDebug("方块列表加载完毕");
        Manager->setVersion(TokiSlopeCraft::MC17);
        Collected=true;
    }
    static bool needInitialize=true;
    if(needInitialize)
    {
        qDebug()<<"当前运行路径："<<QCoreApplication::applicationDirPath();
        //QString DirPath=QCoreApplication::applicationDirPath()+'/';
        QDir::setCurrent(QCoreApplication::applicationDirPath());

        needInitialize=false;
#ifdef dispDerivative
    //checkBlockIds();
    makeImage(1);
#endif
    }
}

void MainWindow::contactG()
{
    static string Toki=Kernel->getAuthorURL()[1];
    QDesktopServices::openUrl(QUrl(QString::fromStdString(Toki)));
}

void MainWindow::contactB()
{
    static string Toki=Kernel->getAuthorURL()[0];
    QDesktopServices::openUrl(QUrl(QString::fromStdString(Toki)));
}

#ifndef tpSDestroyer
#define tpSDestroyer
tpS::~tpS()
{

}
#endif

void MainWindow::updateEnables() {
    bool temp=Kernel->queryStep()>=TokiSlopeCraft::colorSetReady;
    ui->StartWithFlat->setEnabled(temp);
    ui->StartWithNotVanilla->setEnabled(temp);
    ui->StartWithNotVanilla->setEnabled(temp);


    //temp=Kernel->queryStep()>=TokiSlopeCraft::convertionReady;
    ui->Convert->setEnabled(temp);

    temp=Kernel->queryStep()>=TokiSlopeCraft::converted;
    ui->ExData->setEnabled(temp);
    ui->ExLite->setEnabled(temp);
    ui->ExStructure->setEnabled(temp);
    ui->progressEx->setEnabled(temp);
    ui->progressExData->setEnabled(temp);
    ui->progressExLite->setEnabled(temp);
    ui->progressExStructure->setEnabled(temp);
    ui->Build4Lite->setEnabled(temp);
    ui->ExportData->setEnabled(temp);

    temp=Kernel->queryStep()>=TokiSlopeCraft::builded;
    ui->ExportLite->setEnabled(temp);
    ui->ManualPreview->setEnabled(temp);


}

/*
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
            out<<' '<<BlockId[r][c].toLocal8Bit().data()<<'\n';
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
            CurrentColor=qRgb(255*Basic._RGB(index,0),255*Basic._RGB(index,1),255*Basic._RGB(index,2));
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
            temp.setId(BlockId[r][c].toStdString());
            temp.setVersion(BlockVersion[r][c]);
            temp.setIdOld(BlockIdfor12[r][c].toStdString());
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
*/
void MainWindow::on_StartWithSlope_clicked() {
ui->isMapSurvival->setChecked(true);
turnToPage(1);
}

void MainWindow::on_StartWithFlat_clicked() {
    ui->isMapFlat->setChecked(true);
    turnToPage(1);
}

void MainWindow::on_StartWithNotVanilla_clicked() {
    ui->isMapFlat->setChecked(true);
    turnToPage(1);
}

void MainWindow::on_ImportPic_clicked() {
    QString Path =QFileDialog::getOpenFileName(this,
                                               tr("选择图片"),
                                               "./",
                                               tr("图片(*.png *.bmp *.jpg *.tif *.GIF )"));
        if(Path.isEmpty())return;



        if(!rawPic.load(Path))
        {
            QMessageBox::information(this,tr("打开图片失败"),tr("要不试试换一张图片吧！"));
                        return;
        }
        bool needSearch=rawPic.hasAlphaChannel();
        rawPic=rawPic.convertToFormat(QImage::Format_ARGB32);
        bool OriginHasTp=false;
        if(needSearch)
        {
            QRgb*CL=nullptr;
            for(short r=0;r<rawPic.height();r++)
            {
                CL=(QRgb*)rawPic.scanLine(r);
                for(short c=0;c<rawPic.width();c++)
                {
                    if(qAlpha(CL[c])<255)
                    {
                        r=rawPic.height()+1;
                        OriginHasTp=true;
                        break;
                    }
                }
            }
        }

        //ui->ShowRawPic->setPixmap(QPixmap::fromImage(rawPic));

        ui->IntroPicInfo->setText(tr("图片尺寸：")+QString::number(rawPic.height())+"×"+QString::number(rawPic.width())+tr("像素"));
        if(OriginHasTp)
        {
            preProcess(Strategy.pTpS,Strategy.hTpS,Strategy.BGC);
            ui->IntroPicInfo->setText(ui->IntroPicInfo->text()+"\n"+tr("图片中存在透明/半透明像素，已处理，您可以点击“设置”重新选择处理透明/半透明像素的方式。\n重新设置处理方式后，需要重新导入一次。"));
        }
        else
        {
            rawPic=rawPic.copy();
        }
        ui->ShowRawPic->setPixmap(QPixmap::fromImage(rawPic));
        ui->ShowPic->setPixmap(QPixmap::fromImage(rawPic));

        Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
        updateEnables();

        return;
}

void MainWindow::on_ImportSettings_clicked() {
    if(transSubWind!=nullptr) {
            qDebug("子窗口已经打开，不能重复打开！");
            return;
        }
        transSubWind=new tpStrategyWind(this);
        transSubWind->show();
        connect(transSubWind,SIGNAL(destroyed()),this,SLOT(destroySubWindTrans()));
        connect(transSubWind,SIGNAL(Confirm(tpS)),this,SLOT(ReceiveTPS(tpS)));
        transSubWind->setVal(Strategy);
}

void MainWindow::destroySubWindTrans() {
    disconnect(transSubWind,SIGNAL(Confirm(tpS)),this,SLOT(ReceiveTPS(tpS)));
    transSubWind=nullptr;
}

void MainWindow::ReceiveTPS(tpS t) {
    this->Strategy=t;
    qDebug("接收成功");
    qDebug()<<"pTpS="<<t.pTpS<<"；hTpS="<<t.hTpS;
}

void MainWindow::preProcess(char pureTpStrategy,
                char halfTpStrategy,
                QRgb BGC) {
    qDebug("调用了preProcess");
    //透明像素处理策略：B->替换为背景色；A->空气；W->暂缓，等待处理
    //半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色；W->暂缓，等待处理
    qDebug("Cpoied");
    bool hasTotalTrans=false;
    if(pureTpStrategy!='W'&&halfTpStrategy!='W')
    {
        QRgb*CL=nullptr;
        for(int r=0;r<rawPic.height();r++)
        {
            CL=(QRgb*)rawPic.scanLine(r);
            for(int c=0;c<rawPic.width();c++)
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

}

void MainWindow::onGameVerClicked() {
    if(ui->isGame12->isChecked()) {
        Manager->setVersion(12);

    }
    if(ui->isGame13->isChecked()) {
        Manager->setVersion(13);

    }
    if(ui->isGame14->isChecked()) {
        Manager->setVersion(14);

    }
    if(ui->isGame15->isChecked()) {
        Manager->setVersion(15);

    }
    if(ui->isGame16->isChecked()) {
        Manager->setVersion(16);

    }
    if(ui->isGame17->isChecked()) {
        Manager->setVersion(17);

    }
    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::onMapTypeClicked() {
    if(ui->isMapCreative->isChecked()) {
        Manager->setEnabled(12,true);

    }
    if(ui->isMapFlat->isChecked()) {
        Manager->setEnabled(12,true);

    }
    if(ui->isMapSurvival->isChecked()) {
        Manager->setEnabled(12,false);

    }
    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::ChangeToCustom() {
    ui->isBLCustom->setChecked(true);
    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::onPresetsClicked() {
    if(ui->isBLCreative->isChecked()) {
        Manager->applyPreset(BLCreative);
    }
    if(ui->isBLSurvivalCheaper->isChecked()) {
        Manager->applyPreset(BLCheaper);
    }
    if(ui->isBLSurvivalBetter->isChecked()) {
        Manager->applyPreset(BLBetter);
    }
    if(ui->isBLGlowing->isChecked()) {
        Manager->applyPreset(BLGlowing);
    }

    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}
