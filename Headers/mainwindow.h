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
using namespace std;
using namespace Eigen;

class MainWindow;

#include <QColor>
#include <QRgb>
#include <Eigen/Dense>

using namespace Eigen;
//class ColorSet;
void f(float &);
void invf(float&);
void RGB2HSV(float, float, float,  float &, float &, float &);
void HSV2RGB(float,float,float,float&,float&,float&);
void RGB2XYZ(float R, float G, float B, float &X, float &Y, float &Z);
void XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b);
void Lab2XYZ(float L,float a,float b,float&X,float&Y,float&Z);

float Lab00(float,float,float,float,float,float);

float squeeze01(float);
QRgb RGB2QRGB(float,float,float);
QRgb HSV2QRGB(float,float,float);
QRgb XYZ2QRGB(float,float,float);
QRgb Lab2QRGB(float,float,float);
bool dealBlockId(const QString&BlockId,
                 QString&netBlockId,
                 vector<QString>&Property,
                 vector<QString>&ProVal);
QRgb ComposeColor(const QRgb&front,const QRgb&back);
bool readFromFile(const char*FileName,ArrayXXf & M);
bool readFromTokiColor(const char*,ArrayXXf &,const string &);
uchar h2d(char);


void GetBLCreative(short*BL);
void GetBLCheaper(short*BL);
void GetBLBetter(short*BL);
void GetBLGlowing(short*BL);
bool compressFile(const char*sourcePath,const char*destPath);
void matchColor(TokiColor * tColor,QRgb qColor);


extern Matrix<float,2,3> DitherMapLR,DitherMapRL;
extern const short WaterColumnSize[3];

class mcMap
{
public:
        mcMap();
      ~mcMap();

        MainWindow*parent;
        unsigned char gameVersion;//12,13,14,15,16,17
        char mapType;//S,C,F
        int step;
        ColorSet Basic;
        ColorSet Allowed;
        bool colorAllowed[256];

        vector<vector<simpleBlock>> FullBlockList;
        short SelectedBlockList[64];//实际的方块列表
        vector<string*> BlockListId;

        int size3D[3];//x,y,z
        int sizePic[2];
        int totalBlocks;

        short adjStep;
        QImage OriginPic;
        QImage rawPic;
        QImage adjedPic;

        MatrixXf Dither[3];
        //std::unordered_map<QRgb,TokiColor>*colorHash;


        MatrixXi mapPic;//stores mapColor

        ArrayXXi Base;
        ArrayXXi HighMap;
        ArrayXXi LowMap;
        std::unordered_map<TokiPos,waterItem> WaterList;

        Tensor<unsigned char,3>Build;//x,y,z
        char Mode;//R->RGB,H->HSV,L->Lab,X->XYZ

        short ExLitestep;
        short ExMcFstep;

        bool allowNaturalOpti;
        bool allowForcedOpti;
        int maxHeight;

        //char netFilePath[256];//纯路径，不包含最后的文件名
        //char netFileName[64];//纯文件名，不含后缀名



#ifdef putMapData
        void putMap(const QString &,const MatrixXi&HighMap,const MatrixXi&LowMap);
#endif


        static inline short mapColor2Index(short mapColor);
        static inline short Index2mapColor(short Index);

        long makeHeight();//构建高度矩阵

        inline bool is16();
        inline bool is17();
        inline bool isFlat();
        inline bool isSurvival();
        inline bool isCreative();

        inline bool canUseBlock(short r,short c);
        long BuildHeight();//构建真正的立体结构（三维矩阵
        long exportAsLitematica(string FilePathAndName);
        long exportAsStructure(string FilePathAndName);
        long exportAsData(const string &FolderPath,const int indexStart);

        //void putCommand(const QString&Command);

        void writeBlock(const string &netBlockId,vector<string>&Property,vector<string>&ProVal,NBT::NBTWriter&);

        void writeTrash(int count,NBT::NBTWriter&);

