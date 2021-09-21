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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define dispDerivative
//#define putDitheredImg
//#define putMapData
//#define putBlockList
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QButtonGroup>
#include <QScrollArea>
#include <QScrollBar>
#include <QTranslator>
#include <QHash>
#include <QString>
#include <QImage>
#include <QThread>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>
#include <QtConcurrent>
#include <QFuture>
#include <QFileDialog>
#include <QCryptographicHash>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>


#include "ColorSet.h"
#include "NBTWriter.h"
#include "OptiChain.h"
#include "tpstrategywind.h"
#include "previewwind.h"
#include "WaterItem.h"
#include "zlib.h"
#include "simpleBlock.h"
#include "TokiSlopeCraft.h"
#include "BlockListManager.h"

class MainWindow;

#include <QColor>
#include <QRgb>
#include <Eigen/Dense>

//class ColorSet;

bool compressFile(const char*sourcePath,const char*destPath);

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class tpStrategyWind;

#ifndef TPS__
#define TPS__
class tpS{
public:
    tpS(char _pTpS='B',char _hTpS='C',QRgb _BGC=qRgb(220,220,220)){
            pTpS=_pTpS;
            hTpS=_hTpS;
            BGC=_BGC;    }
    ~tpS();
    char pTpS;
    char hTpS;
    QRgb BGC;
};
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //初始化方块列表用
    void InitializeAll();

#ifdef dispDerivative
    void checkBlockIds();
    void makeImage(int);
#endif

#ifdef putBlockList
    void putBlockListInfo();
#endif

public slots:
    void destroySubWindTrans();
    void preProcess(char pureTpStrategy='B',
                    char halfTpStrategy='C',
                    QRgb BGC=qRgb(220,220,220));
    void ReceiveTPS(tpS);
    //透明像素处理策略：B->替换为背景色；A->空气；W->暂缓，等待处理
    //半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色；W->暂缓，等待处理

    void showPreview();

private slots:    
    void progressRangeSet(int min,int max,int val);//设置进度条的取值范围和值
    void progressAdd(int deltaVal);
    void keepAwake();

    void showError(TokiSlopeCraft::errorFlag);
    void showWorkingStatue(TokiSlopeCraft::workStatues);

    void algoProgressRangeSet(int min,int max,int val);
    void algoProgressAdd(int deltaVal);

    void contactG();
    void contactB();
    //语言槽
    void turnCh();
    void turnEn();
    //翻页的自定义槽
    void turnToPage0();
    void turnToPage1();
    void turnToPage2();
    void turnToPage3();
    void turnToPage4();
    void turnToPage5();
    void turnToPage6();
    void turnToPage7();
    void turnToPage8();

    //for Page0
    void on_StartWithSlope_clicked();
    void on_StartWithFlat_clicked();
    void on_StartWithNotVanilla_clicked();

    //for Page1
    void on_ImportPic_clicked();
    void on_ImportSettings_clicked();

    //for Page2
    void onGameVerClicked();
    void onMapTypeClicked();

    //for Page3
    //应用预设方块列表的自定义槽
    void ChangeToCustom();
    void onPresetsClicked();
    //for Page4
    void onAlgoClicked();
    void on_Convert_clicked();
    void on_ShowRaw_clicked();
    void on_ShowAdjed_clicked();
    void on_ExData_clicked();

    //for Page5
    void on_Build4Lite_clicked();
    void on_ManualPreview_clicked();
    void on_ExportLite_clicked();
    void on_allowGlassBridge_stateChanged(int arg1);

    //for Page7
    void on_InputDataIndex_textChanged();
    void on_ExportData_clicked();


private:
    Ui::MainWindow *ui;

    TokiSlopeCraft * Kernel;
    tpStrategyWind*transSubWind;
    BlockListManager * Manager;

    QImage rawPic;

    //QTranslator translater;
    //bool Enabled[64];//被启动的方块列表，相当于最终的MIndex
    static const ushort BLCreative[64];
    static const ushort BLCheaper[64];
    static const ushort BLBetter[64];
    static const ushort BLGlowing[64];

    tpS Strategy;

    std::string ProductPath;
    QTranslator trans;
    bool Collected;
    QProgressBar * proTracker;
    //void applyPre(ushort*BL);
    void loadBlockList();
    void loadColormap();
    void turnToPage(int);
    void updateEnables();
    void switchLan(Language);
    void kernelSetType();
};

EImage QImage2EImage(const QImage &);
QImage EImage2QImage(const EImage &,ushort scale=1);
#endif // MAINWINDOW_H
