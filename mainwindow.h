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


#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "mcmap.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QButtonGroup>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend mcMap;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    mcMap Data;
    void turnToPage(int);

    QRadioButton *Blocks[64][9];
    QCheckBox *Enables[64];
    QLabel *ShowColors[64];
    //bool Enabled[64];//被启动的方块列表，相当于最终的MIndex
    bool NeedGlass[64][9];
    bool doGlow[64][9];
    //QString BlockId[64][9];
    short BLCreative[64];
    short BLCheaper[64];
    short BLBetter[64];
    short BLGlowing[64];

    //初始化方块列表用
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
    void transQim2Float();//for step1
    void transPic2RGB();//for step2
    void transPic2HSV();
    void transPic2Lab();
    void transPic2XYZ();
    void Pic2Map(MatrixXf&);//for step3//old method
    void Pic2Map4RGB(MatrixXf&);
    void Pic2Map4HSV(MatrixXf&);
    void Pic2Map4Lab(MatrixXf&);
    //void Pic2Map4XYZ(MatrixXf&);

    void getAdjedPic();//for step4

    void checkBlockIds();


    void updateEnables();



//////////////////////////////////////////////////////////////////////////////////
private slots:

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

    void on_isColorSpaceLab_clicked();

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

private:
    Ui::MainWindow *ui;
};






#endif // MAINWINDOW_H
