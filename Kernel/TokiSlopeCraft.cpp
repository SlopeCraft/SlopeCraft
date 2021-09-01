#include "TokiSlopeCraft.h"
#ifdef WITH_QT
TokiSlopeCraft::TokiSlopeCraft(const vector<string> & paths,QObject *parent) : QObject(parent)
#else
TokiSlopeCraft::TokiSlopeCraft(const vector<string> & paths)
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
if(!readFromTokiColor(paths[0].data(),Basic._RGB)) {
    cerr<<"Failed to read colormap file "<<paths[0]<<", crash the program\n";
    crash();
    return;
}
if(!readFromTokiColor(paths[1].data(),Basic.HSV)) {
    cerr<<"Failed to read colormap file "<<paths[1]<<", crash the program\n";
    crash();
    return;
}
if(!readFromTokiColor(paths[2].data(),Basic.Lab)) {
    cerr<<"Failed to read colormap file "<<paths[2]<<", crash the program\n";
    crash();
    return;
}
if(!readFromTokiColor(paths[3].data(),Basic.XYZ)) {
    cerr<<"Failed to read colormap file "<<paths[3]<<", crash the program\n";
    crash();
    return;
}

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
bool readFromTokiColor(const char*FileName,ArrayXXf & M) {
    fstream Reader;
    Reader.open(FileName, ios::in|ios::binary);
    if(!Reader)return false;

    char * buf=new char[7168];
    Reader.read(buf,7168);
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

    delete[] buf;
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
    if(kernelStep<nothing)return false;
    rawImage=_rawimg;
    mapType=type;
    mcVer=ver;

    blockPalette.resize(64);
    for(short i=0;i<64;i++)
        blockPalette[i]=*palettes;

    ArrayXi baseColorVer(64);//基色对应的版本
    baseColorVer.setConstant(255);
    baseColorVer.segment(0,52).setZero();
    baseColorVer.segment(52,7).setConstant(16);
    baseColorVer.segment(59,3).setConstant(17);

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
//第四步，抖动（包含四个函数）（一次遍历*/


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

ArrayXXi TokiSlopeCraft::getConovertedImage() const {
ArrayXXi cvtedImg(sizePic(0),sizePic(1));
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
