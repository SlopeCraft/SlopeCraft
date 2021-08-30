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
#ifndef MCMAP_CPP
#define MCMAP_CPP
#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include  <Eigen/Dense>

#include <unsupported/Eigen/CXX11/Tensor>

//#include "D:/OpenCV/build/include/opencv2/opencv.hpp"
#include <QTranslator>
#include <QMessageBox>
#include <QDebug>
using namespace std;
using namespace Eigen;

inline bool mcMap::canUseBlock(short r, short c)
{
    return (gameVersion>=BlockVersion[r][c]);
}

inline bool mcMap::is16()
{
    return gameVersion>=16;
}

inline bool mcMap::is17()
{
    return gameVersion>=17;
}

inline bool mcMap::isSurvival()
{
    return mapType=='S';
}

inline bool mcMap::isCreative()
{
    return mapType=='C';
}

inline bool mcMap::isFlat()
{
    return mapType=='F';
}

bool readFromFile(const char*FileName,ArrayXXf & M)
{
    fstream Reader;
    Reader.open(FileName, ios::in|ios::binary);
    if(!Reader)return false;
    float data[256][3];
    Reader.read((char*)&data[0][0],sizeof(data));
    Reader.close();
    M.setZero(256,3);
    for(short r=0;r<256;r++)
    {M(r,0)=data[r][0];
    M(r,1)=data[r][1];
    M(r,2)=data[r][2];
    }
    return true;
}


