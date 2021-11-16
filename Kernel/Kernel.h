#ifndef KERNEL_H
#define KERNEL_H

//#define WITH_QT

#ifdef WITH_QT
#include <QObject>
#endif

#include "Kernel_global.h"

namespace SlopeCraft {

class KERNEL_EXPORT AbstractBlock
{
public:
    AbstractBlock();
    virtual ~AbstractBlock() {};

    static AbstractBlock * createSimpleBlock();

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
};


class KERNEL_EXPORT Kernel
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

    virtual ~Kernel() {};


#ifdef WITH_QT
static Kernel *createKernel(QObject * parent=nullptr);
#else
static Kernel * createKernel();
#endif

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
    enum convertAlgo {
        RGB='r',
        RGB_Better='R',
        HSV='H',
        Lab94='l',
        Lab00='L',
        XYZ='X'
    };
    enum compressSettings {
        noCompress=0,NaturalOnly=1,ForcedOnly=2,Both=3
    };
    enum glassBridgeSettings {
        noBridge=0,withBridge=1
    };
    enum mapTypes {
        Slope=0, //立体
        Flat=1, //平板
        FileOnly=2,//纯文件
        Wall=3,//竖版
    };
    enum step {
        nothing,//对象刚刚创建，什么都没做
        colorSetReady,//颜色表读取完毕
        wait4Image,//等待图片
        convertionReady,//一切就绪，等待convert
        converted,//已经将图像转化为地图画
        builded,//构建了三维结构
    };
    enum errorFlag {
        NO_ERROR_OCCUR=-1,//无故障
        HASTY_MANIPULATION=0x00,//跳步操作
        LOSSYCOMPRESS_FAILED=0x01,//有损压缩失败
        DEPTH_3_IN_VANILLA_MAP=0x02,//非纯文件地图画中出现深度为3的颜色
        MAX_ALLOWED_HEIGHT_LESS_THAN_14=0x03,
        USEABLE_COLOR_TO_LITTLE=0x04,
        EMPTY_RAW_IMAGE=0x05,
        PARSING_COLORMAP_RGB_FAILED=0x10,//颜色表错误
        PARSING_COLORMAP_HSV_FAILED=0x11,
        PARSING_COLORMAP_Lab_FAILED=0x12,
        PARSING_COLORMAP_XYZ_FAILED=0x13,
    };
    enum workStatues {
        none=-1,
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

        virtual void getAuthorURL(char ** dest) const=0;
        //get TokiNoBug's url

        virtual void getARGB32(unsigned int *) const=0;
        //get palette in ARGB32

    //can do in wait4Image:
        virtual void setRawImage(const unsigned int * src, short rows,short cols)=0;
        // set original image from ARGB32 matrix (col-major)
        virtual unsigned short getColorCount() const=0;
        //get accessible color count
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

        virtual void getBlockCounts(int * total, int detail[64]) const=0;
        virtual int getBlockCounts() const=0;

        virtual const unsigned char * getBuild(int* xSize,int* ySize,int* zSize) const=0;

    #ifdef WITH_QT
    signals:
        void progressRangeSet(int min,int max,int val) const;//设置进度条的取值范围和值
        void progressAdd(int deltaVal) const;
        void keepAwake() const;//保持主窗口唤醒

        void algoProgressRangeSet(int,int,int) const;
        void algoProgressAdd(int) const;

        void reportError(errorFlag) const;
        void reportWorkingStatue(workStatues) const;

    #else
        void (*progressRangeSet)(int,int,int);
        void (*progressAdd)(int);
        void (*keepAwake)();

        void (*algoProgressRangeSet)(int,int,int);
        void (*algoProgressAdd)(int);

        void (*reportError)(errorFlag);
        void (*reportWorkingStatue)(workStatues);
    #endif

private:
};


};
#endif // KERNEL_H
