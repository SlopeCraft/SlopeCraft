#include "TokiSlopeCraft.h"
#ifdef WITH_QT

Array<float,2,3>TokiSlopeCraft::DitherMapLR;
Array<float,2,3>TokiSlopeCraft::DitherMapRL;

TokiSlopeCraft::TokiSlopeCraft(QObject *parent) : QObject(parent)
#else
TokiSlopeCraft::TokiSlopeCraft()
#endif
{
    kernelStep=TokiSlopeCraft::step::nothing;
    rawImage.setZero(0,0);
    DitherMapLR<<0.0,0.0,7.0,
                             3.0,5.0,1.0;
    DitherMapRL<<7.0,0.0,0.0,
                             1.0,5.0,3.0;
    DitherMapLR/=16.0;
    DitherMapRL/=16.0;
}

bool TokiSlopeCraft::setColorSet(const char*R,const char*H,const char*L,const char*X) {
    if(!readFromTokiColor(R,Basic._RGB)) {
        cerr<<"Failed to read colormap RGB\n";
        //crash();
        return false;
    }
    if(!readFromTokiColor(H,Basic.HSV)) {
        cerr<<"Failed to read colormap HSV\n";
        //crash();
        return false;
    }
    if(!readFromTokiColor(L,Basic.Lab)) {
        cerr<<"Failed to read colormap Lab\n";
        //crash();
        return false;
    }
    if(!readFromTokiColor(X,Basic.XYZ)) {
        cerr<<"Failed to read colormap XYZ\n";
        //crash();
        return false;
    }
    kernelStep=colorSetReady;
    return true;
}

void crash() {
    int i;
    delete &i;
    return;
}
uchar h2d(char h) {
    //cout<<h;
    if(h>='0'&&h<='9')
        return h-'0';
    if(h>='A'&&h<='Z')
        return h-'A'+10;
    if(h>='a'&&h<='z')
        return h-'a'+10;
    cerr<<"Wrong byte:"<<(int)h<<"->"<<h;
    return 255;
}
bool readFromTokiColor(const string & FileName,ArrayXXf & M) {
    fstream Reader;
    Reader.open(FileName, ios::in|ios::binary);
    if(!Reader)return false;

    char * buf=new char[7168];
    Reader.read(buf,7168);
    bool result=readFromTokiColor(buf,M);
    delete[] buf;
    Reader.close();
    return result;
}

bool readFromTokiColor(const char*src,ArrayXXf & M) {
    const char * buf=src;
    /*
    string fileMD5=
    QCryptographicHash::hash(buf,QCryptographicHash::Md5).toHex().toStdString();
    //cout<<FileName<<",hash="<<fileMD5<<endl;
    if(fileMD5!=MD5) {
#ifdef WITH_QT
        qDebug("颜色表文件哈希校验失败");
#endif
        delete [] buf;
        return false;
    }*/
    float temp;
    uchar * wp=(uchar*)&temp;
    uchar * rp=(uchar*)buf;
    M.setZero(256,3);

    for(int i=0;i<M.size();i++) {

        for(int byte=0;byte<4;byte++)
            wp[byte]=(h2d(rp[byte*2+0])<<4)|(h2d(rp[byte*2+1]));
        M(i/3,i%3)=temp;
        rp+=9;
        if(i%3==2)
            rp++;
    }
    return true;
}

TokiSlopeCraft::step TokiSlopeCraft::queryStep() const {
    return kernelStep;
}

bool TokiSlopeCraft::setType(mapTypes type,
                             gameVersion ver,
                             const bool * allowedBaseColor,
                             simpleBlock * palettes,
                             const ArrayXXi & _rawimg) {
    if(kernelStep<colorSetReady)return false;
    rawImage=_rawimg;
    mapType=type;
    mcVer=ver;

    blockPalette.resize(64);
    for(short i=0;i<64;i++)
        blockPalette[i]=*palettes;

    ArrayXi baseColorVer(64);//基色对应的版本
    baseColorVer.setConstant(FUTURE);
    baseColorVer.segment(0,52).setConstant(ANCIENT);
    baseColorVer.segment(52,7).setConstant(MC16);
    baseColorVer.segment(59,3).setConstant(MC17);

    bool MIndex[256];

    for(short index=0;index<256;index++) {
        MIndex[index]=true;//默认可以使用这种颜色，逐次判断各个不可以使用的条件

        if(!allowedBaseColor[index2baseColor(index)]) {//在allowedBaseColor中被禁止
            MIndex[index]=false;
            continue;
        }
        if(index2baseColor(index)==0) {//空气禁用
            MIndex[index]=false;
            continue;
        }
        if(mcVer<baseColorVer(index2baseColor(index))) {//版本低于基色版本
            MIndex[index]=false;
            continue;
        }
        if(blockPalette[index2baseColor(index)].id.empty()) {//空id
            MIndex[index]=false;
            continue;
        }
        if(isVanilla()&&(index2depth(index)>=3)) {//可实装的地图画不允许第四种阴影
            MIndex[index]=false;
            continue;
        }
        if(index2baseColor(index)==12) {//如果是水
            if(isFlat()&&index2depth(index)!=2) {//平板且水深不是1格
                MIndex[index]=false;
                continue;
            }
        } else {
            if(isFlat()&&index2depth(index)!=1) {//平板且阴影不为1
                MIndex[index]=false;
                continue;
            }
        }
    }

    Allowed.ApplyAllowed(&Basic,MIndex);

    kernelStep=convertionReady;
    return true;
}

