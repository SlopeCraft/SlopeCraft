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
#include <QObject>
#include <QRgb>
#include <QtConcurrent>
#include <QFuture>
#endif

typedef unsigned char gameVersion;

using namespace Eigen;

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
    explicit TokiSlopeCraft(const string &, QObject *parent = nullptr);
#else
    TokiSlopeCraft(const string &);
#endif
    enum convertAlgo{
        RGB,RGB_Better,HSV,Lab94,Lab00,XYZ
    };
    enum compressSettings{
        noCompress,Natural,Forced
    };
    enum mapType{
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
        mapType,
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
        R,H,L,X
    };
    gameVersion mcVer;//12,13,14,15,16,17
    mapType mapType;//S,C,F
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
//for convert:
    ColorSpace getColorSpace() const;
    void pushToHash();
    void matchColor();
    void fillMap();
    void Dither();

//for build
    void makeHeight();//构建HighMap和LowMap
    void buildHeight();//构建Build
//for Litematic
    void writeBlock(const string &netBlockId,
                    vector<string>&Property,
                    vector<string>&ProVal,
                    NBT::NBTWriter&);
    void writeTrash(int count,NBT::NBTWriter&);

};

#endif // TOKISLOPECRAFT_H
