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

#define bufferSize 2048

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
    qDebug("成功创建内核");
    connect(Kernel,&TokiSlopeCraft::keepAwake,this,&MainWindow::keepAwake);
    connect(Kernel,&TokiSlopeCraft::progressRangeSet,
            this,&MainWindow::progressRangeSet);
    connect(Kernel,&TokiSlopeCraft::progressAdd,
            this,&MainWindow::progressAdd);
    proTracker=nullptr;

    Manager=new BlockListManager(
                (QHBoxLayout*)ui->scrollAreaWidgetContents->layout());

    qDebug("成功创建方块列表管理者");
    connect(Manager,&BlockListManager::switchToCustom,
            this,&MainWindow::ChangeToCustom);
    //connect(Kernel,SIGNAL(convertProgressSetRange(int,int,int)));

    transSubWind=nullptr;

        connect(ui->progressStart,&QRadioButton::clicked,this,&MainWindow::turnToPage0);
        connect(ui->progressImPic,&QRadioButton::clicked,this,&MainWindow::turnToPage1);
        connect(ui->progressType,&QRadioButton::clicked,this,&MainWindow::turnToPage2);
        connect(ui->progressBL,&QRadioButton::clicked,this,&MainWindow::turnToPage3);
        connect(ui->progressAdjPic,&QRadioButton::clicked,this,&MainWindow::turnToPage4);
        connect(ui->progressExLite,&QRadioButton::clicked,this,&MainWindow::turnToPage5);
        connect(ui->progressExStructure,&QRadioButton::clicked,this,&MainWindow::turnToPage5);
        //connect(ui->menuExMcF,&QAction::trigger,this,&MainWindow::turnToPage6);
        connect(ui->progressExData,&QRadioButton::clicked,this,&MainWindow::turnToPage7);
        connect(ui->progressAbout,&QRadioButton::clicked,this,&MainWindow::turnToPage8);
        connect(ui->actionAboutSlopeCraft,&QAction::triggered,this,&MainWindow::turnToPage8);
        connect(ui->actionChinese,&QAction::triggered,this,&MainWindow::turnCh);
        connect(ui->actionEnglish,&QAction::triggered,this,&MainWindow::turnEn);
        connect(ui->progressChinese,&QRadioButton::clicked,this,&MainWindow::turnCh);
        connect(ui->progressEnglish,&QRadioButton::clicked,this,&MainWindow::turnEn);
        connect(ui->actionToki,&QAction::triggered,this,&MainWindow::contactG);
        connect(ui->actionDoki,&QAction::triggered,this,&MainWindow::contactB);
        connect(ui->progressG,&QRadioButton::clicked,this,&MainWindow::contactG);
        connect(ui->progressB,&QRadioButton::clicked,this,&MainWindow::contactB);
    qDebug("成功connect所有的菜单");


        connect(ui->NextPage,&QRadioButton::clicked,
                this,&MainWindow::turnToPage2);
        connect(ui->NextPage2,&QRadioButton::clicked,
                this,&MainWindow::turnToPage3);
        connect(ui->NextPage3,&QRadioButton::clicked,
                this,&MainWindow::turnToPage4);
        connect(ui->ExLite,&QRadioButton::clicked,
                this,&MainWindow::turnToPage5);
        //connect(ui->ExMcF,&QRadioButton::clicked,this,&MainWindow::turnToPage6);
        connect(ui->ExData,&QRadioButton::clicked,
                this,&MainWindow::turnToPage7);
        connect(ui->FinishExLite,&QRadioButton::clicked,
                this,&MainWindow::turnToPage8);
        connect(ui->FinshExData,&QRadioButton::clicked,
                this,&MainWindow::turnToPage8);
        connect(ui->Exit,&QRadioButton::clicked,
                this,&MainWindow::close);
    qDebug("成功connect所有的翻页按钮");

    connect(ui->isGame12,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame13,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame14,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame15,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame16,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);
    connect(ui->isGame17,&QRadioButton::clicked,
            this,&MainWindow::onGameVerClicked);

    connect(ui->isMapCreative,&QRadioButton::clicked,
            this,&MainWindow::onMapTypeClicked);
    connect(ui->isMapSurvival,&QRadioButton::clicked,
            this,&MainWindow::onMapTypeClicked);
    connect(ui->isMapFlat,&QRadioButton::clicked,
            this,&MainWindow::onMapTypeClicked);

    connect(ui->isBLCreative,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);
    connect(ui->isBLSurvivalCheaper,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);
    connect(ui->isBLSurvivalBetter,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);
    connect(ui->isBLGlowing,&QRadioButton::clicked,
            this,&MainWindow::onPresetsClicked);

    connect(ui->isColorSpaceRGBOld,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceRGB,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceHSV,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceLab94,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceLab00,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->isColorSpaceXYZ,&QRadioButton::clicked,
            this,&MainWindow::onAlgoClicked);
    connect(ui->AllowDither,&QCheckBox::clicked,
            this,&MainWindow::onAlgoClicked);

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