bool TokiSlopeCraft::isVanilla() const {
    return mapType!=FileOnly;
}

bool TokiSlopeCraft::isFlat() const {
    return mapType==Flat;
}

vector<string> TokiSlopeCraft::getAuthorURL() const {
    if(kernelStep<colorSetReady) {
        vector<string> error(2);
        error[0]="Too hasty operation!";
        error[1]="make sure that you've deployed the colormap!";
        return error;
    }
    vector<string> urls(2);
    static string Toki="";
        if(Toki=="")
        {
            const short size3D[]={1229, 150, 150, 44, 40, 69, 204, 204, 376, 114, 150, 1229, 598, 182, 142, 173, 110, 238, 204, 132, 110, 117, 114, 882, 110, 7, 598, 376, 204, 101, 166, 110, 44, 364, 870, 169, 922, 134, 150,};
            Toki=Noder(size3D,sizeof(size3D)/2);
        }
        static string Doki="";
            if(Doki=="")
            {
                const short sizePic[]={1229, 150, 150, 44, 40, 69, 204, 204, 40, 44, 922, 173, 364, 142, 182, 114, 166, 114, 182, 114, 166, 114, 142, 173, 110, 238, 204, 80, 218, 380, 56, 28, 286, 28, 80, 380};
                Doki=this->Noder(sizePic,sizeof(sizePic)/2);
            }
            urls[0]=Toki;
            urls[1]=Doki;
            return urls;
}

string TokiSlopeCraft::Noder(const short *src,int size) const {
    string dst;
        char*u=nullptr;
        for(int i=0;i<size;i++)
        {
            u=(char*)(&Basic.Lab(src[i]/4))+src[i]%4;
            dst.push_back(*u);
        }
        return dst;
}

bool TokiSlopeCraft::convert(convertAlgo algo,bool dither) {
    if(kernelStep<convertionReady)
        return false;
    ConvertAlgo=algo;
    colorHash.clear();

    emit convertProgressRangeSet(0,4*sizePic(2),0);
/*
//第一步，装入hash顺便转换颜色空间;（一次遍历
//第二步，遍历hash并匹配颜色;（一次遍历
//第三步，从hash中检索出对应的匹配结果;（一次遍历
//第四步，抖动（一次遍历*/
    pushToHash();

    emit keepAwake();
    emit convertProgressSetVal(1*sizePic(2));

    applyTokiColor();

    emit keepAwake();
    emit convertProgressSetVal(2*sizePic(2));

    fillMapMat();
    emit keepAwake();
    emit convertProgressSetVal(3*sizePic(2));

    if(dither)
        Dither();

    emit convertProgressSetVal(4*sizePic(2));

    emit keepAwake();

    kernelStep=converted;
    return true;
}

short TokiSlopeCraft::sizePic(short dim) const {
    if(dim==0) return rawImage.rows();
    if(dim==1) return rawImage.cols();
    return rawImage.size();
}

void TokiSlopeCraft::pushToHash() {
    auto R=&colorHash;
    R->clear();
        int ColorCount=0;
        TokiColor::Allowed=&Allowed;
        TokiColor::Basic=&Basic;
        char Mode=ConvertAlgo;
        for(short r=0;r<sizePic(0);r++)
        {
            for(short c=0;c<sizePic(1);c++)
                if(R->find(rawImage(r,c))==R->end())//找不到这个颜色
                {
                    ColorCount++;
                    R->operator[](rawImage(r,c))=TokiColor(rawImage(r,c),Mode);
                }
            emit convertProgressAdd(sizePic(1));
        }
        qDebug()<<"总颜色数量："<<R->size();
}