        //int CommandCount;
};

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
    friend mcMap;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    mcMap Data;
    tpStrategyWind*transSubWind;
    //QTranslator translater;
    vector<vector<QRadioButton*>>Blocks;
    vector<QCheckBox*>Enables;
    vector<QLabel*>ShowColors;
    //bool Enabled[64];//被启动的方块列表，相当于最终的MIndex
    bool NeedGlass[64][12];
    bool doGlow[64][12];
    short BLCreative[64];
    short BLCheaper[64];
    short BLBetter[64];
    short BLGlowing[64];

    tpS Strategy;

    string ProductPath;

    //初始化方块列表用

    void InitializeAll();

    void Collect();//done
    bool Collected;
    void IniNeedGlass();//done
    void InidoGlow();//done
    void IniBL();//done
    void IniEnables();
    //void setShowColors();
    void applyPre(short*BL);//done
    void connectEnables();//done
    void showColorColors();

    //其他非初始化用途
    void allowUpdateToCustom(bool allowAutoUpdate);//done
    //void getBlockIds();
    void getMIndex();
    void getBlockList();

    //调整颜色,Page4
    void pushToHash(unordered_map<QRgb,TokiColor>*);
    void applyTokiColor(unordered_map<QRgb,TokiColor>*);
    void fillMapMat(unordered_map<QRgb,TokiColor>*);
    void Dither(unordered_map<QRgb,TokiColor>*);//抖动，将三个Dither矩阵填充为抖动后的三通道值
    void getAdjedPic();//for step5

    void turnToPage(int);

    void updateEnables();

    void switchLan(bool);

#ifdef dispDerivative
    void checkBlockIds();
    void makeImage(int);
#endif

#ifdef putBlockList
    void putBlockListInfo();
#endif

private:
    QTranslator trans;
    string Noder(const short *src,int size);

public slots:
    void AdjPro(int step=1);
    void destroySubWindTrans();
    void preProcess(char pureTpStrategy='B',char halfTpStrategy='C',QRgb BGC=qRgb(220,220,220));
    void ReceiveTPS(tpS);
    //透明像素处理策略：B->替换为背景色；A->空气；W->暂缓，等待处理
    //半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色；W->暂缓，等待处理

    void showPreview();

private slots:
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

    void grabGameVersion();

    void versionCheck();

    //forPage1
    void on_ImportPic_clicked();

    //forPage2

    //forPage3
    //应用预设方块列表的自定义槽
    void ChangeToCustom();
    void applyVanilla();
    void applyCheap();
    void applyElegant();
    void applyShiny();
    //forPage4

    //forPage5



    void on_StartWithSlope_clicked();

    void on_StartWithFlat_clicked();

    void on_StartWithNotVanilla_clicked();

    void on_confirmType_clicked();

    void on_confirmBL_clicked();

    void on_FirstConcrete_clicked();

    void on_FirstWool_clicked();

    void on_FirstStainedGlass_clicked();

    void on_isColorSpaceXYZ_clicked();

    void on_isColorSpaceLab94_clicked();

    void on_isColorSpaceLab00_clicked();

    void on_isColorSpaceHSV_clicked();

    void on_isColorSpaceRGB_clicked();

    void on_AdjPicColor_clicked();

    void on_ShowRaw_clicked();

    void on_ShowAdjed_clicked();

    void on_isColorSpaceRGBOld_clicked();

    void on_Build4Lite_clicked();
    void on_ExportLite_clicked();

    void on_seeExported_clicked();

    void on_isMapFlat_clicked();

    void on_isMapSurvival_clicked();

    void on_isMapCreative_clicked();

    void on_ExportData_clicked();

    void on_InputDataIndex_textChanged();

    void on_ExImage_clicked();

    void on_AllowNaturalOpti_stateChanged(int arg1);

    void on_ImportSettings_clicked();

    void on_AllowNaturalOpti_clicked(bool checked);

    void on_AllowForcedOpti_stateChanged(int arg1);

    void on_ManualPreview_clicked();

    void on_contact_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
