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

#ifndef KERNEL_H
#define KERNEL_H
//#define WITH_QT
//#define NO_DLL

#ifdef WITH_QT
    #include <QObject>
    #define NO_DLL
#endif

#ifndef NO_DLL
    #include "SlopeCraftL_global.h"
    #define SCL_EXPORT SLOPECRAFTL_EXPORT
#endif

namespace SlopeCraft {

#ifndef NO_DLL
class SCL_EXPORT AbstractBlock
#else
class AbstractBlock
#endif
{
public:
    AbstractBlock();
    //virtual ~AbstractBlock() {};
#ifndef NO_DLL
    static AbstractBlock * create();
#endif
    virtual unsigned long long size()=0;

    virtual const char* getId()const=0;
    virtual unsigned char getVersion()const=0;
    virtual const char* getIdOld()const=0;
    virtual bool getNeedGlass()const=0;
    virtual bool getDoGlow()const=0;
    virtual bool getEndermanPickable()const=0;
    virtual bool getBurnable()const=0;
    virtual bool getWallUseable()const=0;

    virtual void setId(const char*)=0;
    virtual void setVersion(unsigned char)=0;
    virtual void setIdOld(const char*)=0;
    virtual void setNeedGlass(bool)=0;
    virtual void setDoGlow(bool)=0;
    virtual void setEndermanPickable(bool)=0;
    virtual void setBurnable(bool)=0;
    virtual void setWallUseable(bool)=0;

    void copyTo(AbstractBlock * dst) const;
    void clear();

    virtual void destroy()=0;
};


#ifndef NO_DLL
class SCL_EXPORT Kernel
#else
class  Kernel
#endif
        #ifdef WITH_QT
        : public QObject
        #endif
{
#ifdef WITH_QT
    Q_OBJECT
public:
    explicit Kernel(QObject *parent = nullptr);
#else
public:
    Kernel();
#endif
    //virtual ~Kernel() {};

#ifndef NO_DLL
static Kernel * create();
#endif


enum gameVersion {
    ANCIENT=0,//older than 1.12
    MC12=12,//1.12
    MC13=13,//1.13
    MC14=14,//1.14
    MC15=15,//1.15
    MC16=16,//1.16
    MC17=17,//1.17
    FUTURE=255//future version
};
enum convertAlgo {//color difference formula used to match colors
    RGB='r',//naive RGB
    RGB_Better='R',//RGB with rotation
    HSV='H',//naive HSV formula
    Lab94='l',//CIELAB 1994 formula
    Lab00='L',//CIELAB 2000 formula
    XYZ='X'//naive XYZ formula
};
enum compressSettings {
    noCompress=0,//don't compress
    NaturalOnly=1,//compress in lossless only
    ForcedOnly=2,//compress in lossy only
    Both=3//compress with both lossless and lossy
};
enum glassBridgeSettings {
    noBridge=0,//don't construce bridge
    withBridge=1//construct bridge
};
enum mapTypes {
    Slope=0, //3D
    Flat=1, //flat
    FileOnly=2,//map data files
    Wall=3,//wall
};
enum step {
    nothing,//the instance is created
    colorSetReady,//colorset is configured
    wait4Image,//map type is set and waitting for image
    convertionReady,//image is ready and ready for converting
    converted,//image is converted and ready for building 3D structure, exporting as file-only map(s) can be done in this step
    builded,//3D structure is built and ready for exporting 3d structure
};
enum errorFlag {
    NO_ERROR_OCCUR=-1,//no error
    HASTY_MANIPULATION=0x00,//trying to skip steps
    LOSSYCOMPRESS_FAILED=0x01,//failed when compressing in lossy
    DEPTH_3_IN_VANILLA_MAP=0x02,//color in shadow 3 appears in vanilla map
    MAX_ALLOWED_HEIGHT_LESS_THAN_14=0x03,
    USEABLE_COLOR_TOO_FEW=0x04,//too few color to convert
    EMPTY_RAW_IMAGE=0x05,//the original image is empty
    PARSING_COLORMAP_RGB_FAILED=0x10,//colorsheet error
    PARSING_COLORMAP_HSV_FAILED=0x11,//colorsheet error
    PARSING_COLORMAP_Lab_FAILED=0x12,//colorsheet error
    PARSING_COLORMAP_XYZ_FAILED=0x13,//colorsheet error
};
enum workStatues {
    none=-1,//waiting
    collectingColors=0x00,
    converting=0x01,
    dithering=0x02,
    //convertFinished=0x03,

    buidingHeighMap=0x10,
    compressing=0x11,
    building3D=0x12,
    constructingBridges=0x13,
    flippingToWall=0x14,

    writingMetaInfo=0x20,
    writingBlockPalette=0x21,
    writing3D=0x22,
    //slopeFinished=0x16,

    writingMapDataFiles=0x30,
    //dataFilesFinished=0x31,
};
//can do in nothing:
    virtual unsigned long long size()=0;
    virtual void decreaseStep(step)=0;
    virtual void destroy()=0;
//revert to a previous step
    virtual bool setColorSet(const char* _RGB,
                             const char* HSV,
                             const char* Lab,
                             const char* XYZ)=0;
    //configure colorsheets

//can do in colorSetReady:
    virtual step queryStep() const=0;
    //get current step

    virtual bool setType(mapTypes,
                 gameVersion,
                 const bool [64],
                 const AbstractBlock * [64])=0;
    //set map type and blocklist

    virtual void getAuthorURL(int * count,char ** dest) const=0;
    //get TokiNoBug's url

    virtual void getARGB32(unsigned int *) const=0;
    //get palette in ARGB32

//can do in wait4Image:
    virtual void setRawImage(const unsigned int * src, short rows,short cols)=0;
    // set original image from ARGB32 matrix (col-major)
    virtual unsigned short getColorCount() const=0;
    //get accessible color count
    virtual void makeTests(const AbstractBlock **,
                       const unsigned char *,
                       const char *,char*)=0;
    //make a structure that includes all accessible blocks
//can do in convertionReady:
    virtual bool convert(convertAlgo=RGB_Better,bool dither=false)=0;
    //convert original image to map
    virtual short getImageRows() const=0;
    //get image rows
    virtual short getImageCols() const=0;
    //get image cols
    virtual bool isVanilla() const=0;
    //query if map is buildable in vanilla survival
    virtual bool isFlat() const=0;
    //query if map is a flat one

//can do in converted:

    virtual bool build(compressSettings=noCompress,unsigned short=256,
               glassBridgeSettings=noBridge,unsigned short=3,
               bool fireProof=false,bool endermanProof=false)=0;
    //construct 3D structure

    virtual void getConvertedImage(short * rows,short * cols,unsigned int * dest) const=0;
    //get converted image
//can do in builded:
    virtual void exportAsLitematic(const char * TargetName,
                           const char * LiteName,
                           const char * author,
                           const char * RegionName,
                           char * FileName)const=0;
    //export map into litematica files (*.litematic)

    virtual void exportAsStructure(const char * TargetName,char * FileName) const=0;
    //export map into Structure files (*.NBT)
    virtual void get3DSize(int & x,int & y,int & z) const=0;
    //get x,y,z size

    virtual int getHeight() const=0;
    virtual int getXRange() const=0;
    virtual int getZRange() const=0;
    //get 3d structure's size

    virtual void getBlockCounts(int * total, int detail[64]) const=0;
    //get block count in total and in detail
    virtual int getBlockCounts() const=0;
    //get sum block count

    virtual const unsigned char * getBuild(int* xSize,int* ySize,int* zSize) const=0;
    //get 3d structure in 3d-matrix (col major)

#ifdef WITH_QT
signals:
    void progressRangeSet(int min,int max,int val) const;
    void progressAdd(int deltaVal) const;
    void keepAwake() const;

    void algoProgressRangeSet(int,int,int) const;
    void algoProgressAdd(int) const;

    void reportError(errorFlag) const;
    void reportWorkingStatue(workStatues) const;

#else
    void (*progressRangeSet)(int,int,int);
    //a function ptr to show progress of converting and exporting
    void (*progressAdd)(int);
    //a function ptr to add progress value
    void (*keepAwake)();
    //a function ptr to prevent window from being syncoped

    void (*algoProgressRangeSet)(int,int,int);
    //a function ptr to show progress of compressing and bridge-building
    void (*algoProgressAdd)(int);
    //a function ptr to add progress value of compressing and bridge-building

    void (*reportError)(errorFlag);
    //a function ptr to report error when something wrong happens
    void (*reportWorkingStatue)(workStatues);
    //a function ptr to report working statue especially when busy
#endif

};
}
#endif // KERNEL_H