void TokiSlopeCraft::applyTokiColor() {
    auto R=&colorHash;
    int step=sizePic(2)/R->size();
        queue<QFuture<void>> taskTracker;
        for(auto it=R->begin();it!=R->end();it++)
            taskTracker.push(QtConcurrent::run(matchColor,&it->second,it->first));

        while(!taskTracker.empty()) {
            taskTracker.front().waitForFinished();
            emit convertProgressAdd(step);
            taskTracker.pop();
        }

        qDebug("子线程执行完毕");
}

void TokiSlopeCraft::fillMapMat() {
    auto R=&colorHash;
        for(short r=0;r<sizePic(0);r++)
        {
            for(short c=0;c<sizePic(1);c++)
            {
                mapPic(r,c)=R->operator[](rawImage(r,c)).Result;
            }
            emit convertProgressAdd(sizePic(1));
        }
}

void TokiSlopeCraft::Dither() {
    auto R=&colorHash;
    ArrayXXf Dither[3];
    /*
    cout<<"DitherMapLR="<<endl;
    cout<<DitherMapLR<<endl;
    cout<<"DitherMapRL="<<endl;
    cout<<DitherMapRL<<endl;*/
    bool isDirLR=true;
    Dither[0].setZero(sizePic(0)+2,sizePic(1)+2);
    Dither[1].setZero(sizePic(0)+2,sizePic(1)+2);
    Dither[2].setZero(sizePic(0)+2,sizePic(1)+2);

    ArrayXXf *ColorMap=nullptr;
    QRgb Current;
    QRgb (*CvtFun)(float,float,float);
    switch (ConvertAlgo) {
    case 'R':
        ColorMap=&Basic._RGB;
        CvtFun=RGB2QRGB;
        break;
    case 'r':
        ColorMap=&Basic._RGB;
        CvtFun=RGB2QRGB;
        break;
    case 'H':
        ColorMap=&Basic.HSV;
        CvtFun=HSV2QRGB;
        break;
    case 'L':
        ColorMap=&Basic.Lab;
        CvtFun=Lab2QRGB;
        break;
    case 'l':
        ColorMap=&Basic.Lab;
        CvtFun=Lab2QRGB;
        break;
    default:
        ColorMap=&Basic.XYZ;
        CvtFun=XYZ2QRGB;
        break;
    }
    ArrayXXf &CM=*ColorMap;
    int index=0;
    for(short r=0;r<sizePic(0);r++)
    {
        for(short c=0;c<sizePic(1);c++)
        {
            Dither[0](r+1,c+1)=R->operator[](rawImage(r,c)).c3[0];
            Dither[1](r+1,c+1)=R->operator[](rawImage(r,c)).c3[1];
            Dither[2](r+1,c+1)=R->operator[](rawImage(r,c)).c3[2];
        }
    }
    qDebug("成功填充了待抖动的矩阵Dither");
    float Error[3];
    int newCount=0;
    TokiColor* oldColor=nullptr;
    for(short r=0;r<sizePic(0);r++)//底部一行、左右两侧不产生误差扩散，只接受误差
    {
        emit keepAwake();
        if(isDirLR)//从左至右遍历
        {
            for(short c=0;c<sizePic(1);c++)
            {
                if(qAlpha(rawImage(r,c))<=0)continue;

                Current=CvtFun(Dither[0](r+1,c+1),Dither[1](r+1,c+1),Dither[2](r+1,c+1));
                if(R->find(Current)==R->end())
                {
                    R->operator[](Current)=TokiColor(Current,ConvertAlgo);
                    R->operator[](Current).apply(Current);
                    //装入了一个新颜色并匹配为地图色
                    newCount++;
                }
                mapPic(r,c)=R->operator[](Current).Result;
                index=mapColor2Index(mapPic(r,c));

                oldColor=&R->operator[](Current);

                Error[0]=oldColor->c3[0]-CM(index,0);
                Error[1]=oldColor->c3[1]-CM(index,1);
                Error[2]=oldColor->c3[2]-CM(index,2);

                Dither[0].block(r+1,c+1-1,2,3)+=Error[0]*DitherMapLR;
                Dither[1].block(r+1,c+1-1,2,3)+=Error[1]*DitherMapLR;
                Dither[2].block(r+1,c+1-1,2,3)+=Error[2]*DitherMapLR;
            }
            //qDebug("从左至右遍历了一行");
        }
        else
        {
            for(short c=sizePic(1)-1;c>=0;c--)
            {
                if(qAlpha(rawImage(r,c))<=0)continue;

                Current=CvtFun(Dither[0](r+1,c+1),Dither[1](r+1,c+1),Dither[2](r+1,c+1));

                if(R->find(Current)==R->end())
                {
                    R->operator[](Current)=TokiColor(Current,ConvertAlgo);
                    R->operator[](Current).apply(Current);
                    //装入了一个新颜色并匹配为地图色
                    newCount++;
                }
                mapPic(r,c)=R->operator[](Current).Result;
                index=mapColor2Index(mapPic(r,c));

                oldColor=&R->operator[](Current);

                Error[0]=oldColor->c3[0]-CM(index,0);
                Error[1]=oldColor->c3[1]-CM(index,1);
                Error[2]=oldColor->c3[2]-CM(index,2);

                Dither[0].block(r+1,c+1-1,2,3)+=Error[0]*DitherMapRL;
                Dither[1].block(r+1,c+1-1,2,3)+=Error[1]*DitherMapRL;
                Dither[2].block(r+1,c+1-1,2,3)+=Error[2]*DitherMapRL;
            }
            //qDebug("从左至右遍历了一行");
        }
        isDirLR=!isDirLR;
        emit convertProgressAdd(sizePic(1));
    }
    qDebug("完成了误差扩散");
    qDebug()<<"Hash中共新插入了"<<newCount<<"个颜色";
}

