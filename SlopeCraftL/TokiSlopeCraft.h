/*
 Copyright © 2021-2022  TokiNoBug
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

#ifdef SLOPECRAFTL_WITH_AICVETR
#include "AiConverterInterface.h"
#endif

using namespace SlopeCraft;
#include <thread>

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

#ifdef SLOPECRAFTL_CAPI
struct Kernel
{
public:
    Kernel();
    //virtual ~Kernel() {};

///create a kernel object
static Kernel * create();

public:
    static void getColorMapPtrs(const float**,const unsigned char**,int*);
    //full palette
    static const float * getBasicColorMapPtrs();

    static const char * getSCLVersion();

    static unsigned long long mcVersion2VersionNumber(gameVersion);

//can do in nothing:
    ///real size of kernel
    virtual unsigned long long size()=0;
    ///revert to a previous step
    virtual void decreaseStep(step)=0;
    ///replacement for operator delete
    virtual void destroy()=0;
    ///configure colorsheets
    virtual bool setColorSet(const char* _RGB,
                             const char* HSV,
                             const char* Lab,
                             const char* XYZ)=0;


//can do in colorSetReady:
    ///get current step
    virtual step queryStep() const=0;
    ///set map type and blocklist
    virtual bool setType(mapTypes,
                 gameVersion,
                 const bool [64],
                 const AbstractBlock * [64])=0;
    ///get TokiNoBug's url
    virtual void getAuthorURL(int * count,char ** dest) const=0;
    ///get palette (base colors only) in ARGB32
    virtual void getARGB32(unsigned int *) const=0;


//can do in wait4Image:
    ///set original image from ARGB32 matrix (col-major)
    virtual void setRawImage(const unsigned int * src, short rows,short cols)=0;
    ///get accessible color count
    virtual unsigned short getColorCount() const=0;
    ///make a structure that includes all accessible blocks
    virtual void makeTests(const AbstractBlock **,
                       const unsigned char *,
                       const char *,char*)=0;

//can do in convertionReady:
    ///convert original image to map
    virtual bool convert(convertAlgo=RGB_Better,bool dither=false)=0;
    ///get image rows
    virtual short getImageRows() const=0;
    ///get image cols
    virtual short getImageCols() const=0;
    ///query if map is buildable in vanilla survival
    virtual bool isVanilla() const=0;
    ///query if map is a flat one
    virtual bool isFlat() const=0;

//can do in converted:
    ///construct 3D structure
    virtual bool build(compressSettings=noCompress,unsigned short=256,
               glassBridgeSettings=noBridge,unsigned short=3,
               bool fireProof=false,bool endermanProof=false)=0;

    ///get converted image
    virtual void getConvertedImage(short * rows,short * cols,unsigned int * dest) const=0;
    ///export as map data files
    virtual void exportAsData(const char * FolderPath,
                              const int indexStart,
                              int* fileCount,
                              char ** dest) const=0;
    ///get converted map(in mapColor array)
    virtual void getConvertedMap(short * rows,short * cols,unsigned char *) const=0;

//can do in builded:
    ///export map into litematica files (*.litematic)
    virtual void exportAsLitematic(const char * TargetName,
                           const char * LiteName,
                           const char * author,
                           const char * RegionName,
                           char * FileName)const=0;
    ///export map into Structure files (*.NBT)
    virtual void exportAsStructure(const char * TargetName,char * FileName) const=0;

    ///get x,y,z size
    virtual void get3DSize(int & x,int & y,int & z) const=0;

    ///get 3d structure's size
    virtual int getHeight() const=0;
    ///get 3d structure's size
    virtual int getXRange() const=0;
    ///get 3d structure's size
    virtual int getZRange() const=0;
    ///get block count in total and in detail
    virtual void getBlockCounts(int * total, int detail[64]) const=0;
    ///get sum block count
    virtual int getBlockCounts() const=0;
    ///get 3d structure in 3d-matrix (col major)
    virtual const unsigned char * getBuild(int* xSize,int* ySize,int* zSize) const=0;

    ///function ptr to window object
    virtual void setWindPtr(void *)=0;
    ///a function ptr to show progress of converting and exporting
    virtual void setProgressRangeSet(void(*)(void*,int,int,int))=0;
    ///a function ptr to add progress value
    virtual void setProgressAdd(void(*)(void*,int))=0;
    ///a function ptr to prevent window from being syncoped
    virtual void setKeepAwake(void(*)(void*))=0;

    ///a function ptr to show progress of compressing and bridge-building
    virtual void setAlgoProgressRangeSet(void(*)(void*,int,int,int))=0;
    ///a function ptr to add progress value of compressing and bridge-building
    virtual void setAlgoProgressAdd(void(*)(void*,int))=0;

    ///a function ptr to report error when something wrong happens
    virtual void setReportError(void(*)(void*,errorFlag))=0;
    ///a function ptr to report working statue especially when busy
    virtual void setReportWorkingStatue(void(*)(void*,workStatues))=0;

protected:
    ///calling delete is deprecated, use void Kernel::destroy() instead
    void operator delete(void*) {}

};
#endif  //  #ifndef SLOPECRAFTL_CAPI

class TokiSlopeCraft : public Kernel
{
public:
    TokiSlopeCraft();
    virtual ~TokiSlopeCraft();

//can do in nothing:
    ///function ptr to window object
    void setWindPtr(void *);
    ///a function ptr to show progress of converting and exporting
    void setProgressRangeSet(void(*)(void*,int,int,int));
    ///a function ptr to add progress value
    void setProgressAdd(void(*)(void*,int));
    ///a function ptr to prevent window from being syncoped
    void setKeepAwake(void(*)(void*));

    ///a function ptr to show progress of compressing and bridge-building
    void setAlgoProgressRangeSet(void(*)(void*,int,int,int));
    ///a function ptr to add progress value of compressing and bridge-building
    void setAlgoProgressAdd(void(*)(void*,int));

    ///a function ptr to report error when something wrong happens
    void setReportError(void(*)(void*,errorFlag));
    ///a function ptr to report working statue especially when busy
    void setReportWorkingStatue(void(*)(void*,workStatues));
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
    uint16_t getColorCount() const;
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
    void getConvertedMap(short * rows,short * cols,unsigned char *) const;
    //void getConvertedMap(Eigen::Arra) const;
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
#ifdef SLOPECRAFTL_CAPI
    friend struct Kernel;
#else
    friend class Kernel;
#endif  //  #ifdef SLOPECRAFTL_CAPI
    //friend void * allowedRGB();
    //friend void * allowedMap();
    enum ColorSpace {
        R='R',H='H',L='L',X='X'
    };

    static ColorSet Basic,Allowed;
    static const Eigen::Array<float,2,3> DitherMapLR,DitherMapRL;
    static const uint32_t reportRate=100;

    void * wind;
    void (*progressRangeSet)(void*,int,int,int);
    void (*progressAdd)(void*,int);
    void (*keepAwake)(void*);
    void (*algoProgressRangeSet)(void*,int,int,int);
    void (*algoProgressAdd)(void*,int);
    void (*reportError)(void*,errorFlag);
    void (*reportWorkingStatue)(void*,workStatues);

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

    uint16_t maxAllowedHeight;
    uint16_t bridgeInterval;
    PrimGlassBuilder * glassBuilder;
    LossyCompressor * Compressor;
#ifdef SLOPECRAFTL_WITH_AICVETR
    AiConverterInterface * AiCvter;
#endif
    Eigen::ArrayXXi mapPic;//stores mapColor
    Eigen::ArrayXXi Base;
    Eigen::ArrayXXi HighMap;
    Eigen::ArrayXXi LowMap;
    std::unordered_map<TokiPos,waterItem> WaterList;
    Eigen::Tensor<uchar,3>Build;//x,y,z

//for setType:

#ifdef SLOPECRAFTL_WITH_AICVETR
    void configAiCvter();
#endif
//for setImage:

//for convert:
    ColorSpace getColorSpace() const;
    void pushToHash();
    void applyTokiColor();
    void fillMapMat();
    void Dither();
    int sizePic(short) const;

//for build
    void getTokiColorPtr(uint16_t,const TokiColor*[]) const;
    //void makeHeight_old();//构建HighMap和LowMap
    void makeHeight_new();
    //void makeHeightInLine(const uint16_t c);
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
