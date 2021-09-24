#include "HeightLine.h"

const ARGB HeightLine::BlockColor=ARGB32(0,0,0);
const ARGB HeightLine::AirColor=ARGB32(255,255,255);
const ARGB HeightLine::WaterColor=ARGB32(0,64,255);
const ARGB HeightLine::greyColor=ARGB32(192,192,192);

HeightLine::HeightLine() {

}

float HeightLine::make(const TokiColor **src,
                       const Eigen::Array<uchar, Eigen::Dynamic, 1> & g,
                       bool allowNaturalCompress) {
    float sumDiff=0;
    Eigen::ArrayXi mapColorCol(g.rows());
    //mapColorCol(0)=11;
    //std::cerr<<"gene size="<<g.rows()<<std::endl;
    //std::cerr<<g.transpose().cast<ushort>()<<std::endl;
    //std::cerr<<"parsing gene\n";
    for(ushort r=0;r<g.rows();r++) {
        //std::cerr<<"row="<<r<<std::endl;
        if(src[r]==nullptr) {
            std::cerr<<"Fatal Error! nullptr found in src\n";
            return 0;
        }
        switch (g(r)) {
        case 0:
            mapColorCol(r)=src[r]->Result;
            sumDiff+=src[r]->ResultDiff;
            break;
        case 1:
            mapColorCol(r)=src[r]->sideResult[0];
            sumDiff+=src[r]->sideSelectivity[0];
            break;
        default:
            mapColorCol(r)=src[r]->sideResult[1];
            sumDiff+=src[r]->sideSelectivity[1];
            break;
        }
    }
    //std::cerr<<"mapColorCol="<<mapColorCol.transpose()<<std::endl;
    //std::cerr<<"making\n";
    make(mapColorCol,allowNaturalCompress);
    return sumDiff;
}

void HeightLine::make(const Eigen::ArrayXi &mapColorCol,bool allowNaturalCompress) {
    ///////////////////////1
    waterMap.clear();
    const ushort picRows=mapColorCol.rows();
    base.setConstant(1+picRows,11);
    HighLine.setZero(1+picRows);
    LowLine.setZero(1+picRows);
    //qDebug("初始化完毕");

    //////////////////////////////2
    //qDebug()<<"size(base.segment(1,picRows))=["<<base.segment(1,picRows).rows()<<','<<base.segment(1,picRows).cols()<<']';
    //qDebug()<<"size(mapColorCol)=["<<mapColorCol.rows()<<','<<mapColorCol.cols()<<']';
    base.segment(1,picRows)=mapColorCol/4;
    Eigen::ArrayXi rawShadow=mapColorCol-4*(mapColorCol/4);

    if((rawShadow>=3).any()) {
        std::cerr<<"Fatal Error: depth=3 in vanilla map!"<<std::endl;
        std::cerr<<"SlopeCraft will crash."<<std::endl;
        delete &rawShadow;
        return;
    }
    Eigen::ArrayXi dealedDepth(picRows+1);
    dealedDepth.setZero();
    dealedDepth.segment(1,picRows)=rawShadow-1;

    if(base(1)==12||base(1)==0||rawShadow(0)==2) {
        base(0)=0;
        dealedDepth(1)=0;
    }
    for(ushort r=1;r<picRows;r++) {
        if(base(r+1)==0) {
            dealedDepth(r+1)=0;
            continue;
        }
        if(base(r+1)==12) {
            dealedDepth(r+1)=0;
            waterMap[r+1]=nullWater;
        }
    }
    ///////////////////////3
    for(ushort r=0;r<picRows;r++) {
        //HighMap.row(r+1)=HighMap.row(r)+dealedDepth.row(r+1);
        HighLine(r+1)=HighLine(r)+dealedDepth(r+1);
    }
    //////////////////4
    LowLine=HighLine;
    for(auto it=waterMap.cbegin();it!=waterMap.cend();it++) {
        /*
        LowMap(TokiRow(it->first),TokiCol(it->first))=
                HighMap(TokiRow(it->first),TokiCol(it->first))
                -WaterColumnSize[rawShadow(TokiRow(it->first)-1,TokiCol(it->first))]+1;
*/
        LowLine(it->first)=HighLine(it->first)-WaterColumnSize[rawShadow(it->first-1)]+1;
    }
    /////////////////5
    HighLine-=LowLine.minCoeff();
    LowLine-=LowLine.minCoeff();

    if(allowNaturalCompress) {
        OptiChain OC(base,HighLine,LowLine);
        OC.divideAndCompress();
        HighLine=OC.getHighLine();
        LowLine=OC.getLowLine();
    }
    for(auto it=waterMap.begin();it!=waterMap.end();it++) {
        waterMap[it->first]=TokiWater(HighLine(it->first),
                                      LowLine(it->first));
        HighLine(it->first)+=1;
    }
}

ushort HeightLine::maxHeight() const {
    return HighLine.maxCoeff()-LowLine.minCoeff()+1;
}

void HeightLine::updateWaterMap() {
    waterMap.clear();
    for(ushort r=1;r<base.rows();r++) {
        if(base(r)!=12)
            continue;
        waterMap[r]=TokiWater(HighLine(r)-1,LowLine(r));
    }
}