void matchColor(TokiColor * tColor,QRgb qColor) {
    tColor->apply(qColor);
}

TokiSlopeCraft::ColorSpace TokiSlopeCraft::getColorSpace() const {
    switch (ConvertAlgo) {
    case RGB:
        return R;
    case RGB_Better:
        return R;
    case HSV:
        return H;
    case Lab94:
        return L;
    case Lab00:
        return L;
    case XYZ:
        return X;
    }
}

EImage TokiSlopeCraft::getConovertedImage() const {
EImage cvtedImg(sizePic(0),sizePic(1));
cvtedImg.setZero();
if(kernelStep<converted)
    return cvtedImg;

ArrayXXi RGBint=(255.0f*Basic._RGB).cast<int>();
    short Index;
    for(short r=0;r<sizePic(0);r++)
    {
        for(short c=0;c<sizePic(1);c++)
        {
            if(mapPic(r,c)<=3)
            {
                cvtedImg(r,c)=qRgba(0,0,0,0);
                continue;
            }
            Index=mapColor2Index(mapPic(r,c));

           cvtedImg(r,c)=qRgb(RGBint(Index,0),RGBint(Index,1),RGBint(Index,2));
        }
    }
    return cvtedImg;
}

short TokiSlopeCraft::getImageRows() const {
    if(kernelStep<convertionReady) return -1;
    return rawImage.rows();
}

short TokiSlopeCraft::getImageCols() const {
    if(kernelStep<convertionReady) return -1;
    return rawImage.cols();
}

bool TokiSlopeCraft::build(compressSettings cS, ushort mAH) {
    if(kernelStep<converted)return false;
    if(maxAllowedHeight<2)return false;

    compressMethod=cS;

    maxAllowedHeight=mAH;

    emit buildProgressRangeSet(0,8*sizePic(2),0);

    makeHeight();

    emit buildProgressRangeSet(0,8*sizePic(2),5*sizePic(2));

    buildHeight();

    emit buildProgressRangeSet(0,8*sizePic(2),8*sizePic(2));

    kernelStep=builded;

    return true;
}

