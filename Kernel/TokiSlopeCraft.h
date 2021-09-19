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

#ifndef TOKISLOPECRAFT_H
#define TOKISLOPECRAFT_H
/////////////////////////////
#define WITH_QT
/////////////////////////////

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_map>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

#include "ColorSet.h"
#include "simpleBlock.h"
#include "OptiChain.h"
#include "HeightLine.h"
#include "TokiColor.h"
#include "WaterItem.h"
#include "NBTWriter.h"
#include "PrimGlassBuilder.h"

#ifdef WITH_QT
#include <QObject>
#endif




#include <QRgb>
#include <QtConcurrent>
#include <QFuture>
//typedef unsigned char gameVersion;
typedef Array<uint,Dynamic,Dynamic> EImage;
using namespace Eigen;

#ifndef WITH_QT
/*
    #define emit ;
    #define qDebug() cerr;
    void qDebug(const char* info) {
        std::cerr<<info<<std::endl;
    }
*/
#endif

#define mapColor2Index(mapColor) (64*(mapColor%4)+(mapColor/4))
#define index2mapColor(index) (4*(index%64)+(index/64))
#define mapColor2baseColor(mapColor) (mapColor>>2)
#define index2baseColor(index) (mapColor2baseColor(index2mapColor(index)))
#define mapColor2depth(mapColor) (mapColor%4)
#define index2depth(index) (mapColor2depth(index2mapColor(index)))


#ifdef WITH_QT
class TokiSlopeCraft : public QObject
{
    Q_OBJECT
#else
    class TokiSlopeCraft
{
#endif
public:
#ifdef WITH_QT
    explicit TokiSlopeCraft(QObject *parent = nullptr);
#else
    TokiSlopeCraft();
#endif
    ~TokiSlopeCraft();

    enum gameVersion {
        ANCIENT=0,
        MC12=12,
        MC13=13,
        MC14=14,
        MC15=15,
        MC16=16,
        MC17=17,
        FUTURE=255
    };
    enum convertAlgo{
        RGB='r',
        RGB_Better='R',
        HSV='H',
        Lab94='l',
        Lab00='L',
        XYZ='X'
    };
    enum compressSettings{
        noCompress=0,NaturalOnly=1,ForcedOnly=2,Both=3
    };
    enum glassBridgeSettings{
        noBridge=0,withBridge=1
    };
    enum mapTypes{
        Slope, //立体
        Flat, //平板
        FileOnly,//纯文件
    };
    enum step{
        nothing,//对象刚刚创建，什么都没做
        colorSetReady,//颜色表读取完毕
        convertionReady,//一切就绪，等待convert
        converted,//已经将图像转化为地图画
        builded,//构建了三维结构
    };
//can do in nothing:
    void decreaseStep(step);
    bool setColorSet(const char*,const char*,const char*,const char*);
//can do in colorSetReady:
    step queryStep() const;
    bool setType(
        mapTypes,
        gameVersion,
        const bool [64],
        const simpleBlock[64] ,
        const EImage &);
    vector<string> getAuthorURL() const;
    void getARGB32(QRgb*) const;

//can do in convertionReady:
    bool convert(convertAlgo=RGB_Better,bool dither=false);
    short getImageRows() const;
    short getImageCols() const;
    bool isVanilla() const;//判断是可以生存实装的地图画
    bool isFlat() const;//判断是平板的

//can do in converted:
    bool build(compressSettings=noCompress,ushort=256,
               glassBridgeSettings=noBridge,ushort=3);//构建三维结构
    EImage getConovertedImage() const;
    vector<string> exportAsData(const string &,int) const;
    void getTokiColorPtr(ushort,const TokiColor*[]) const;
//can do in builded:
    string exportAsLitematic(const string & TargetName,//Local
                                             const string & LiteName,//Utf8
                                             const string & author,//Utf8
                                             const string & RegionName//Utf8
                             ) const;
    string exportAsStructure(const string &) const;
    void get3DSize(int & x,int & y,int & z) const;
    int getHeight() const;
    int getXRange() const;
    int getZRange() const;
    int getBlockCounts(vector<int> & ) const;
    int getBlockCounts() const;

#ifdef WITH_QT
signals:
    void progressRangeSet(int min,int max,int val) const;//设置进度条的取值范围和值
    void progressAdd(int deltaVal) const;
    void keepAwake() const;//保持主窗口唤醒

    void algoProgressRangeSet(int,int,int) const;
    void algoProgressAdd(int) const;
private slots:
#else
    void (*progressRangeSet)(int,int,int);
    void (*progressAdd)(int);
    void (*keepAwake)();
#endif

private:
    enum ColorSpace {
        R='R',H='H',L='L',X='X'
    };
    static Array<float,2,3> DitherMapLR,DitherMapRL;
    gameVersion mcVer;//12,13,14,15,16,17
    mapTypes mapType;
    step kernelStep;
    convertAlgo ConvertAlgo;
    compressSettings compressMethod;
    glassBridgeSettings glassMethod;

    ColorSet Basic;
    ColorSet Allowed;
    vector<simpleBlock> blockPalette;

    int size3D[3];//x,y,z

    EImage rawImage;
    EImage ditheredImage;

    std::unordered_map<QRgb,TokiColor> colorHash;

    ushort maxAllowedHeight;
    ushort bridgeInterval;
    PrimGlassBuilder * glassBuilder;
    ArrayXXi mapPic;//stores mapColor
    ArrayXXi Base;
    ArrayXXi HighMap;
    ArrayXXi LowMap;
    std::unordered_map<TokiPos,waterItem> WaterList;
    Tensor<unsigned char,3>Build;//x,y,z

//for setType:
//for convert:
    ColorSpace getColorSpace() const;
    void pushToHash();
    void applyTokiColor();
    void fillMapMat();
    void Dither();
    int sizePic(short) const;

//for build
    void makeHeight_old();//构建HighMap和LowMap
    void makeHeight_new();
    //void makeHeightInLine(const ushort c);
    void buildHeight();//构建Build
    void makeBridge();
//for Litematic
    void writeBlock(const string &netBlockId,
                    const vector<string>&Property,
                    const vector<string>&ProVal,
                    NBT::NBTWriter&) const;
    void writeTrash(int count,NBT::NBTWriter&) const;
    string Noder(const short *src,int size) const;

};
bool readFromTokiColor(const string & FileName,ArrayXXf & M);
bool readFromTokiColor(const char*src,ArrayXXf & M);
uchar h2d(char h);
void crash();
void matchColor(TokiColor * tColor,QRgb qColor);

#ifndef WITH_QT
void defaultProgressRangeSet(int,int,int);
void defaultProgressAdd(int);
void defaultKeepAwake();
#endif

#endif // TOKISLOPECRAFT_H