const Eigen::ArrayXi & HeightLine::getHighLine() const {
    return HighLine;
}
const Eigen::ArrayXi & HeightLine::getLowLine() const {
    return LowLine;
}

const Eigen::ArrayXi & HeightLine::getBase() const {
    return base;
}

const std::map<ushort,waterItem> & HeightLine::getWaterMap() const {
    return waterMap;
}

EImage HeightLine::toImg() const {
    const short rMax=maxHeight()-1;
    EImage img(maxHeight(),HighLine.size());
    img.setConstant(AirColor);
    short y=0,r=rMax-y;
    for(ushort x=0;x<HighLine.size();x++) {
        y=HighLine(x);
        r=rMax-y;
        if(base(x)) {
            if(base(x)!=12) {
                img(r,x)=BlockColor;
            } else {
                img(r,x)=greyColor;
                short rmin=rMax-LowLine(x);
                r++;
                img.col(x).segment(r,rmin-r+1)=WaterColor;
                if(rmin<rMax) {
                    rmin++;
                    img(rmin,x)=greyColor;
                }

            }
        }

    }
    return img;
}

/*
Base.setConstant(sizePic(0)+1,sizePic(1),11);
    WaterList.clear();
    HighMap.setZero(sizePic(0)+1,sizePic(1));
    LowMap.setZero(sizePic(0)+1,sizePic(1));

    Base.block(1,0,sizePic(0),sizePic(1))=mapPic/4;

    ArrayXXi dealedDepth;
    ArrayXXi rawShadow=mapPic-4*(mapPic/4);

    if((rawShadow>=3).any())
    {
        qDebug("错误：Depth中存在深度为3的方块");
        return;
    }
    dealedDepth.setZero(sizePic(0)+1,sizePic(1));
    dealedDepth.block(1,0,sizePic(0),sizePic(1))=rawShadow-1;
    //Depth的第一行没有意义，只是为了保持行索引一致

    for(short r=0;r<Base.rows();r++)
    {
        for(short c=0;c<Base.cols();c++)
        {
            if(Base(r,c)==12)
            {
                WaterList[TokiRC(r,c)]=nullWater;
                dealedDepth(r,c)=0;
                continue;
            }
            if(Base(r,c)==0)
            {
                dealedDepth(r,c)=0;
                continue;
            }
        }
        emit progressAdd(sizePic(0));
    }

    HighMap.setZero(sizePic(0)+1,sizePic(1));
    LowMap.setZero(sizePic(0)+1,sizePic(1));

    int waterCount=WaterList.size();
    qDebug()<<"共有"<<waterCount<<"个水柱";
    for(short r=0;r<sizePic(0);r++)//遍历每一行，根据高度差构建高度图
    {
        HighMap.row(r+1)=HighMap.row(r)+dealedDepth.row(r+1);
        emit progressAdd(sizePic(0));
    }

    for(short c=0;c<Base.cols();c++)
    {
        if(Base(1,c)==0||Base(1,c)==12||rawShadow(0,c)==2)
        {
            Base(0,c)=0;
            HighMap(0,c)=HighMap(1,c);
        }
        emit progressAdd(sizePic(1));
    }

    cerr<<"extra north side stones removed"<<endl;

    LowMap=HighMap;

    for(auto it=WaterList.begin();it!=WaterList.end();it++)
    {
        LowMap(TokiRow(it->first),TokiCol(it->first))=
                HighMap(TokiRow(it->first),TokiCol(it->first))
                -WaterColumnSize[rawShadow(TokiRow(it->first)-1,TokiCol(it->first))]+1;
    }

    cerr<<"LowMap updated"<<endl;

    for(short c=0;c<sizePic(1);c++)
    {
        HighMap.col(c)-=LowMap.col(c).minCoeff();
        LowMap.col(c)-=LowMap.col(c).minCoeff();
        //沉降每一列
        emit progressAdd(sizePic(1));
    }

    cerr<<"basic sink done"<<endl;

    if(compressMethod==NaturalOnly)
    {
        //执行高度压缩
        OptiChain::Base=Base;
        for(int c=0;c<sizePic(1);c++)
        {
            OptiChain Compressor(HighMap.col(c),LowMap.col(c),c);
            Compressor.divideAndCompress();
            HighMap.col(c)=Compressor.HighLine;
            LowMap.col(c)=Compressor.LowLine;
            emit progressAdd(sizePic(1));
        }
    }

    cerr<<"waterList updated again"<<endl;

    int maxHeight=HighMap.maxCoeff();

    for(auto it=WaterList.begin();it!=WaterList.end();it++)
    {
        int r=TokiRow(it->first),c=TokiCol(it->first);
        it->second=TokiWater(HighMap(r,c),LowMap(r,c));
        maxHeight=max(maxHeight,HighMap(r,c)+1);
        //遮顶玻璃块
    }
    size3D[2]=2+sizePic(0);//z
    size3D[0]=2+sizePic(1);//x
    size3D[1]=1+maxHeight;//y
    return;
*/