void TokiSlopeCraft::makeHeight() {
    Base.setConstant(sizePic(0)+1,sizePic(1),11);

    Base.block(1,0,sizePic(0),sizePic(1))=mapPic/4;

    ArrayXXi dealedDepth;
    ArrayXXi rawShadow=mapPic-4*(mapPic/4);

    if((dealedDepth>=3).any())
    {
        qDebug("错误：Depth中存在深度为3的方块");
        return;
    }
    dealedDepth.setZero(sizePic(0)+1,sizePic(1));
    dealedDepth.block(1,0,sizePic(0),sizePic(1))=rawShadow-1;
    //Depth的第一行没有意义，只是为了保持行索引一致
    WaterList.clear();

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
        emit buildProgressAdd(sizePic(0));
    }

    HighMap.setZero(sizePic(0)+1,sizePic(1));
    LowMap.setZero(sizePic(0)+1,sizePic(1));

    int waterCount=WaterList.size();
    qDebug()<<"共有"<<waterCount<<"个水柱";
    for(short r=0;r<sizePic(0);r++)//遍历每一行，根据高度差构建高度图
    {
        HighMap.row(r+1)=HighMap.row(r)+dealedDepth.row(r+1);
        emit buildProgressAdd(sizePic(0));
    }

    for(short c=0;c<Base.cols();c++)
    {
        if(Base(1,c)==0||Base(1,c)==12||rawShadow(0,c)==2)
        {
            Base(0,c)=0;
            HighMap(0,c)=HighMap(1,c);
        }
        emit buildProgressAdd(sizePic(1));
    }


    LowMap=HighMap;

    for(auto it=WaterList.begin();it!=WaterList.end();it++)
    {
        LowMap(TokiRow(it->first),TokiCol(it->first))=HighMap(TokiRow(it->second),TokiCol(it->second))
                -WaterColumnSize[rawShadow(TokiRow(it->first)-1,TokiCol(it->first))]+1;
    }

    for(short c=0;c<sizePic(1);c++)
    {
        HighMap.col(c)-=LowMap.col(c).minCoeff();
        LowMap.col(c)-=LowMap.col(c).minCoeff();
        //沉降每一列
        emit buildProgressAdd(sizePic(1));
    }

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
            emit buildProgressAdd(sizePic(1));
        }
    }

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
}

void TokiSlopeCraft::buildHeight() {
        Build.resize(size3D[0],size3D[1],size3D[2]);
        Build.setZero();
        //Base(r+1,c)<->High(r+1,c)<->Build(c+1,High(r+1,c),r+1)
        //为了区分玻璃与空气，张量中存储的是Base+1.所以元素为1对应着玻璃，0对应空气
        int x=0,y=0,z=0;
        int yLow=0;
        qDebug()<<"共有"<<WaterList.size()<<"个水柱";
        //qDebug()<<2;
        for(auto it=WaterList.begin();it!=WaterList.end();it++)//水柱周围的玻璃
        {
            x=TokiCol(it->first)+1;
            z=TokiRow(it->first);
            y=waterHigh(it->second);
            yLow=waterLow(it->second);
            Build(x,y+1,z)=0+1;//柱顶玻璃
            for(short yDynamic=yLow;yDynamic<=y;yDynamic++)
            {
                Build(x-1,yDynamic,z-0)=1;
                Build(x+1,yDynamic,z+0)=1;
                Build(x+0,yDynamic,z-1)=1;
                Build(x+0,yDynamic,z+1)=1;
            }
            if(yLow>=1)       Build(x,yLow-1,z)=1;//柱底玻璃
        }
        //qDebug()<<3;

        emit buildProgressAdd(sizePic(2));

        for(short r=0;r<sizePic(0);r++)//普通方块
        {
            for(short c=0;c<sizePic(1);c++)
            {
                if(Base(r+1,c)==12||Base(r+1,c)==0)
                    continue;
                x=c+1;y=HighMap(r+1,c);z=r+1;
                if(y>=1&&blockPalette[Base(r+1,c)].needGlass)
                    Build(x,y-1,z)=0+1;

                Build(x,y,z)=Base(r+1,c)+1;
            }
            emit buildProgressAdd(sizePic(1));
        }

    //qDebug()<<4;

    emit buildProgressAdd(sizePic(2));

    for(auto it=WaterList.begin();it!=WaterList.end();it++)
    {
        x=TokiCol(it->first)+1;
        z=TokiRow(it->first);
        y=waterHigh(it->second);
        yLow=waterLow(it->second);
        for(short yDynamic=yLow;yDynamic<=y;yDynamic++)
        {
            Build(x,yDynamic,z)=13;
        }
    }

    for(short c=0;c<sizePic(1);c++)//北侧方块
        if(Base(0,c))   Build(c+1,HighMap(0,c),0)=11+1;
}


void TokiSlopeCraft::get3DSize(int & x,int & y,int & z) const {
    if(kernelStep<builded)return;
    x=size3D[0];
    y=size3D[1];
    z=size3D[2];
    return;
}
int TokiSlopeCraft::getHeight() const {
    if(kernelStep<builded) return -1;
    return size3D[1];
}
int TokiSlopeCraft::getBlockCounts(vector<int> & dest) const {
    if(kernelStep<builded) return -1;
    dest.resize(64);
    for(int i=0;i<64;i++)
        dest[i]=0;
    for(int i=0;i<Build.size();i++) {
        if(Build(i))
            dest[Build(i)-1]++;
    }
    int totalBlockCount=0;
    for(int i=0;i<64;i++)
        totalBlockCount+=dest[i];
    return totalBlockCount;
}