void GetBLCreative(short*BL)
{
short Crea[64]={0,0,1,1,0,0,0,0,3,0,4,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
for(short i=0;i<64;i++)
    BL[i]=Crea[i];
return;
}

void GetBLCheaper(short*BL)
{
    short Chea[64]={0,0,0,0,1,0,5,2,3,0,4,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(short i=0;i<64;i++)
        BL[i]=Chea[i];
    return;
}

void GetBLBetter(short*BL)
{
short Bett[64]={0,1,1,0,0,1,0,2,0,0,3,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0};
for(short i=0;i<64;i++)
    BL[i]=Bett[i];
return;
}

void GetBLGlowing(short*BL)
{
short Glow[64]={0,1,2,0,0,2,4,2,0,0,3,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,1,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0};

for(short i=0;i<64;i++)
    BL[i]=Glow[i];
return;
}
//using namespace cv;
mcMap::mcMap()
{
    gameVersion=16;
    mapType='S';
    step=1;//1,2,3,4,5,6,7.     1 代表进度为0
   // BLCreative[0]=0;
    Mode='L';
    size3D[0]=130;size3D[1]=50;size3D[2]=130;//x,y,z
    //isTransed2Float=false;
    adjStep=-1;

    CommandCount=0;
    NWPos[0]=-64;NWPos[1]=5;NWPos[2]=-64;

    netFilePath[0]=0;//纯路径，不包含最后的文件名
    netFileName[0]=0;//纯文件名，不含后缀名
    allowNaturalOpti=false;
    allowForcedOpti=false;
    maxHeight=-1;

    colorHash=nullptr;


    DitherMapLR<<0.0,0.0,7.0,
                             3.0,5.0,1.0;
    DitherMapRL<<7.0,0.0,0.0,
                             1.0,5.0,3.0;
    DitherMapLR/=16.0;
    DitherMapRL/=16.0;

    //Build.reshape(size3D);
    //Build.setZero();
    return;
}

mcMap::~mcMap()
{
    return;
}

inline short mcMap::mapColor2Index(short mapColor)
{
    return 64*(mapColor%4)+(mapColor/4);
}
inline short mcMap::Index2mapColor(short Index)
{
    return 4*(Index%64)+(Index/64);
}

bool dealBlockId(const QString&BlockId,
                 QString&netBlockId,
                 vector<QString>&Property,
                 vector<QString>&ProVal)
{
    Property.clear();
    ProVal.clear();
    if(!BlockId.endsWith(']'))
    {
        netBlockId=BlockId.toLower();
        return false;
    }
    short ReadBeg=BlockId.indexOf('[');
    short ReadEnd=BlockId.indexOf(']');

    if(ReadBeg<=0||ReadEnd<=0||ReadEnd<=ReadBeg)
    {
        qDebug()<<"方块Id格式出现错误:"<<BlockId;
        return false;
    }

    short ProIndex[2]={-1,-1},ProValIndex[2]={-1,-1};

    netBlockId=BlockId.mid(0,ReadBeg).toLower();

    for(short read=ReadBeg;read<=ReadEnd;read++)
    {
        switch(BlockId.at(read).toLatin1())
        {
        case '[':     //代表找到了一个新的属性
            ProIndex[0]=read+1;
            continue;

        case '=':   //识别出了属性名，寻找属性值
            ProIndex[1]=read-1;
            ProValIndex[0]=read+1;
            continue;

        case ',':   //代表结束了一个属性，并找到了下一个属性
            ProValIndex[1]=read-1;
            Property.push_back(BlockId.mid(ProIndex[0],ProIndex[1]-ProIndex[0]+1).toLower());
            ProVal.push_back(BlockId.mid(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1).toLower());
            ProIndex[0]=-1;ProIndex[1]=-1;ProValIndex[0]=-1;ProValIndex[1]=-1;

            ProIndex[0]=read+1;
            continue;
        case ']':
            ProValIndex[1]=read-1;
            Property.push_back(BlockId.mid(ProIndex[0],ProIndex[1]-ProIndex[0]+1).toLower());
            ProVal.push_back(BlockId.mid(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1).toLower());
            continue;
        }
    }
    //qDebug()<<ProVal.back()<<'='<<ProVal.back();
    return true;
}

void mcMap::writeBlock(const QString &netBlockId,vector<QString>&Property,vector<QString>&ProVal,NBT::NBTWriter&Lite)
{
    Lite.writeCompound("ThisStringShouldNeverBeSeen");
    QString BlockId;
    BlockId=netBlockId;
    if(!netBlockId.startsWith("minecraft:"))
        BlockId="minecraft:"+BlockId;

    Lite.writeString("Name",BlockId.toLocal8Bit().data());
    if(Property.empty()||ProVal.empty())
    {
        Lite.endCompound();
        return;
    }


    if(Property.size()!=ProVal.size())
    {
        qDebug("出现错误：Property和ProVal尺寸不匹配");
        return;
    }
        Lite.writeCompound("Properties");
            for(unsigned short i=0;i<ProVal.size();i++)
                Lite.writeString(Property.at(i).toLocal8Bit().data(),ProVal.at(i).toLocal8Bit().data());
        Lite.endCompound();
        //Property.clear();
        //ProVal.clear();
    Lite.endCompound();
}

void mcMap::writeTrash(int count,NBT::NBTWriter&Lite)
{
    vector<QString> ProName(5),ProVal(5);
    //ProName:NEWSP
    //,,,,
    ProName.at(0)="north";
    ProName.at(1)="east";
    ProName.at(2)="west";
    ProName.at(3)="south";
    ProName.at(4)="power";
    QString dir[3]={"none","size","up"};
    QString power[16];
    for(short i=0;i<15;i++)
        power[i]=QString::number(i);
    int written=0;
    for(short North=0;North<3;North++)
        for(short East=0;East<3;East++)
            for(short West=0;West<3;West++)
                for(short South=0;South<3;South++)
                    for(short Power=0;Power<16;Power++)
                    {
                        if(written>=count)return;
                        ProVal.at(0)=dir[North];
                        ProVal.at(1)=dir[East];
                        ProVal.at(2)=dir[West];
                        ProVal.at(3)=dir[South];
                        ProVal.at(4)=power[Power];
                        writeBlock("minecraft:redstone_wire",ProName,ProVal,Lite);
                        written++;
                    }
}

ColorSet::ColorSet()
{
    GetMap(Map);
    _RGB.setZero(256,3);
    HSV.setZero(256,3);
    Lab.setZero(256,3);
    XYZ.setZero(256,3);
}

void ColorSet::ApplyAllowed(ColorSet*standard,bool *MIndex)
{
    TokiColor::DepthCount[0]=0;
    TokiColor::DepthCount[1]=0;
    TokiColor::DepthCount[2]=0;
    TokiColor::DepthCount[3]=0;
    short totalAllowColorCount=0;
        for(short r=0;r<256;r++)
        {
            totalAllowColorCount+=MIndex[r];//if r%64==0, that is air, air isn't used as color(it's transparent)
            TokiColor::DepthCount[mcMap::Index2mapColor(r)%4]+=MIndex[r];
        }
        TokiColor::DepthIndexEnd[0]=TokiColor::DepthCount[0]-1;
        TokiColor::DepthIndexEnd[1]=TokiColor::DepthIndexEnd[0]+TokiColor::DepthCount[1];
        TokiColor::DepthIndexEnd[2]=TokiColor::DepthIndexEnd[1]+TokiColor::DepthCount[2];
        TokiColor::DepthIndexEnd[3]=TokiColor::DepthIndexEnd[2]+TokiColor::DepthCount[3];

        if(totalAllowColorCount<=1)
        {
            qDebug("你只准许了一两种颜色，巧妇难为无米之炊！");
            _RGB.setZero(1,3);
            HSV.setZero(1,3);
            Lab.setZero(1,3);
            XYZ.setZero(1,3);
            Map.setZero(1);
            return;
        }
        qDebug()<<"共允许使用"<<totalAllowColorCount<<"种颜色";

        /*for(short Index=0;Index<256;Index++)
            if(MIndex[Index])
                qDebug()<<4*(Index%64)+(Index/64);*/



        _RGB.setZero(totalAllowColorCount,3);
        HSV.setZero(totalAllowColorCount,3);
        Lab.setZero(totalAllowColorCount,3);
        XYZ.setZero(totalAllowColorCount,3);
        Map.setZero(totalAllowColorCount,1);
        short write=0;
        for(short readIndex=0;readIndex<=256;readIndex++)
        {

            if(write>=totalAllowColorCount)
                break;
            if(MIndex[readIndex])
            {
                _RGB.row(write)=standard->_RGB.row(readIndex);
                HSV.row(write)=standard->HSV.row(readIndex);
                Lab.row(write)=standard->Lab.row(readIndex);
                XYZ.row(write)=standard->XYZ.row(readIndex);
                Map(write)=standard->Map(readIndex);
                write++;
            }
        }
        //cout<<Map;
        return;
}

void GetMap(unsigned char *Map)
{
    for(short r=0;r<256;r++)Map[r]=4*(r%64)+r/64;
    return;
}

void GetMap(VectorXi &Map)
{
    Map.setZero(256);
    for(short r=0;r<256;r++)Map(r)=4*(r%64)+r/64;
    return;
}

#endif
