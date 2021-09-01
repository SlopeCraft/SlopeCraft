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
#include "TokiColor.h"
#include "WaterItem.h"
#include "NBTWriter.h"

#ifdef WITH_QT

#endif

#include <QObject>
#include <QRgb>
#include <QtConcurrent>
#include <QFuture>
typedef unsigned char gameVersion;

using namespace Eigen;

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
    explicit TokiSlopeCraft(const vector<string> &, QObject *parent = nullptr);
#else
    TokiSlopeCraft(const vector<string> &);
#endif
    enum convertAlgo{
        RGB='r',
        RGB_Better='R',
        HSV='H',
        Lab94='l',
        Lab00='L',
        XYZ='X'
    };
    enum compressSettings{
        noCompress,Natural,Forced
    };
    enum mapTypes{
        Slope, //立体
        Flat, //平板
        FileOnly,//纯文件
    };
    enum step{
        nothing,//对象刚刚创建，什么都没做
        convertionReady,//一切就绪，等待convert
        converted,//已经将图像转化为地图画
        builded,//构建了三维结构
    };
//can do in nothing:
    step queryStep() const;
    bool setType(
        mapTypes,
        gameVersion,
        const bool [64],
        simpleBlock [64] ,
        const ArrayXXi &);

    vector<string> getAuthorURL() const;

//can do in convertionReady:
    bool convert(convertAlgo=RGB_Better,bool dither=false);
    short getImageRows() const;
    short getImageCols() const;

//can do in converted:
    bool build(compressSettings,ushort maxAllowedHeight);//构建三维结构
    ArrayXXi getConovertedImage() const;
    string exportAsData(const string & ,int) const;
//can do in builded:
    string exportAsLitematic(const string &);
    string exportAsStructure(const string &);
    void get3DSize(int & x,int & y,int & z) const;
    int getHeight() const;
    int getBlockCounts(vector<int> & ) const;

#ifdef WITH_QT
signals:
    void convertProgressRangeSet(int min,int max,int val);//设置进度条的取值范围和值
    void convertProgressSetVal(int val);
    void convertProgressAdd(int deltaVal);
    void buildProgressRangeSet(int min,int max,int val);//设置进度条的取值范围
    void buildProgressAdd(int deltaVal);
    void exportProgressRangeSet(int min,int max,int val);//设置进度条的取值范围
    void exportProgressAdd(int deltaVal);
    void keepAwake();//保持主窗口唤醒
private slots:

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

    ColorSet Basic;
    ColorSet Allowed;
    vector<simpleBlock> blockPalette;

    int size3D[3];//x,y,z
    int totalBlocks;
    vector<int> blockCounts;

    ArrayXXi rawImage;

    std::unordered_map<QRgb,TokiColor> colorHash;

    ArrayXXi mapPic;//stores mapColor
    ArrayXXi Base;
    ArrayXXi HighMap;
    ArrayXXi LowMap;
    std::unordered_map<TokiPos,waterItem> WaterList;
    Tensor<unsigned char,3>Build;//x,y,z

//for setType:
    void makeAllowedColorIndex(bool*);
    void makeColorSet();
    bool isVanilla() const;//判断是可以生存实装的地图画
    bool isFlat() const;//判断是平板的
//for convert:
    ColorSpace getColorSpace() const;
    void pushToHash();
    void applyTokiColor();
    void fillMapMat();
    void Dither();
    short sizePic(short) const;

//for build
    void makeHeight();//构建HighMap和LowMap
    void buildHeight();//构建Build
//for Litematic
    void writeBlock(const string &netBlockId,
                    vector<string>&Property,
                    vector<string>&ProVal,
                    NBT::NBTWriter&);
    void writeTrash(int count,NBT::NBTWriter&);
    string Noder(const short *src,int size) const;

};

bool readFromTokiColor(const char*FileName,ArrayXXf & M);
uchar h2d(char h);
void crash();
void matchColor(TokiColor * tColor,QRgb qColor);

#endif // TOKISLOPECRAFT_H
