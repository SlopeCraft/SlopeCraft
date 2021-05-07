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


#ifndef MCMAP_H
#define MCMAP_H

#include <iostream>
#include <fstream>
#include <vector>
#include  <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include "mainwindow.h"

#include "NBTWriter.h"

#include <QString>

#include <QImage>

using namespace std;
using namespace Eigen;

class MainWindow;

class ColorSet{
    public:
        MatrixXf _RGB;
        MatrixXf  HSV;
        MatrixXf  Lab;
        MatrixXf  XYZ;
        MatrixXi  Map;
        ColorSet();
        void ApplyAllowed(ColorSet*standard,bool *MIndex);
};


class mcMap
{
public:
        mcMap();
        //mcMap(mcMap*p);
      ~mcMap();

        MainWindow*parent;

        inline short mapColor2Index(short mapColor);
        inline short Index2mapColor(short Index);

        inline bool is16();
        unsigned char gameVersion;//12,13,14,15,16,17
        char mapType;//S,C,F
        inline bool isFlat();
        inline bool isSurvival();
        inline bool isCreative();
        int step;
        ColorSet Basic;
        ColorSet Allowed;
        bool colorAllowed[256];
        QString BlockId[64][9];
        QString BlockIdfor12[64][9];
        unsigned char BlockVersion[64][9];
        inline bool canUseBlock(short r,short c);
        short SelectedBlockList[64];//实际的方块列表
        QString BlockListId[64];
        NBT::NBTWriter Lite;

        int size3D[3];//x,y,z
        int sizePic[2];
        int totalBlocks;
        //cv::Mat RawPic;

        /*调整颜色的进度：0代表尚未开始，
        1代表已经转换为浮点图像，
        2代表已经调整成对应颜色空间的图像，
        3代表已经调整为地图色，
        4代表已经生成调整后的图像*/
        bool isTransed2Float;
        short adjStep;
        QImage rawPic;
        QImage adjedPic;
        MatrixXf rawPicRGBc3[3];
        MatrixXf rawPicRHLXc3[3];

        //MatrixXf CurrentColor;

        MatrixXi mapPic;//stores mapColor
        MatrixXi Base;
        MatrixXi Depth;
        MatrixXi Height;
        Matrix<int,4,Dynamic> WaterList;

        //unsigned char***Build;

        Tensor<unsigned char,3>Build;//x,y,z
        char Mode;//R->RGB,H->HSV,L->Lab,X->XYZ

        //Colors
        //rawPic*
        //MapPic*
        //AdjedPic*
        //NBTWriter*
        //Build
        //
        short ExLitestep;
        short ExMcFstep;

        QString ProductPath;

        long makeHeight();//构建高度矩阵
        long BuildHeight();//构建真正的立体结构（三维矩阵
        long exportAsLitematica(QString FilePathAndName);
        //long exportAsMcF(QString FilePathAndName);
        long exportAsData(const QString &FolderPath,const int indexStart);

        void putCommand(const QString&Command);



        void writeBlock(const QString &netBlockId,vector<QString>&Property,vector<QString>&ProVal);

        void writeTrash(int count);

        int CommandCount;
        int NWPos[3];//x,y,z
        fstream ExMcF;
        char netFilePath[256];//纯路径，不包含最后的文件名
        char netFileName[64];//纯文件名，不含后缀名
};
bool dealBlockId(const QString&BlockId,QString&netBlockId,vector<QString>&Property,vector<QString>&ProVal);
#endif // MCMAP_H
