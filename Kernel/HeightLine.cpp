#include "HeightLine.h"

HeightLine::HeightLine()
{

}

float HeightLine::make(const TokiColor **src, const Array<uchar, Dynamic, 1> & g) {
    float sumDiff=0;
    ArrayXi mapColorCol(g.rows());
    for(ushort r=0;r<g.rows();r++) {
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
    make(mapColorCol);
    return sumDiff;
}

void HeightLine::make(const ArrayXi mapColorCol,bool allowNaturalCompress) {
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
    //qDebug("line 42");
    ArrayXi rawShadow=mapColorCol-4*(mapColorCol/4);

    if((rawShadow>=3).any()) {
        std::cerr<<"Fatal Error: depth=3 in vanilla map!"<<std::endl;
        return;
    }
    ArrayXi dealedDepth(picRows+1);
    dealedDepth.setZero();
    dealedDepth.segment(1,picRows)=rawShadow-1;

    if(base(1)==12||base(1)==0||rawShadow(0)==2)
        base(0)=0;
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
    //qDebug("step2 finished");
    ///////////////////////3
    for(ushort r=0;r<picRows;r++) {
        //HighMap.row(r+1)=HighMap.row(r)+dealedDepth.row(r+1);
        HighLine(r+1)=HighLine(r)+dealedDepth(r+1);
    }
    //qDebug("step3 finished");
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
    //qDebug("step4 finished");
    /////////////////5
    HighLine-=LowLine.minCoeff();
    LowLine-=LowLine.minCoeff();

    if(allowNaturalCompress) {
        OptiChain OC(base,HighLine,LowLine);
        OC.divideAndCompress();
        HighLine=OC.getHighLine();
        LowLine=OC.getLowLine();
    }
    //qDebug("无损压缩完毕");
    for(auto it=waterMap.begin();it!=waterMap.end();it++) {
        waterMap[it->first]=TokiWater(HighLine(it->first),
                                      LowLine(it->first));
        HighLine(it->first)+=1;
    }
    //qDebug("step5 finished");
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

const ArrayXi & HeightLine::getHighLine() const {
    return HighLine;
}
const ArrayXi & HeightLine::getLowLine() const {
    return LowLine;
}

const ArrayXi & HeightLine::getBase() const {
    return base;
}

const std::map<ushort,waterItem> & HeightLine::getWaterMap() const {
    return waterMap;
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
*/
/*
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