void MainWindow::keepAwake() {
    QCoreApplication::processEvents();
}

void MainWindow::loadColormap() {
    QString ColorFilePath;
    ColorFilePath="./Colors/RGB.TokiColor";

    QByteArray R,H,L,X;
    while(true) {
        QFile temp(ColorFilePath);
        if(temp.exists()) {
            temp.open(QFile::OpenModeFlag::ReadOnly);
            R=temp.readAll();
            if(QCryptographicHash::hash(R,QCryptographicHash::Algorithm::Md5).toHex()
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
            temp.open(QFile::OpenModeFlag::ReadOnly);
            H=temp.readAll();
            if(QCryptographicHash::hash(H,QCryptographicHash::Algorithm::Md5).toHex()
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
            temp.open(QFile::OpenModeFlag::ReadOnly);
            L=temp.readAll();
            if(QCryptographicHash::hash(L,QCryptographicHash::Algorithm::Md5).toHex()
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
            temp.open(QFile::OpenModeFlag::ReadOnly);
            X=temp.readAll();
            if(QCryptographicHash::hash(X,QCryptographicHash::Algorithm::Md5).toHex()
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
    QFile temp(Path);
    temp.open(QIODevice::ReadOnly | QIODevice::Text);
    jd=QJsonDocument::fromJson(temp.readAll(),&error);
    if(error.error!=QJsonParseError::NoError) {
        qDebug()<<"解析固定方块列表时出错："<<error.errorString();
        return;
    }

    QJsonArray ja=jd.object().value("FixedBlocks").toArray();

    qDebug()<<ja.size();

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
    QFile temp2(Path);
    temp2.open(QFile::OpenModeFlag::ReadOnly);
    jd=QJsonDocument::fromJson(temp2.readAll(),&error);

    while(error.error!=QJsonParseError::NoError) {
        qDebug()<<"自定义方块列表json格式错误："<<error.errorString();
        QMessageBox::warning(this,
                              "解析自定义方块列表json时出错",
                              error.errorString()+"\n这是因为json格式错误。\n你自定义的方块无法被加载。",
                              QMessageBox::Abort);
        return;
    }
    ja=jd.object().value("CustomBlocks").toArray();

    Manager->addBlocks(ja,Dir);

    if(Kernel->queryStep()>=TokiSlopeCraft::colorSetReady) {
        QRgb colors[64];
        Kernel->getARGB32(colors);
        Manager->setLabelColors(colors);
    }
    //applyPre(BLBetter);
    Manager->applyPreset(BLBetter);
}

void MainWindow::InitializeAll()
{
    ui->LeftScroll->verticalScrollBar()->setStyleSheet("QScrollBar{width: 7px;margin: 0px 0 0px 0;background-color: rgba(255, 255, 255, 64);color: rgba(255, 255, 255, 128);}");
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
    if(!Collected)
    {
        loadColormap();
        qDebug("颜色表加载完毕");
        loadBlockList();
        qDebug("方块列表加载完毕");
        Manager->setVersion(TokiSlopeCraft::MC17);
        onPresetsClicked();
        Collected=true;
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

void MainWindow::turnToPage(int page)
{
    page%=9;
    QString newtitle="SlopeCraft v3.6 Copyright © 2021 TokiNoBug    ";
    switch (page)
    {
        case 0:
                newtitle+="Step 0 / 6";
                newtitle+="    ";
                newtitle+=tr("开始");
                ui->stackedWidget->setCurrentIndex(page);
            break;

    case 1:
            newtitle+="Step 1 / 6";
            newtitle+="    ";
            newtitle+=tr("导入图片");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 2:
            newtitle+="Step 2 / 6";
            newtitle+="    ";
            newtitle+=tr("设置地图画类型");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 3:
            newtitle+="Step 3 / 6";
            newtitle+="    ";
            newtitle+=tr("设置方块列表");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 4:
            newtitle+="Step 4 / 6";
            newtitle+="    ";
            newtitle+=tr("调整颜色");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 5:
            newtitle+="Step 5 / 6";
            newtitle+="    ";
            newtitle+=tr("导出为投影文件");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 6:
            newtitle+="Step 5 / 6";
            newtitle+="    ";
            newtitle+=tr("导出为mcfunction");
            ui->stackedWidget->setCurrentIndex(page);
       break;
    case 7:
            newtitle+="Step 5 / 6";
            newtitle+="    ";
            newtitle+=tr("导出为地图文件");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    case 8:
            newtitle+="Step 6 / 6";
            newtitle+="    ";
            newtitle+=tr("结束");
            ui->stackedWidget->setCurrentIndex(page);
        break;
    default:
        qDebug("尝试翻页错误");
        break;
    }
    this->setWindowTitle(newtitle);
    updateEnables();
    return;
}

void MainWindow::turnToPage0() {
    turnToPage(0);
    return;
}

void MainWindow::turnToPage1() {
    turnToPage(1);
    return;
}

void MainWindow::turnToPage2() {
    turnToPage(2);
    return;
}

void MainWindow::turnToPage3() {
    turnToPage(3);
    return;
}

void MainWindow::turnToPage4() {
    turnToPage(4);
    return;
}

void MainWindow::turnToPage5() {
    turnToPage(5);
    return;
}

void MainWindow::turnToPage6() {
    turnToPage(6);
    return;
}

void MainWindow::turnToPage7() {
    turnToPage(7);
    return;
}

void MainWindow::turnToPage8() {
    turnToPage(8);
    return;
}

void MainWindow::updateEnables() {
    bool temp=Kernel->queryStep()>=TokiSlopeCraft::colorSetReady;
    ui->StartWithFlat->setEnabled(temp);
    ui->StartWithNotVanilla->setEnabled(temp);
    ui->StartWithNotVanilla->setEnabled(temp);


    //temp=Kernel->queryStep()>=TokiSlopeCraft::convertionReady;
    ui->Convert->setEnabled(temp);

    temp=Kernel->queryStep()>=TokiSlopeCraft::convertionReady;
    ui->ShowRaw->setEnabled(temp);

    temp=Kernel->queryStep()>=TokiSlopeCraft::converted;
    ui->ShowAdjed->setEnabled(temp);
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

    if(ui->isMapSurvival->isChecked())
        Manager->setEnabled(12,false);

    Kernel->decreaseStep(TokiSlopeCraft::colorSetReady);
    updateEnables();
}

void MainWindow::onAlgoClicked() {
    static TokiSlopeCraft::convertAlgo lastChoice=TokiSlopeCraft::convertAlgo::RGB_Better;
    static bool lastDither=false;

    TokiSlopeCraft::convertAlgo now;
    bool nowDither=ui->AllowDither->isChecked();
    if(ui->isColorSpaceRGBOld->isChecked())
        now=TokiSlopeCraft::convertAlgo::RGB;
    if(ui->isColorSpaceRGB->isChecked())
        now=TokiSlopeCraft::convertAlgo::RGB_Better;
    if(ui->isColorSpaceHSV->isChecked())
        now=TokiSlopeCraft::convertAlgo::HSV;
    if(ui->isColorSpaceLab94->isChecked())
        now=TokiSlopeCraft::convertAlgo::Lab94;
    if(ui->isColorSpaceLab00->isChecked())
        now=TokiSlopeCraft::convertAlgo::Lab00;
    if(ui->isColorSpaceXYZ->isChecked())
        now=TokiSlopeCraft::convertAlgo::XYZ;
    if(lastChoice!=now||lastDither!=nowDither)
        Kernel->decreaseStep(TokiSlopeCraft::step::convertionReady);

    updateEnables();
    lastChoice=now;
    lastDither=nowDither;
}

void MainWindow::kernelSetType() {
    /*
bool TokiSlopeCraft::setType(mapTypes type,
                             gameVersion ver,
                             const bool * allowedBaseColor,
                             simpleBlock * palettes,
                             const ArrayXXi & _rawimg)
*/
    TokiSlopeCraft::mapTypes type;
    {
    if(ui->isMapCreative->isChecked())
        type=TokiSlopeCraft::mapTypes::FileOnly;
    if(ui->isMapFlat->isChecked())
        type=TokiSlopeCraft::mapTypes::Flat;
    if(ui->isMapSurvival->isChecked())
        type=TokiSlopeCraft::mapTypes::Slope;
    }

    TokiSlopeCraft::gameVersion ver;
    {
    if(ui->isGame12->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC12;
    if(ui->isGame13->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC13;
    if(ui->isGame14->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC14;
    if(ui->isGame15->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC15;
    if(ui->isGame16->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC16;
    if(ui->isGame17->isChecked())
        ver=TokiSlopeCraft::gameVersion::MC17;
    }
    qDebug("975");
    bool allowedBaseColor[64];
    Manager->getEnableList(allowedBaseColor);
    qDebug("978");
    simpleBlock palette[64];
    Manager->getSimpleBlockList(palette);
    qDebug("981");

    /*for(uchar i=0;i<temp.size();i++){qDebug()<<i;
        palette[i].copyFrom(temp[i]);}*/
    qDebug("983");
    EImage rawImg=QImage2EImage(rawPic);
    qDebug("985");
    Kernel->setType(type,ver,allowedBaseColor,palette,rawImg);
    qDebug("987");
    updateEnables();
}

EImage QImage2EImage(const QImage & qi) {
    EImage ei;
    ei.setZero(qi.height(),qi.width());
    const QRgb * CL=nullptr;
    for(int r=0;r<ei.rows();r++) {
        CL=(const QRgb *)qi.scanLine(r);
        for(int c=0;c<ei.cols();c++)
            ei(r,c)=CL[c];
    }
    return ei;
}

QImage EImage2QImage(const EImage & ei,ushort scale) {
    QImage qi(ei.cols()*scale,ei.rows()*scale,QImage::Format::Format_ARGB32);
    QRgb * CL=nullptr;
    for(int r=0;r<qi.height();r++) {
        CL=(QRgb *)qi.scanLine(r);
        for(int c=0;c<qi.width();c++)
            CL[c]=ei(r/scale,c/scale);
    }
    return qi;
}

void MainWindow::progressRangeSet(int min,int max,int val) {//设置进度条的取值范围和值
    if(proTracker==nullptr) {
        qDebug("错误！proTracker==nullptr");
        return;
    }
    proTracker->setRange(min,max);
    proTracker->setValue(val);
}

void MainWindow::progressAdd(int deltaVal) {
    if(proTracker==nullptr) {
        qDebug("错误！proTracker==nullptr");
        return;
    }
    proTracker->setValue(deltaVal+
                                    proTracker->value());
}

void MainWindow::on_Convert_clicked() {
qDebug("开始SetType");
if(Kernel->queryStep()<TokiSlopeCraft::convertionReady) {
    qDebug("重新setType");
    kernelSetType();

}

TokiSlopeCraft::convertAlgo now;
bool nowDither=ui->AllowDither->isChecked();
{
if(ui->isColorSpaceRGBOld->isChecked())
    now=TokiSlopeCraft::convertAlgo::RGB;
if(ui->isColorSpaceRGB->isChecked())
    now=TokiSlopeCraft::convertAlgo::RGB_Better;
if(ui->isColorSpaceHSV->isChecked())
    now=TokiSlopeCraft::convertAlgo::HSV;
if(ui->isColorSpaceLab94->isChecked())
    now=TokiSlopeCraft::convertAlgo::Lab94;
if(ui->isColorSpaceLab00->isChecked())
    now=TokiSlopeCraft::convertAlgo::Lab00;
if(ui->isColorSpaceXYZ->isChecked())
    now=TokiSlopeCraft::convertAlgo::XYZ;
}

proTracker=ui->ShowProgressABbar;

Kernel->decreaseStep(TokiSlopeCraft::step::convertionReady);
updateEnables();

bool temp=false;
ui->Convert->setEnabled(temp);//防止用户在繁忙时重复操作
ui->isColorSpaceHSV->setEnabled(temp);
ui->isColorSpaceRGB->setEnabled(temp);
ui->isColorSpaceLab94->setEnabled(temp);
ui->isColorSpaceLab00->setEnabled(temp);
ui->isColorSpaceXYZ->setEnabled(temp);
ui->isColorSpaceRGBOld->setEnabled(temp);
ui->AllowDither->setEnabled(temp);
qDebug("开始convert");
Kernel->convert(now,nowDither);

proTracker=nullptr;

temp=true;
ui->Convert->setEnabled(temp);//恢复锁定
ui->isColorSpaceHSV->setEnabled(temp);
ui->isColorSpaceRGB->setEnabled(temp);
ui->isColorSpaceLab94->setEnabled(temp);
ui->isColorSpaceLab00->setEnabled(temp);
ui->isColorSpaceXYZ->setEnabled(temp);
ui->isColorSpaceRGBOld->setEnabled(temp);
ui->AllowDither->setEnabled(temp);
on_ShowAdjed_clicked();
updateEnables();
}

void MainWindow::on_ShowRaw_clicked() {
    ui->ShowPic->setPixmap(QPixmap::fromImage(rawPic));
}

void MainWindow::on_ShowAdjed_clicked() {
    ui->ShowPic->setPixmap(QPixmap::fromImage(
                               EImage2QImage(Kernel->getConovertedImage())));
}

void MainWindow::on_ExData_clicked() {
    int mapRows=ceil(Kernel->getImageRows()/128.0);
    int mapCols=ceil(Kernel->getImageCols()/128.0);
    int mapCounts=mapRows*mapCols;
    ui->ShowDataRows->setText(QString::number(mapRows));
    ui->ShowDataCols->setText(QString::number(mapCols));
    ui->ShowDataCounts->setText(QString::number(mapCounts));
    ui->InputDataIndex->setText("0");
}

//Page5

void MainWindow::on_Build4Lite_clicked() {

    bool naturalCompress=ui->AllowNaturalOpti->isChecked();
    bool forcedCompress=ui->AllowForcedOpti->isChecked();
    TokiSlopeCraft::compressSettings cS;
    if(naturalCompress) {
        if(forcedCompress)
            cS=TokiSlopeCraft::compressSettings::Both;
        else
            cS=TokiSlopeCraft::compressSettings::NaturalOnly;
    } else {
        if(forcedCompress)
            cS=TokiSlopeCraft::compressSettings::ForcedOnly;
        else cS=TokiSlopeCraft::compressSettings::noCompress;
    }
    Kernel->decreaseStep(TokiSlopeCraft::step::converted);
    ui->ExportLite->setEnabled(false);
    ui->FinishExLite->setEnabled(false);
    ui->ManualPreview->setEnabled(false);

    proTracker=ui->ShowProgressExLite;

    Kernel->build(cS,ui->maxHeight->value());

    int size3D[3],total;

    Kernel->get3DSize(size3D[0],size3D[1],size3D[2]);
    total=Kernel->getBlockCounts();
    ui->ShowLiteBlocks->setText(QString::number(total));
    ui->ShowLiteXYZ->setText(QString::fromStdString(
                                     "X:"+std::to_string(size3D[0])+
                                 "  × Y:"+std::to_string(size3D[1])+
                "  × Z:"+std::to_string(size3D[2])));
    proTracker=nullptr;
    updateEnables();
    showPreview();
}

void MainWindow::on_ManualPreview_clicked() {
    showPreview();
}

void MainWindow::on_ExportLite_clicked() {
        string FileName=QFileDialog::getSaveFileName(this,tr("导出为投影/结构方块文件"),"",tr("投影文件(*.litematic) ;; 结构方块文件(*.nbt)")).toLocal8Bit().data();
        string unCompressed;
        if(FileName.empty())return;
        bool putLitematic=(FileName.substr(FileName.length()-strlen(".litematic"))==".litematic");
        bool putStructure=(FileName.substr(FileName.length()-strlen(".nbt"))==".nbt");

        if(!putLitematic&&!putStructure)
        {
            qDebug("得到的文件路径有错！");
            return;
        }
        qDebug("开始导出投影");
        cout<<FileName<<endl;


        ui->FinishExLite->setEnabled(false);
        ui->seeExported->setEnabled(false);
        ui->Build4Lite->setEnabled(false);

        if(putStructure)
            unCompressed=Kernel->exportAsStructure(FileName);
        else
            unCompressed=Kernel->exportAsLitematic(FileName,
                                                   ui->InputLiteName->toPlainText().toUtf8().data(),
                                                   ui->InputAuthor->toPlainText().toUtf8().data(),
                                                   (ui->InputRegionName->toPlainText()+tr("(xz坐标=-65±128×整数)")).toUtf8().data());

        if(compressFile(unCompressed.data(),FileName.data()))
        {
            qDebug("压缩成功");
            QFile umComFile(QString(unCompressed.data()));
            umComFile.remove();
            ProductPath=FileName;
        }
        else
        {
            qDebug("压缩失败");
            QMessageBox::warning(this,tr("投影文件导出失败"),
                                 tr("这可能是汉字编码错误造成的。请检查路径中是否有汉字"));
            return;
        };
        updateEnables();
        qDebug("导出为投影成功");
        return;
}


void MainWindow::on_InputDataIndex_textChanged() {
    bool isIndexValid=false;
        const int indexStart=ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
        isIndexValid=isIndexValid&&(indexStart>=0);
        if(isIndexValid)
        {
            if(ceil(Kernel->getImageRows()/128.0f)==1&&
                    ceil(Kernel->getImageCols()/128.0f)==1)
            ui->ShowDataFileName->setText("map_"+QString::number(indexStart)+".dat");
            else
                ui->ShowDataFileName->setText(
                            "map_"+QString::number(indexStart)+".dat"+
                            "~"+"map_"+
                            QString::number(indexStart+ceil(Kernel->getImageRows()/128.0f)*ceil(Kernel->getImageRows()/128.0f)-1)+".dat");
            ui->ExportData->setEnabled(true);
            return;
        }

        ui->ShowDataFileName->setText(tr("你输入的起始序号不可用，请输入大于等于0的整数！"));
        ui->ExportData->setEnabled(false);
        return;
}


void MainWindow::on_ExportData_clicked() {
    bool isIndexValid=false;
        const int indexStart=ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
        isIndexValid=isIndexValid&&(indexStart>=0);
        if(!isIndexValid)
        {
            QMessageBox::information(this,tr("你输入的起始序号不可用"),tr("请输入大于等于0的整数！"));
                        return;
        }
        string FolderPath=(QFileDialog::getExistingDirectory(this,tr("请选择导出的文件夹"))).toStdString();
        if(FolderPath.empty())
        {
            QMessageBox::information(this,tr("你选择的文件夹不存在！"),tr("你可以选择存档中的data文件夹"));
            return;
        }

        ui->InputDataIndex->setEnabled(false);
        ui->ExportData->setEnabled(false);
        ui->FinshExData->setEnabled(false);
        ui->ExportData->setText(tr("请稍等"));

        for(auto it=FolderPath.begin();it!=FolderPath.end();it++)
            if(*it=='\\')*it='/';

        Kernel->exportAsData(FolderPath,indexStart);
        qDebug("导出地图文件成功");

        ui->InputDataIndex->setEnabled(true);
        ui->ExportData->setEnabled(true);
        ui->FinshExData->setEnabled(true);
        ui->ExportData->setText(tr("导出"));
        updateEnables();
}

void MainWindow::turnCh() {
    switchLan(ZH);
}

void MainWindow::turnEn() {
    switchLan(EN);
}

void MainWindow::switchLan(Language lang) {
    qDebug("开始调整语言");
    emit Manager->translate(lang);
        if(lang==EN)
        {
            if(!trans.load(":/lans/Slope_en_US.qm"))
            {
                qDebug("载入\":/lans/Slope_en_US.qm\"失败");
                return;
            }
            qApp->installTranslator(&trans);
            ui->retranslateUi(this);
            qDebug("成功调整为英语界面");
        }
        else
        {
            if(!trans.load(":/lans/Slope_zh_CN.qm"))
            {
                qDebug("载入\":/lans/Slope_zh_CN.qm\"失败");
                return;
            }
            qApp->installTranslator(&trans);
            ui->retranslateUi(this);
            qDebug("成功调整为简体中文界面");
        }
        return;
}

bool compressFile(const char*sourcePath,const char*destPath)
{
    char buf[bufferSize]={0};
    FILE*in=NULL;
    gzFile out=NULL;
    int len=0;
    fopen_s(&in,sourcePath,"rb");
    out=gzopen(destPath,"wb");
    if(in==NULL||out==NULL)
        return false;
    while(true)
    {
        len=(int)fread(buf,1,sizeof(buf),in);
        if(ferror(in))return false;
        if(len==0)break;
        if(len!=gzwrite(out,buf,(unsigned)len))
            return false;
        memset(buf,0,sizeof(buf));
    }
    fclose(in);
    gzclose(out);
    qDebug("succeed");
    return true;
}
