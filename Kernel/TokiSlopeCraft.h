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

/////////////////////////////

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_map>

#include "defines.h"
#include <unsupported/Eigen/CXX11/Tensor>
#include "ColorSet.h"
#include "simpleBlock.h"
#include "TokiColor.h"
#include "WaterItem.h"

#include "PrimGlassBuilder.h"
#include "lossyCompressor.h"
#include "NBTWriter.h"

using namespace SlopeCraft;
#ifdef WITH_QT
#include <QObject>
#include <QtConcurrent>
#include <QFuture>
#else
#include <thread>
#endif

#define mapColor2Index(mapColor) (64*(mapColor%4)+(mapColor/4))
#define index2mapColor(index) (4*(index%64)+(index/64))
#define mapColor2baseColor(mapColor) (mapColor>>2)
#define index2baseColor(index) (mapColor2baseColor(index2mapColor(index)))
#define mapColor2depth(mapColor) (mapColor%4)
#define index2depth(index) (mapColor2depth(index2mapColor(index)))

class PrimGlassBuilder;
class LossyCompressor;

namespace NBT {
    class NBTWriter;
};

class TokiSlopeCraft : public Kernel
{
#ifdef WITH_QT
    Q_OBJECT
public:
    explicit TokiSlopeCraft(QObject *parent = nullptr);
#else
public:
    TokiSlopeCraft();
#endif
    virtual ~TokiSlopeCraft();

//can do in nothing:
    unsigned long long size() {
        return sizeof(TokiSlopeCraft);
    }
    void destroy() {
        delete this;
    }
    void decreaseStep(step);
    bool setColorSet(const char*,const char*,const char*,const char*);
    void makeTests(const AbstractBlock **,const unsigned char *,const char *,char*);
    std::string makeTests(const AbstractBlock **,const uint8_t *,const std::string & );
//can do in colorSetReady:
    step queryStep() const;

    bool setType(mapTypes,
                 gameVersion,
                 const bool [64],
                 const AbstractBlock * [64]);
    bool setType(
        mapTypes,
        gameVersion,
        const bool [64],
        const simpleBlock [64]);

    void getAuthorURL(int * count,char** dest) const;
    std::vector<std::string> getAuthorURL() const;
    void getARGB32(unsigned int *) const;
//can do in wait4Image:
    void setRawImage(const unsigned int * src, short rows,short cols);
    void setRawImage(const EImage & );
    ushort getColorCount() const;
//can do in convertionReady:
    bool convert(convertAlgo=RGB_Better,bool dither=false);
    short getImageRows() const;
    short getImageCols() const;
    bool isVanilla() const;//判断是可以生存实装的地图画
    bool isFlat() const;//判断是平板的

//can do in converted:
    bool build(compressSettings=noCompress,unsigned short=256,
               glassBridgeSettings=noBridge,unsigned short=3,
               bool fireProof=false,bool endermanProof=false);//构建三维结构
    void getConvertedImage(short * rows,short * cols,unsigned int * dest) const;
    EImage getConovertedImage() const;
    void exportAsData(const char *, const int, int * fileCount,char**) const;
    std::vector<std::string> exportAsData(const std::string &,int) const;
//can do in builded:
    void exportAsLitematic(const char * TargetName,
                           const char * LiteName,
                           const char * author,
                           const char * RegionName,
                           char * FileName)const;
    std::string exportAsLitematic(const std::string & TargetName,//Local
                                             const std::string & LiteName,//Utf8
                                             const std::string & author,//Utf8
                                             const std::string & RegionName//Utf8
                             ) const;

    void exportAsStructure(const char * TargetName,char * FileName) const;
    std::string exportAsStructure(const std::string &) const;
    void get3DSize(int & x,int & y,int & z) const;
    int getHeight() const;
    int getXRange() const;
    int getZRange() const;

    void getBlockCounts(int * total, int detail[64]) const;
    int getBlockCounts(std::vector<int> & ) const;
    int getBlockCounts() const;

    const unsigned char * getBuild(int* xSize,int* ySize,int* zSize) const;
    const Eigen::Tensor<uchar,3> & getBuild() const;

private:
    friend class Kernel;
    enum ColorSpace {
        R='R',H='H',L='L',X='X'
    };

    static ColorSet Basic,Allowed;
    static const Eigen::Array<float,2,3> DitherMapLR,DitherMapRL;
    static const uint reportRate=100;

    gameVersion mcVer;//12,13,14,15,16,17
    mapTypes mapType;
    step kernelStep;
    convertAlgo ConvertAlgo;
    compressSettings compressMethod;
    glassBridgeSettings glassMethod;

    std::vector<simpleBlock> blockPalette;

    int size3D[3];//x,y,z

    EImage rawImage;
    EImage ditheredImage;

    std::unordered_map<ARGB,TokiColor> colorHash;

    ushort maxAllowedHeight;
    ushort bridgeInterval;
    PrimGlassBuilder * glassBuilder;
    LossyCompressor * Compressor;
    Eigen::ArrayXXi mapPic;//stores mapColor
    Eigen::ArrayXXi Base;
    Eigen::ArrayXXi HighMap;
    Eigen::ArrayXXi LowMap;
    std::unordered_map<TokiPos,waterItem> WaterList;
    Eigen::Tensor<uchar,3>Build;//x,y,z

//for setType:
//for convert:
    ColorSpace getColorSpace() const;
    void pushToHash();
    void applyTokiColor();
    void fillMapMat();
    void Dither();
    int sizePic(short) const;

//for build
    void getTokiColorPtr(ushort,const TokiColor*[]) const;
    //void makeHeight_old();//构建HighMap和LowMap
    void makeHeight_new();
    //void makeHeightInLine(const ushort c);
    void buildHeight(bool=false,bool=false);//构建Build
    void makeBridge();
//for Litematic
    static void writeBlock(const std::string &netBlockId,
                    const std::vector<std::string>&Property,
                    const std::vector<std::string>&ProVal,
                    NBT::NBTWriter&);
    static void writeTrash(int count,NBT::NBTWriter&);
    std::string Noder(const short *src,int size) const;

    Kernel * toBaseClassPtr() {
        return this;
    }

};

bool compressFile(const char*sourcePath,const char*destPath);
#endif // TOKISLOPECRAFT_H
