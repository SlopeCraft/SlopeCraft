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

#include "TokiSlopeCraft.h"
#include "zlib.h"

const Eigen::Array<float,2,3>TokiSlopeCraft::DitherMapLR
        ={{0.0/16.0,0.0/16.0,7.0/16.0},
           {3.0/16.0,5.0/16.0,1.0/16.0}};
const Eigen::Array<float,2,3>TokiSlopeCraft::DitherMapRL
        ={{7.0/16.0,0.0/16.0,0.0/16.0},
           {1.0/16.0,5.0/16.0,3.0/16.0}};;
ColorSet TokiSlopeCraft::Basic;
ColorSet TokiSlopeCraft::Allowed;

bool readFromTokiColor(const std::string & FileName,ColorList & M);
bool readFromTokiColor(const char*src,ColorList & M);
uchar h2d(char h);
void crash();

void matchColor(uint32_t taskCount,TokiColor** tk,ARGB * argb);

TokiSlopeCraft::TokiSlopeCraft()
{
    kernelStep=step::nothing;
    rawImage.setZero(0,0);

    glassBuilder=new PrimGlassBuilder;
    Compressor=new LossyCompressor;
#ifdef SLOPECRAFTL_WITH_AICVETR
    AiCvter=AiConverterInterface::create();
#endif
    setProgressRangeSet([](void*,int,int,int){});
    setProgressAdd([](void*,int){});
    setKeepAwake([](void*){});
    setReportError([](void*,errorFlag){});
    setReportWorkingStatue([](void*,workStatues){});
    setAlgoProgressAdd([](void*,int){});
    setAlgoProgressRangeSet([](void*,int,int,int){});

    glassBuilder->windPtr=&wind;
    glassBuilder->progressAddPtr=&this->algoProgressAdd;
    glassBuilder->progressRangeSetPtr=&this->algoProgressRangeSet;
    glassBuilder->keepAwakePtr=&this->keepAwake;

    Compressor->windPtr=&wind;
    Compressor->progressAddPtr=&this->algoProgressAdd;
    Compressor->progressRangeSetPtr=&this->algoProgressRangeSet;
    Compressor->keepAwakePtr=&this->keepAwake;
}

TokiSlopeCraft::~TokiSlopeCraft() {
    delete Compressor;
    delete glassBuilder;
#ifdef SLOPECRAFTL_WITH_AICVETR
    AiCvter->destroy();
#endif
}


///function ptr to window object
void TokiSlopeCraft::setWindPtr(void * _w) {
    wind=_w;
#ifdef SLOPECRAFTL_WITH_AICVETR
    AiCvter->setUiPtr(_w);
#endif
}
///a function ptr to show progress of converting and exporting
void TokiSlopeCraft::setProgressRangeSet(void(*prs)(void*,int,int,int)) {
    progressRangeSet=prs;
#ifdef SLOPECRAFTL_WITH_AICVETR
    AiCvter->setProgressRangeFun(prs);
#endif
}
///a function ptr to add progress value
void TokiSlopeCraft::setProgressAdd(void(*pa)(void*,int)) {
    progressAdd=pa;
#ifdef SLOPECRAFTL_WITH_AICVETR
    AiCvter->setProgressAddFun(pa);
#endif
}
///a function ptr to prevent window from being syncoped
void TokiSlopeCraft::setKeepAwake(void(*ka)(void*)) {
    keepAwake=ka;
}

///a function ptr to show progress of compressing and bridge-building
void TokiSlopeCraft::setAlgoProgressRangeSet(void(*aprs)(void*,int,int,int)) {
    algoProgressRangeSet=aprs;
}
///a function ptr to add progress value of compressing and bridge-building
void TokiSlopeCraft::setAlgoProgressAdd(void(*apa)(void*,int)) {
    algoProgressAdd=apa;
}

///a function ptr to report error when something wrong happens
void TokiSlopeCraft::setReportError(void(*re)(void*,errorFlag)) {
    reportError=re;
}
///a function ptr to report working statue especially when busy
void TokiSlopeCraft::setReportWorkingStatue(void(*rws)(void*,workStatues)) {
    reportWorkingStatue=rws;
}

void TokiSlopeCraft::decreaseStep(step _step) {
    if(kernelStep<=_step)return;
    kernelStep=_step;
}

bool TokiSlopeCraft::setColorSet(const char*R,const char*H,const char*L,const char*X) {
    if(!readFromTokiColor(R,Basic._RGB)) {
        reportError(wind,errorFlag::PARSING_COLORMAP_RGB_FAILED);
        std::cerr<<"Failed to read colormap RGB\n";
        //crash();
        return false;
    }
    if(!readFromTokiColor(H,Basic.HSV)) {
        reportError(wind,errorFlag::PARSING_COLORMAP_HSV_FAILED);
        std::cerr<<"Failed to read colormap HSV\n";
        //crash();
        return false;
    }
    if(!readFromTokiColor(L,Basic.Lab)) {
        reportError(wind,errorFlag::PARSING_COLORMAP_Lab_FAILED);
        std::cerr<<"Failed to read colormap Lab\n";
        //crash();
        return false;
    }
    if(!readFromTokiColor(X,Basic.XYZ)) {
        reportError(wind,errorFlag::PARSING_COLORMAP_XYZ_FAILED);
        std::cerr<<"Failed to read colormap XYZ\n";
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
    std::cerr<<"Wrong byte:"<<(int)h<<"->"<<h;
    return 255;
}
bool readFromTokiColor(const std::string & FileName,ColorList & M) {
    std::fstream Reader;
    Reader.open(FileName, std::ios::in|std::ios::binary);
    if(!Reader)return false;

    char * buf=new char[7168];
    Reader.read(buf,7168);
    bool result=readFromTokiColor(buf,M);
    delete[] buf;
    Reader.close();
    return result;
}

bool readFromTokiColor(const char*src,ColorList & M) {
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
    //qDebug()<<"TokiSlopeCraft::成功加载了一个颜色表";
    return true;
}


#define bufferSize 2048
bool compressFile(const char*sourcePath,const char*destPath) {
    char buf[bufferSize]={0};
    FILE * in=nullptr;
    gzFile out=nullptr;
    int len=0;
    fopen_s(&in,sourcePath,"rb");
    out=gzopen(destPath,"wb");
    if(in==nullptr||out==nullptr)
        return false;
    while(true)
    {
        len=(int)fread(buf,1,sizeof(buf),in);
        if(ferror(in))return false;
        if(len==0)break;
        if(len!=gzwrite(out,buf,(unsigned)len))
            return false;
        memset(buf,0,sizeof(buf));
    }
    fclose(in);
    gzclose(out);
    //succeed
    return true;
}


void TokiSlopeCraft::makeTests(const AbstractBlock ** src,
                               const unsigned char * baseColor,
                               const char * dst,char * _unFileName) {
    if(kernelStep<step::wait4Image) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);

        if(_unFileName!=nullptr)
            std::strcpy(_unFileName,"");
        return;
    }

    std::string s=makeTests(src,baseColor,std::string(dst));
    if(_unFileName!=nullptr)
        std::strcpy(_unFileName,s.data());
}

std::string TokiSlopeCraft::makeTests(const AbstractBlock ** src,
                               const uint8_t * src_baseColor,
                               const std::string & fileName) {

    if(kernelStep<step::wait4Image) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return "";
    }

    if(fileName.find_last_of(".nbt")==std::string::npos) {
        return "";
    }

    //const simpleBlock ** realSrc=(const simpleBlock **)src;
    std::vector<const simpleBlock *> realSrc;
    std::vector<uint8_t> realBaseColor;
    realSrc.clear();
    realBaseColor.clear();
    for(uint32_t idx=0;src[idx]!=nullptr;idx++) {
        if(src[idx]->getVersion()>mcVer) {
            continue;
        }
        realSrc.emplace_back((const simpleBlock*)src[idx]);
        realBaseColor.emplace_back(src_baseColor[idx]);
    }

    std::vector<std::vector<int>> blocks;
    blocks.resize(64);

    for(auto & it : blocks) {
        while(!it.empty()) {
            it.clear();
            //it.reserve(16);
        }
    }
    for(uint32_t idx=0;idx<realSrc.size();idx++) {
        blocks[realBaseColor[idx]].push_back(idx);
    }


    int xSize=0;
    static const int zSize=64,ySize=1;
    for(const auto & it : blocks) {
        xSize=std::max(size_t(xSize),it.size());
    }

    NBT::NBTWriter file;
    std::string unCompress=fileName+".TokiNoBug";
        file.open(unCompress.data());
        file.writeListHead("entities",NBT::idByte,0);
        file.writeListHead("size",NBT::idInt,3);
        file.writeInt("This should never be shown",xSize);
        file.writeInt("This should never be shown",ySize);
        file.writeInt("This should never be shown",zSize);
        file.writeListHead("palette",NBT::idCompound,realSrc.size()+1);
        {
            std::vector<std::string> ProName,ProVal;
            std::string netBlockId;
            simpleBlock::dealBlockId("minecraft:air",netBlockId,&ProName,&ProVal);
            writeBlock(netBlockId,ProName,ProVal,file);
            for(const auto & it : blocks) {
                for(const auto jt : it) {
                    simpleBlock::dealBlockId(
                                (mcVer>gameVersion::MC12)?
                                    (realSrc[jt]->id):(realSrc[jt]->idOld),
                        netBlockId,&ProName,&ProVal);
                    writeBlock(netBlockId,ProName,ProVal,file);
                }
            }
            file.writeListHead("blocks",NBT::idCompound,realSrc.size());

            for(uint8_t base=0;base<64;base++) {
                for(uint32_t idx=0;idx<blocks[base].size();idx++) {
                    int xPos=idx;
                    int yPos=0;
                    int zPos=base;
                    file.writeCompound("This should never be shown");
                    file.writeListHead("pos",NBT::idInt,3);
                    file.writeInt("This should never be shown",xPos);
                    file.writeInt("This should never be shown",yPos);
                    file.writeInt("This should never be shown",zPos);
                    file.writeInt("state",blocks[base][idx]+1);
                    file.endCompound();
                }
            }

        }
                switch (mcVer)
                {
                case MC12:
                    file.writeInt("DataVersion",Kernel::mcVersion2VersionNumber(mcVer));
                    break;
                case MC13:
                    file.writeInt("DataVersion",Kernel::mcVersion2VersionNumber(mcVer));
                    break;
                case MC14:
                    file.writeInt("DataVersion",Kernel::mcVersion2VersionNumber(mcVer));
                    break;
                case MC15:
                    file.writeInt("DataVersion",Kernel::mcVersion2VersionNumber(mcVer));
                    break;
                case MC16:
                    file.writeInt("DataVersion",Kernel::mcVersion2VersionNumber(mcVer));
                    break;
                case MC17:
                    file.writeInt("DataVersion",Kernel::mcVersion2VersionNumber(mcVer));
                    break;
                case MC18:
                    file.writeInt("DataVersion",Kernel::mcVersion2VersionNumber(mcVer));
                    break;
                default:
                    std::cerr<<"Wrong game version!\n";
                    break;
                }
    file.close();

    if(!compressFile(unCompress.data(),fileName.data())) {
        reportError(wind,errorFlag::FAILED_TO_COMPRESS);
        return unCompress;
    }

    if(std::remove(unCompress.data())!=0) {
        reportError(wind,errorFlag::FAILED_TO_REMOVE);
        return unCompress;
    }

    return fileName;
}

step TokiSlopeCraft::queryStep() const {
    return kernelStep;
}

void TokiSlopeCraft::setAiCvterOpt(const AiCvterOpt * _a) {
    AiOpt=*_a;
}

const AiCvterOpt * TokiSlopeCraft::aiCvterOpt() const {
    return &AiOpt;
}

bool TokiSlopeCraft::setType(mapTypes type,
                             gameVersion ver,
                             const bool * allowedBaseColor,
                             const AbstractBlock ** palettes) {

    if(kernelStep<colorSetReady) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return false;
    }

    mapType=type;
    mcVer=ver;

    TokiColor::needFindSide=(mapType==mapTypes::Slope);

    blockPalette.resize(64);

    //std::cerr<<__FILE__<<__LINE__<<std::endl;
    for(short i=0;i<64;i++) {
        //std::cerr<<"Block_"<<i<<std::endl;
        if(palettes[i]==nullptr) {
            blockPalette[i].clear();
            continue;
        }
        palettes[i]->copyTo(&blockPalette[i]);

        if(blockPalette[i].id.find(':')==blockPalette[i].id.npos) {
            blockPalette[i].id="minecraft:"+blockPalette[i].id;
        }
        if(blockPalette[i].idOld.size()>0
                &&(blockPalette[i].idOld.find(':')==blockPalette[i].idOld.npos)) {
            blockPalette[i].idOld="minecraft:"+blockPalette[i].idOld;
        }
    }

    //std::cerr<<__FILE__<<__LINE__<<std::endl;

    reportWorkingStatue(wind,workStatues::collectingColors);

    Eigen::ArrayXi baseColorVer(64);//基色对应的版本
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
        if((mapType==mapTypes::Wall)
                &&!blockPalette[index2baseColor(index)].wallUseable) {//墙面像素画且当前方块不适合墙面
            MIndex[index]=false;
            continue;
        }
        if(isVanilla()&&(index2depth(index)>=3)) {//可实装的地图画不允许第四种阴影
            MIndex[index]=false;
            continue;
        }
        if(index2baseColor(index)==12&&mapType!=mapTypes::Wall) {//如果是水且非墙面
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

    if(!Allowed.ApplyAllowed(&Basic,MIndex)) {
        reportError(wind,errorFlag::USEABLE_COLOR_TOO_FEW);
        return false;
    }

    reportWorkingStatue(wind,workStatues::none);

    kernelStep=wait4Image;
    return true;
}

bool TokiSlopeCraft::setType(mapTypes type,
                             gameVersion ver,
                             const bool * allowedBaseColor,
                             const simpleBlock * palettes) {
    const AbstractBlock * temp[64];
    for(uint16_t idx=0;idx<64;idx++) {
        temp[idx]=palettes+idx;
    }

    return setType(type,ver,allowedBaseColor,temp);

}

#ifdef SLOPECRAFTL_WITH_AICVETR
void TokiSlopeCraft::configAiCvter() {
    AiCvter->loadColorSheet(Allowed._RGB.data(),
                            Allowed.Map.data(),
                            Allowed.Map.size());
    AiCvter->loadImage(rawImage.data(),rawImage.rows(),rawImage.cols());
}
#endif

uint16_t TokiSlopeCraft::getColorCount() const {
    if(kernelStep<wait4Image) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return 0;
    }
    return Allowed.colorCount();
}

void TokiSlopeCraft::setRawImage(const ARGB * src, short rows,short cols) {
    setRawImage(EImage::Map(src,rows,cols));
}

void TokiSlopeCraft::setRawImage(const EImage & _rawimg) {
    if(kernelStep<wait4Image) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return;
    }
    if(_rawimg.size()<=0) {
        reportError(wind,errorFlag::EMPTY_RAW_IMAGE);
        return;
    }

    rawImage=_rawimg;
    kernelStep=convertionReady;
#ifdef SLOPECRAFTL_WITH_AICVETR
    configAiCvter();
#endif
    return;
}

bool TokiSlopeCraft::isVanilla() const {
    return mapType!=FileOnly;
}

bool TokiSlopeCraft::isFlat() const {
    return mapType==Flat||mapType==Wall;
}

void TokiSlopeCraft::getAuthorURL(int * count,char **dest) const {
    std::vector<std::string> result=getAuthorURL();
    for(uint16_t i=0;i<result.size();i++) {
        if(dest!=nullptr&&dest[i]!=nullptr)
            std::strcpy(dest[i],result[i].data());
    }
    if(count!=nullptr)
        *count=result.size();
}

std::vector<std::string> TokiSlopeCraft::getAuthorURL() const {
    if(kernelStep<colorSetReady) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        std::vector<std::string> error(2);
        error[0]="Too hasty operation!";
        error[1]="make sure that you've deployed the colormap!";
        return error;
    }
    std::vector<std::string> urls(2);
    static std::string Toki="";
        if(Toki=="")
        {
            const short size3D[]={1229, 150, 150, 44, 40, 69, 204, 204, 376, 114, 150, 1229, 598, 182, 142, 173, 110, 238, 204, 132, 110, 117, 114, 882, 110, 7, 598, 376, 204, 101, 166, 110, 44, 364, 870, 169, 922, 134, 150,};
            Toki=Noder(size3D,sizeof(size3D)/2);
        }
        static std::string Doki="";
            if(Doki=="")
            {
                const short sizePic[]={1229, 150, 150, 44, 40, 69, 204, 204, 40, 44, 922, 173, 364, 142, 182, 114, 166, 114, 182, 114, 166, 114, 142, 173, 110, 238, 204, 80, 218, 380, 56, 28, 286, 28, 80, 380};
                Doki=this->Noder(sizePic,sizeof(sizePic)/2);
            }
            urls[0]=Toki;
            urls[1]=Doki;
            return urls;
}

std::string TokiSlopeCraft::Noder(const short *src,int size) const {
    std::string dst;
        char*u=nullptr;
        for(int i=0;i<size;i++)
        {
            u=(char*)(&Basic.Lab(src[i]/4))+src[i]%4;
            dst.push_back(*u);
        }
        return dst;
}

void TokiSlopeCraft::getARGB32(ARGB * dest) const {
    if(kernelStep<colorSetReady) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return;
    }
    if(dest==nullptr) return;

    for(uchar base=0;base<64;base++)
        dest[base]=ARGB32(255*Basic._RGB(128+base,0),
                         255*Basic._RGB(128+base,1),
                         255*Basic._RGB(128+base,2),255
                         );
}

bool TokiSlopeCraft::convert(convertAlgo algo,bool dither) {
    if(kernelStep<convertionReady) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return false;
    }

    if(algo==convertAlgo::AiCvter) {
#ifdef SLOPECRAFTL_WITH_AICVETR
        convertAlgo algos[6]={RGB,RGB_Better,HSV,Lab94,Lab00,XYZ};
        std::array<const uint8_t *,6> seed;
        Eigen::ArrayXX<uint8_t> CvtedMap[6];
        for(int a=0;a<6;a++) {
            this->convert(algos[a]);
            CvtedMap[a].resize(getImageRows(),getImageCols());
            this->getConvertedMap(nullptr,nullptr,CvtedMap[a].data());
            seed[a]=CvtedMap[a].data();
        }

        AiCvter->setCrossoverProb(AiOpt.crossoverProb);
        AiCvter->setMutatteProb(AiOpt.mutationProb);
        AiCvter->setMaxGeneration(AiOpt.maxGeneration);
        AiCvter->setMaxFailTime(AiOpt.maxFailTimes);
        AiCvter->setPopSize(AiOpt.popSize);
        AiCvter->setSeed(seed.data(),seed.size());

        AiCvter->run();

        //replace raw image with ai result
        AiCvter->resultImage(rawImage.data());

        algo=convertAlgo::RGB_Better;

#else
        algo=convertAlgo::RGB_Better;
#endif
    }

    ConvertAlgo=algo;
    colorHash.clear();

    progressRangeSet(wind,0,4*sizePic(2),0);

/*
//第一步，装入hash顺便转换颜色空间;（一次遍历
//第二步，遍历hash并匹配颜色;（一次遍历
//第三步，从hash中检索出对应的匹配结果;（一次遍历
//第四步，抖动（一次遍历*/

    reportWorkingStatue(wind,workStatues::collectingColors);
    pushToHash();

    keepAwake(wind);
    progressRangeSet(wind,0,4*sizePic(2),1*sizePic(2));

    reportWorkingStatue(wind,workStatues::converting);
    applyTokiColor();

    keepAwake(wind);
    progressRangeSet(wind,0,4*sizePic(2),2*sizePic(2));

    fillMapMat();
    keepAwake(wind);
    progressRangeSet(wind,0,4*sizePic(2),3*sizePic(2));

    ditheredImage=this->rawImage;

    if(dither) {
        reportWorkingStatue(wind,workStatues::dithering);
        Dither();
    }
    progressRangeSet(wind,0,4*sizePic(2),4*sizePic(2));
    keepAwake(wind);

    reportWorkingStatue(wind,workStatues::none);

    kernelStep=converted;
    return true;
}

int TokiSlopeCraft::sizePic(short dim) const {
    if(dim==0) return rawImage.rows();
    if(dim==1) return rawImage.cols();
    return rawImage.size();
}

void TokiSlopeCraft::pushToHash() {
    auto R=&colorHash;
    R->clear();
        TokiColor::Allowed=&Allowed;
        TokiColor::Basic=&Basic;

        char Mode=ConvertAlgo;
        TokiColor::convertAlgo=Mode;

        //R->reserve(sizePic(2)/4);

        for(uint32_t idx=0;idx<rawImage.size();idx++) {
            R->emplace(rawImage(idx),rawImage(idx));

            if((idx/sizePic(1))%reportRate==0) {
                progressAdd(wind,reportRate*sizePic(1));
            }

        }

        std::cerr<<"Total color count:"<<R->size()<<std::endl;
}

void TokiSlopeCraft::applyTokiColor() {
    static const uint64_t threadCount=std::thread::hardware_concurrency();
    const uint64_t taskCount=colorHash.size();

    //int step=threadCount*sizePic(2)/taskCount;
    //int step=threadCount*sizePic(2)/reportRate;

    std::vector<std::pair<const ARGB,TokiColor>*> tasks;

    tasks.clear();
    tasks.reserve(taskCount);

    for(auto it=colorHash.begin();it!=colorHash.end();++it) {
        tasks.emplace_back(&*it);
    }

    static const std::clock_t interval=1*CLOCKS_PER_SEC;
    std::clock_t prevClock=std::clock();
#pragma omp parallel for
    for(uint64_t begIdx=0;begIdx<threadCount;begIdx++) {
        //
        for(uint64_t idx=begIdx;idx<taskCount;idx+=threadCount) {
            tasks[idx]->second.apply(tasks[idx]->first);
            if(idx%reportRate==0) {
                std::clock_t curClock=std::clock();
                if(curClock-prevClock>interval) {
                    prevClock=curClock;
                    progressRangeSet(wind,0,4*sizePic(2),sizePic(2)+idx);
                }
            }
        }
    }

    std::cerr<<"colors converted\n";
}

void TokiSlopeCraft::fillMapMat() {
    mapPic.setZero(getImageRows(),getImageCols());
    //auto R=&colorHash;
        for(short r=0;r<sizePic(0);r++) {
            for(short c=0;c<sizePic(1);c++) {
                mapPic(r,c)=colorHash[rawImage(r,c)].Result;
            }

            if(r%reportRate==0)
                progressAdd(wind,reportRate*sizePic(1));
        }
}

void TokiSlopeCraft::Dither() {
    //colorHash.reserve(3*colorHash.size());
    auto R=&colorHash;
    Eigen::ArrayXXf Dither[3];
    /*
    cout<<"DitherMapLR="<<endl;
    cout<<DitherMapLR<<endl;
    cout<<"DitherMapRL="<<endl;
    cout<<DitherMapRL<<endl;*/
    bool isDirLR=true;
    Dither[0].setZero(sizePic(0)+2,sizePic(1)+2);
    Dither[1].setZero(sizePic(0)+2,sizePic(1)+2);
    Dither[2].setZero(sizePic(0)+2,sizePic(1)+2);

    ditheredImage.setZero(sizePic(0),sizePic(1));

    ColorList *ColorMap=nullptr;
    ARGB Current;
    ARGB (*CvtFun)(float,float,float);
    switch (ConvertAlgo) {
    case 'R':
    case 'r':
        ColorMap=&Basic._RGB;
        CvtFun=RGB2ARGB;
        break;
    case 'H':
        ColorMap=&Basic.HSV;
        CvtFun=HSV2ARGB;
        break;
    case 'L':
    case 'l':
        ColorMap=&Basic.Lab;
        CvtFun=Lab2ARGB;
        break;
    default:
        ColorMap=&Basic.XYZ;
        CvtFun=XYZ2ARGB;
        break;
    }
    ColorList &CM=*ColorMap;

    //int t=sizeof(Eigen::Array3f);

    int index=0;
    for(short r=0;r<sizePic(0);r++)
    {
        for(short c=0;c<sizePic(1);c++)
        {
            TokiColor && temp=std::move(R->operator[](rawImage(r,c)));
            Dither[0](r+1,c+1)=temp.c3[0];
            Dither[1](r+1,c+1)=temp.c3[1];
            Dither[2](r+1,c+1)=temp.c3[2];
        }
    }
    std::cerr<<"Filled Dither matrix\n";
    int newCount=0;
    //TokiColor* oldColor=nullptr;
    for(short r=0;r<sizePic(0);r++)//底部一行、左右两侧不产生误差扩散，只接受误差
    {
        if(isDirLR)//从左至右遍历
        {
            for(short c=0;c<sizePic(1);c++)
            {
                //float Error[3];
                Eigen::Array3f Error;
                if(getA(rawImage(r,c))<=0)continue;

                Current=CvtFun(Dither[0](r+1,c+1),Dither[1](r+1,c+1),Dither[2](r+1,c+1));
                ditheredImage(r,c)=Current;

                auto find=R->find(Current);

                if(find==R->end())
                {
                    R->emplace(Current,Current);
                    find=R->find(Current);
                    find->second.apply(Current);
                    //装入了一个新颜色并匹配为地图色
                    newCount++;
                }
                TokiColor & oldColor=find->second;
                mapPic(r,c)=oldColor.Result;
                index=mapColor2Index(mapPic(r,c));

                Error=oldColor.c3-CM.row(index).transpose();
                /*
                Error[0]=oldColor->c3[0]-CM(index,0);
                Error[1]=oldColor->c3[1]-CM(index,1);
                Error[2]=oldColor->c3[2]-CM(index,2);
                */

                Dither[0].block<2,3>(r+1,c+1-1)+=Error[0]*DitherMapLR;
                Dither[1].block<2,3>(r+1,c+1-1)+=Error[1]*DitherMapLR;
                Dither[2].block<2,3>(r+1,c+1-1)+=Error[2]*DitherMapLR;
            }
        }
        else
        {
            for(short c=sizePic(1)-1;c>=0;c--)
            {
                //float Error[3];
                Eigen::Array3f Error;
                if(getA(rawImage(r,c))<=0)continue;

                Current=CvtFun(Dither[0](r+1,c+1),Dither[1](r+1,c+1),Dither[2](r+1,c+1));
                ditheredImage(r,c)=Current;

                auto find=R->find(Current);
                if(find==R->end())
                {
                    R->emplace(Current,Current);
                    find=R->find(Current);
                    find->second.apply(Current);
                    //装入了一个新颜色并匹配为地图色
                    newCount++;
                }
                TokiColor & oldColor=find->second;
                mapPic(r,c)=oldColor.Result;
                index=mapColor2Index(mapPic(r,c));

                Error=oldColor.c3-CM.row(index).transpose();
                /*
                Error[0]=oldColor->c3[0]-CM(index,0);
                Error[1]=oldColor->c3[1]-CM(index,1);
                Error[2]=oldColor->c3[2]-CM(index,2);
                */

                Dither[0].block<2,3>(r+1,c+1-1)+=Error[0]*DitherMapRL;
                Dither[1].block<2,3>(r+1,c+1-1)+=Error[1]*DitherMapRL;
                Dither[2].block<2,3>(r+1,c+1-1)+=Error[2]*DitherMapRL;
            }
        }
        isDirLR=!isDirLR;
        if(r%reportRate==0) {
            keepAwake(wind);
            progressAdd(wind,reportRate*sizePic(1));
        }
    }
    std::cerr<<"Error diffuse finished\n";
    std::cerr<<"Inserted "<<newCount<<" colors to hash\n";
}


#ifdef WITH_QT
void matchColor(TokiColor * tColor,ARGB qColor) {
    tColor->apply(qColor);
}
#else
void matchColor(uint32_t taskCount,TokiColor** tk,ARGB * argb) {
    for(uint32_t i=0;i<taskCount;i++) {
        tk[i]->apply(argb[i]);
    }
}
#endif

void TokiSlopeCraft::getTokiColorPtr(uint16_t col, const TokiColor ** dst) const {
    if(kernelStep<converted) {
        std::cerr<<"Too hasty! export after you converted the map!"<<std::endl;
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return ;
    }
    for(uint16_t r=0;r<ditheredImage.rows();r++) {
        auto i=colorHash.find(ditheredImage(r,col));

        if(i==colorHash.end())
            dst[r]=nullptr;
        else
            dst[r]=(const TokiColor*)&(colorHash.at(ditheredImage(r,col)));
    }
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
    case convertAlgo::Lab00:
        return L;
    case XYZ:
        return X;
    case convertAlgo::AiCvter:
        return R;
    }
    return R;
}

void TokiSlopeCraft::getConvertedImage(short * rows,short * cols,ARGB * dest) const {
    EImage result=getConovertedImage();
    if(rows!=nullptr)
        *rows=result.rows();
    if(cols!=nullptr)
        *cols=result.cols();
    if(dest!=nullptr)
        for(uint32_t idx=0;idx<result.size();idx++) {
            dest[idx]=result(idx);
        }
}

EImage TokiSlopeCraft::getConovertedImage() const {
EImage cvtedImg(sizePic(0),sizePic(1));
cvtedImg.setZero();
if(kernelStep<converted) {
    reportError(wind,errorFlag::HASTY_MANIPULATION);
    return cvtedImg;
}
Eigen::ArrayXXi RGBint=(255.0f*Basic._RGB).cast<int>();
RGBint=(RGBint>255).select(Eigen::ArrayXXi::Constant(256,3,255),RGBint);
    short Index;
    for(short r=0;r<sizePic(0);r++)
    {
        for(short c=0;c<sizePic(1);c++)
        {
            if(mapPic(r,c)<=3)
            {
                cvtedImg(r,c)=ARGB32(0,0,0,0);
                continue;
            }
            Index=mapColor2Index(mapPic(r,c));

           cvtedImg(r,c)=ARGB32(RGBint(Index,0),RGBint(Index,1),RGBint(Index,2));
        }
    }
    return cvtedImg;
}

void TokiSlopeCraft::getConvertedMap
    (short * rows,short * cols,unsigned char * dst) const {
    if(rows!=nullptr) {
        *rows=getImageRows();
    }
    if(cols!=nullptr) {
        *cols=getImageCols();
    }

    Eigen::Map<Eigen::Array<uint8_t,Eigen::Dynamic,Eigen::Dynamic>>
            dest(dst,getImageRows(),getImageCols());

    for(int r=0;r<getImageRows();r++) {
        for(int c=0;c<getImageCols();c++) {
            dest(r,c)=colorHash.find(ditheredImage(r,c))->second.Result;
        }
    }
}

short TokiSlopeCraft::getImageRows() const {
    if(kernelStep<convertionReady) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return -1;
    }
    return rawImage.rows();
}

short TokiSlopeCraft::getImageCols() const {
    if(kernelStep<convertionReady) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return -1;
    }
    return rawImage.cols();
}

void TokiSlopeCraft::exportAsData(const char * FolderPath,
                                  const int indexStart,
                                  int* fileCount,
                                  char ** dest) const {
    std::vector<std::string> uFL=exportAsData(FolderPath,indexStart);
    if(fileCount!=nullptr)
        *fileCount=uFL.size();
    if(dest!=nullptr)
        for(uint16_t i=0;i<uFL.size();i++) {
            if(dest[i]!=nullptr)
                std::strcpy(dest[i],uFL[i].data());
        }
}

std::vector<std::string> TokiSlopeCraft::exportAsData(const std::string & FolderPath ,
                                            int indexStart) const {
    std::vector<std::string> unCompressedFileList;
    unCompressedFileList.clear();
    std::vector<std::string> compressedFileList;
    compressedFileList.clear();

    if(kernelStep<converted) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        unCompressedFileList.push_back("Too hasty! export after you converted the map!");
        return unCompressedFileList;
    }

    const int rows=ceil(mapPic.rows()/128.0f);
    const int cols=ceil(mapPic.cols()/128.0f);
    //const int maxrr=rows*128;
    //const int maxcc=cols*128;
    progressRangeSet(wind,0,128*rows*cols,0);

    int offset[2]={0,0};//r,c
    int currentIndex=indexStart;

    reportWorkingStatue(wind,workStatues::writingMapDataFiles);

    for(int c=0;c<cols;c++)
    {
        for(int r=0;r<rows;r++)
        {
            offset[0]=r*128;
            offset[1]=c*128;
            std::string currentCn=FolderPath+"/map_"+std::to_string(currentIndex)+".dat";
            std::string currentUn=currentCn+".TokiNoBug";
            //string currentFile=FolderPath+"/map_"+std::to_string(currentIndex)+".dat";

            std::cerr<<"Export map of ("<<r<<","<<c<<")"<<currentUn<<std::endl;

            NBT::NBTWriter MapFile(currentUn.data());

            switch (mcVer)
            {
            case MC12:
                break;
            case MC13:
                break;
            case MC14:
                MapFile.writeInt("DataVersion",mcVersion2VersionNumber(mcVer));
                break;
            case MC15:
                MapFile.writeInt("DataVersion",mcVersion2VersionNumber(mcVer));
                break;
            case MC16:
                MapFile.writeInt("DataVersion",mcVersion2VersionNumber(mcVer));
                break;
            case MC17:
                MapFile.writeInt("DataVersion",mcVersion2VersionNumber(mcVer));
                break;
            case MC18:
                MapFile.writeInt("DataVersion",mcVersion2VersionNumber(mcVer));
                break;
            default:
                std::cerr<<"Wrong game version!\n";
                break;
            }
            static const std::string ExportedBy="Exported by SlopeCraft "+std::string(Kernel::getSCLVersion())+", developed by TokiNoBug";
            MapFile.writeString("ExportedBy",ExportedBy.data());
            MapFile.writeCompound("data");
                MapFile.writeByte("scale",0);
                MapFile.writeByte("trackingPosition",0);
                MapFile.writeByte("unlimitedTracking",0);
                MapFile.writeInt("xCenter",0);
                MapFile.writeInt("zCenter",0);
                switch(mcVer)
                {
                case MC12:
                    MapFile.writeByte("dimension",114);
                    MapFile.writeShort("height",128);
                    MapFile.writeShort("width",128);
                    break;
                case MC13:
                    MapFile.writeListHead("banners",NBT::idCompound,0);
                    MapFile.writeListHead("frames",NBT::idCompound,0);
                    MapFile.writeInt("dimension",889464);
                    break;
                case MC14:
                    MapFile.writeListHead("banners",NBT::idCompound,0);
                    MapFile.writeListHead("frames",NBT::idCompound,0);
                    MapFile.writeInt("dimension",0);
                    MapFile.writeByte("locked",1);
                    break;
                case MC15:
                    MapFile.writeListHead("banners",NBT::idCompound,0);
                    MapFile.writeListHead("frames",NBT::idCompound,0);
                    MapFile.writeInt("dimension",0);
                    MapFile.writeByte("locked",1);
                    break;
                case MC16:
                    MapFile.writeListHead("banners",NBT::idCompound,0);
                    MapFile.writeListHead("frames",NBT::idCompound,0);
                    MapFile.writeString("dimension","minecraft:overworld");
                    MapFile.writeByte("locked",1);
                    break;
                case MC17:
                    MapFile.writeListHead("banners",NBT::idCompound,0);
                    MapFile.writeListHead("frames",NBT::idCompound,0);
                    MapFile.writeString("dimension","minecraft:overworld");
                    MapFile.writeByte("locked",1);
                    break;
                case MC18:
                    MapFile.writeListHead("banners",NBT::idCompound,0);
                    MapFile.writeListHead("frames",NBT::idCompound,0);
                    MapFile.writeString("dimension","minecraft:overworld");
                    MapFile.writeByte("locked",1);
                    break;
                default:
                    std::cerr<<"Wrong game version!\n";
                    break;
                }

                MapFile.writeByteArrayHead("colors",16384);
                uchar ColorCur=0;
                    for(short rr=0;rr<128;rr++)
                    {
                        for(short cc=0;cc<128;cc++)
                        {
                            if(rr+offset[0]<mapPic.rows()&&cc+offset[1]<mapPic.cols())
                                ColorCur=mapPic(rr+offset[0],cc+offset[1]);
                            else
                                ColorCur=0;
                            MapFile.writeByte("this should never be seen",ColorCur);
                        }
                        progressAdd(wind,1);
                    }
                MapFile.endCompound();
                MapFile.close();
                unCompressedFileList.emplace_back(currentUn);
                compressedFileList.emplace_back(currentCn);
                /*
                if(compressFile(currentUn.data(),currentFile.data()))
                {
                    qDebug("压缩成功");
                    QFile umComFile(QString::fromStdString(currentUn));
                    umComFile.remove();
                }*/
                currentIndex++;
        }
    }

    reportWorkingStatue(wind,workStatues::none);

    for(uint32_t i=0;i<compressedFileList.size();i++) {
        bool success=true;
        success=success&&
                compressFile(unCompressedFileList[i].data(),
                                      compressedFileList[i].data());
        if(!success) {
            reportError(wind,errorFlag::FAILED_TO_COMPRESS);
            continue;
        }
        success=success&&(std::remove(unCompressedFileList[i].data())==0);
        if(!success) {
            reportError(wind,errorFlag::FAILED_TO_REMOVE);
            continue;
        }
    }

    return compressedFileList;
}

bool TokiSlopeCraft::build(compressSettings cS, uint16_t mAH,
                           glassBridgeSettings gBS,uint16_t bI,
                           bool fireProof,bool endermanProof) {
    if(kernelStep<converted){
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        std::cerr<<"hasty!"<<std::endl;
        return false;
    }
    if(mAH<14){
        std::cerr<<"maxAllowedHeight<14!"<<std::endl;
        reportError(wind,errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14);
        return false;
    }
    std::cerr<<"ready to build"<<std::endl;


    compressMethod=cS;
    glassMethod=gBS;
    if(isFlat()||!isVanilla()) {
        compressMethod=compressSettings::noCompress;
        glassMethod=glassBridgeSettings::noBridge;
    }

    maxAllowedHeight=mAH;
    bridgeInterval=bI;

    reportWorkingStatue(wind,workStatues::buidingHeighMap);

    progressRangeSet(wind,0,9*sizePic(2),0);
    std::cerr<<"start makeHeight"<<std::endl;

    mapPic.setZero(sizePic(0),sizePic(1));
    for(uint16_t r=0;r<sizePic(0);r++) {
        for(uint16_t c=0;c<sizePic(1);c++) {
            mapPic(r,c)=colorHash[ditheredImage(r,c)].Result;
        }
        progressAdd(wind,sizePic(1));
    }

    makeHeight_new();
    std::cerr<<"makeHeight finished"<<std::endl;
    progressRangeSet(wind,0,9*sizePic(2),5*sizePic(2));

    reportWorkingStatue(wind,workStatues::building3D);
    std::cerr<<"start buildHeight"<<std::endl;
    buildHeight(fireProof,endermanProof);
    std::cerr<<"buildHeight finished"<<std::endl;
    progressRangeSet(wind,0,9*sizePic(2),8*sizePic(2));

    reportWorkingStatue(wind,workStatues::constructingBridges);
    std::cerr<<"start makeBridge"<<std::endl;
    makeBridge();
    std::cerr<<"makeBridge finished"<<std::endl;
    progressRangeSet(wind,0,9*sizePic(2),9*sizePic(2));

    if(mapType==mapTypes::Wall) {
        reportWorkingStatue(wind,workStatues::flippingToWall);
        Eigen::Tensor<uchar,3> temp=Eigen::Tensor<uchar,3>(Build);
        Eigen::array<int,3> perm={1,2,0};
        Build=temp.shuffle(perm);

        for(uint16_t x=0;x<Build.dimension(0);x++)
            for(uint16_t z=0;z<Build.dimension(2);z++)
                for(uint16_t y=0;y<Build.dimension(1)/2;y++) {
                    std::swap(Build(x,y,z),Build(x,Build.dimension(1)-y-1,z));
                }


        size3D[0]=Build.dimension(0);
        size3D[1]=Build.dimension(1);
        size3D[2]=Build.dimension(2);
    }

    reportWorkingStatue(wind,workStatues::none);

    kernelStep=builded;

    return true;
}

void TokiSlopeCraft::makeHeight_new() {
    Base.setZero(sizePic(0)+1,sizePic(1));
    WaterList.clear();
    HighMap.setZero(sizePic(0)+1,sizePic(1));
    LowMap.setZero(sizePic(0)+1,sizePic(1));
    bool allowNaturalCompress=
            compressMethod==compressSettings::Both
            ||compressMethod==compressSettings::NaturalOnly;
    //std::vector<const TokiColor*> src;
    std::cerr<<"makeHeight_new\n";

    if((mapPic-4*(mapPic/4)>=3).any()) {
        reportError(wind,errorFlag::DEPTH_3_IN_VANILLA_MAP);
        return;
    }

    for(uint16_t c=0;c<sizePic(1);c++) {

        std::cerr<<"Coloumn "<<c<<'\n';
        HeightLine HL;
        //getTokiColorPtr(c,&src[0]);
        HL.make(mapPic.col(c),allowNaturalCompress);

        if(HL.maxHeight()>maxAllowedHeight&&
                (compressMethod==compressSettings::ForcedOnly||
                compressMethod==compressSettings::Both)) {

            std::vector<const TokiColor*> ptr(getImageRows());

            getTokiColorPtr(c,&ptr[0]);

            Compressor->setSource(HL.getBase(),&ptr[0]);
            bool success=Compressor->compress(maxAllowedHeight,
                                              allowNaturalCompress);
            if(!success) {
                reportError(wind,LOSSYCOMPRESS_FAILED);
                return;
            }
            Eigen::ArrayXi temp;
            HL.make(&ptr[0],Compressor->getResult(),
                    allowNaturalCompress,&temp);
            mapPic.col(c)=temp;

        }

        Base.col(c)=HL.getBase();
        HighMap.col(c)=HL.getHighLine();
        LowMap.col(c)=HL.getLowLine();
        auto HLM=&HL.getWaterMap();

        for(auto it=HLM->cbegin();it!=HLM->cend();it++) {
            WaterList[TokiRC(it->first,c)]=it->second;
        }

        progressAdd(wind,4*sizePic(0));
    }
    std::cerr<<"makeHeight_new finished\n";
    size3D[2]=2+sizePic(0);//z
    size3D[0]=2+sizePic(1);//x
    size3D[1]=HighMap.maxCoeff()+1;//y
}

void TokiSlopeCraft::buildHeight(bool fireProof,bool endermanProof) {
        Build.resize(size3D[0],size3D[1],size3D[2]);
        Build.setZero();
        //Base(r+1,c)<->High(r+1,c)<->Build(c+1,High(r+1,c),r+1)
        //为了区分玻璃与空气，张量中存储的是Base+1.所以元素为1对应着玻璃，0对应空气
        int x=0,y=0,z=0;
        int yLow=0;

        std::cerr<<WaterList.size()<<" water columns in map\n";
        for(auto it=WaterList.begin();it!=WaterList.end();it++)//水柱周围的玻璃
        {
            x=TokiCol(it->first)+1;
            z=TokiRow(it->first);
            y=waterHigh(it->second);
            yLow=waterLow(it->second);
            Build(x,y+1,z)=0+1;//柱顶玻璃
            for(short yDynamic=yLow;yDynamic<=y;yDynamic++) {
                Build(x-1,yDynamic,z-0)=1;
                Build(x+1,yDynamic,z+0)=1;
                Build(x+0,yDynamic,z-1)=1;
                Build(x+0,yDynamic,z+1)=1;
            }
            if(yLow>=1)
                Build(x,yLow-1,z)=1;//柱底玻璃
        }

        progressAdd(wind,sizePic(2));

        for(short r=-1;r<sizePic(0);r++)//普通方块
        {
            for(short c=0;c<sizePic(1);c++)
            {
                if(Base(r+1,c)==12||Base(r+1,c)==0)
                    continue;
                x=c+1;y=LowMap(r+1,c);z=r+1;
                if(y>=1&&blockPalette[Base(r+1,c)].needGlass)
                    Build(x,y-1,z)=0+1;
                if((fireProof&&blockPalette[Base(r+1,c)].burnable)||
                        (endermanProof&&blockPalette[Base(r+1,c)].endermanPickable)) {
                    if(y>=1&&Build(x,y-1,z)==0)
                        Build(x,y-1,z)=0+1;
                    if(x>=1&&Build(x-1,y,z)==0)
                        Build(x-1,y,z)=0+1;
                    if(z>=1&&Build(x,y,z-1)==0)
                        Build(x,y,z-1)=0+1;
                    if(y+1<size3D[1]&&Build(x,y+1,z)==0)
                        Build(x,y+1,z)=0+1;
                    if(x+1<size3D[0]&&Build(x+1,y,z)==0)
                        Build(x+1,y,z)=0+1;
                    if(z+1<size3D[2]&&Build(x,y,z+1)==0)
                        Build(x,y,z+1)=0+1;
                }

                Build(x,y,z)=Base(r+1,c)+1;
            }
            progressAdd(wind,sizePic(1));
        }


    progressAdd(wind,sizePic(2));

    for(auto it=WaterList.cbegin();it!=WaterList.cend();it++)
    {
        x=TokiCol(it->first)+1;
        z=TokiRow(it->first);
        y=waterHigh(it->second);
        yLow=waterLow(it->second);
        for(short yDynamic=yLow;yDynamic<=y;yDynamic++) {
            Build(x,yDynamic,z)=13;
        }
    }
    /*
    for(short c=0;c<sizePic(1);c++)//北侧方块
        if(Base(0,c))   Build(c+1,HighMap(0,c),0)=11+1;*/
}

void TokiSlopeCraft::makeBridge() {
    if(mapType!=mapTypes::Slope)
        return;
    if(glassMethod!=glassBridgeSettings::withBridge)
        return;

    int step=sizePic(2)/Build.dimension(1);

    algoProgressRangeSet(wind,0,100,0);

    for(uint32_t y=0;y<Build.dimension(1);y++) {
        keepAwake(wind);
        progressAdd(wind,step);
        if(y%(bridgeInterval+1)==0) {
            std::array<int,3> start,extension;
            start[0]=0;start[1]=y;start[2]=0;
            extension[0]=size3D[0];extension[1]=1;extension[2]=size3D[2];
            TokiMap targetMap=ySlice2TokiMap(Build.slice(start,extension));
            glassMap glass;
            std::cerr<<"Construct glass bridge at y="<<y<<std::endl;
            glass=glassBuilder->makeBridge(targetMap);
            for(int r=0;r<glass.rows();r++)
                for(int c=0;c<glass.cols();c++)
                    if(Build(r,y,c)==PrimGlassBuilder::air&&
                            glass(r,c)==PrimGlassBuilder::glass)
                        Build(r,y,c)=PrimGlassBuilder::glass;
        }
        else {
            continue;
            std::array<int,3> start,extension;
            start[0]=0;start[1]=y;start[2]=0;
            extension[0]=size3D[0];extension[1]=1;extension[2]=size3D[2];
            TokiMap yCur=ySlice2TokiMap(Build.slice(start,extension));
            start[1]=y-1;
            TokiMap yBelow=ySlice2TokiMap(Build.slice(start,extension));
            std::cerr<<"Construct glass bridge between y="<<y<<" and y="<<y-1<<std::endl;
            glassMap glass=connectBetweenLayers(yCur,yBelow,nullptr);

            for(int r=0;r<glass.rows();r++)
                for(int c=0;c<glass.cols();c++)
                    if(Build(r,y,c)==PrimGlassBuilder::air&&
                            glass(r,c)==PrimGlassBuilder::glass)
                        Build(r,y,c)=PrimGlassBuilder::glass;
        }
    }
    algoProgressRangeSet(wind,0,100,100);
    std::cerr<<"makeBridge finished\n";
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

void TokiSlopeCraft::getBlockCounts(int * total, int detail[64]) const {
    std::vector<int> temp;
    if(total!=nullptr)
        *total=getBlockCounts(temp);
    if(detail!=nullptr)
        for(uint16_t idx=0;idx<temp.size();idx++) {
            detail[idx]=temp[idx];
        }
}

int TokiSlopeCraft::getBlockCounts(std::vector<int> & dest) const {
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

int TokiSlopeCraft::getBlockCounts() const {
    if(kernelStep<builded) return -1;
    int totalCount=0;
    for(int i=0;i<Build.size();i++) {
        if(Build(i))
            totalCount++;
    }
    return totalCount;
}

void TokiSlopeCraft::writeBlock(const std::string &netBlockId,
                const std::vector<std::string> & Property,
                const std::vector<std::string> & ProVal,
                NBT::NBTWriter & Lite) {
    Lite.writeCompound("ThisStringShouldNeverBeSeen");
        std::string BlockId=netBlockId;

        Lite.writeString("Name",BlockId.data());
        if(Property.empty()||ProVal.empty())
        {
            Lite.endCompound();
            return;
        }

        if(Property.size()!=ProVal.size())
        {
            std::cerr<<"Error: Property and ProVal have different sizes\n";
            return;
        }
            Lite.writeCompound("Properties");
                for(unsigned short i=0;i<ProVal.size();i++)
                    Lite.writeString(Property.at(i).data(),ProVal.at(i).data());
            Lite.endCompound();
            //Property.clear();
            //ProVal.clear();
        Lite.endCompound();
}

void TokiSlopeCraft::writeTrash(int count,NBT::NBTWriter & Lite) {
    std::vector<std::string> ProName(5),ProVal(5);
    //ProName:NEWSP
    //,,,,
    ProName.at(0)="north";
    ProName.at(1)="east";
    ProName.at(2)="west";
    ProName.at(3)="south";
    ProName.at(4)="power";
    std::string dir[3]={"none","size","up"};
    std::string power[16];
    for(short i=0;i<15;i++)
        power[i]=std::to_string(i);
    int written=0;
    for(short North=0;North<3;North++)
        for(short East=0;East<3;East++)
            for(short West=0;West<3;West++)
                for(short South=0;South<3;South++)
                    for(short Power=0;Power<16;Power++)
                    {
                        if(written>=count)return;
                        if(!Lite.isInList())return;
                        ProVal.at(0)=dir[North];
                        ProVal.at(1)=dir[East];
                        ProVal.at(2)=dir[West];
                        ProVal.at(3)=dir[South];
                        ProVal.at(4)=power[Power];
                        writeBlock("minecraft:redstone_wire",ProName,ProVal,Lite);
                        written++;
                    }
}

void TokiSlopeCraft::exportAsLitematic(const char *TargetName,
                                       const char *LiteName,
                                       const char *author,
                                       const char *RegionName,
                                       char *FileName) const {
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    std::string temp=exportAsLitematic(TargetName,LiteName,author,RegionName);
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    if(FileName!=nullptr)
        std::strcpy(temp.data(),FileName);
}

std::string TokiSlopeCraft::exportAsLitematic(const std::string & TargetName,
                                         const std::string & LiteName,
                                         const std::string & author,
                                         const std::string & RegionName) const {
    if(kernelStep<builded) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return "Too hasty! export litematic after you built!";
    }
    reportWorkingStatue(wind,workStatues::writingMetaInfo);
    progressRangeSet(wind,0,100+Build.size(),0);
        NBT::NBTWriter Lite;
        std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
        std::string unCompressed=TargetName+".TokiNoBug";
        Lite.open(unCompressed.data());
        std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
        Lite.writeCompound("Metadata");
        std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
            Lite.writeCompound("EnclosingSize");
            std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
                Lite.writeInt("x",size3D[0]);
                Lite.writeInt("y",size3D[1]);
                Lite.writeInt("z",size3D[2]);
            Lite.endCompound();
            std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
            Lite.writeString("Author",author.data());
            std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
            static const std::string Description="This litematic is generated by SlopeCraft "+std::string(Kernel::getSCLVersion())+", developed by TokiNoBug";
            Lite.writeString("Description",Description.data());
            Lite.writeString("Name",LiteName.data());
            std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
            Lite.writeInt("RegionCount",1);
            std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
            Lite.writeLong("TimeCreated",114514);
            Lite.writeLong("TimeModified",1919810);
            std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
            Lite.writeInt("TotalBlocks",this->getBlockCounts());
            Lite.writeInt("TotalVolume",Build.size());
            std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
        Lite.endCompound();
        std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    progressRangeSet(wind,0,100+Build.size(),50);
        Lite.writeCompound("Regions");
            Lite.writeCompound(RegionName.data());
                Lite.writeCompound("Position");
                    Lite.writeInt("x",0);
                    Lite.writeInt("y",0);
                    Lite.writeInt("z",0);
                Lite.endCompound();
                std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
                Lite.writeCompound("Size");
                    Lite.writeInt("x",size3D[0]);
                    Lite.writeInt("y",size3D[1]);
                    Lite.writeInt("z",size3D[2]);
                Lite.endCompound();
                std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
                progressRangeSet(wind,0,100+Build.size(),100);

                reportWorkingStatue(wind,workStatues::writingBlockPalette);

                Lite.writeListHead("BlockStatePalette",NBT::idCompound,131);
                    {
                        short written=((mcVer>=MC16)?59:52);
                        if(mcVer>=17)written=62;
                        std::vector<std::string> ProName,ProVal;
                        //bool isNetBlockId;
                        std::string netBlockId;

                        simpleBlock::dealBlockId("minecraft:air",netBlockId,&ProName,&ProVal);
                        writeBlock(netBlockId,ProName,ProVal,Lite);
                        for(short r=0;r<written;r++)
                        {
                            simpleBlock::dealBlockId(
                                        (mcVer>=gameVersion::MC13)?(blockPalette[r].id):(blockPalette[r].idOld)
                                        ,netBlockId,&ProName,&ProVal);
                            writeBlock(netBlockId,ProName,ProVal,Lite);
                        }//到此写入了written+1个方块，还需要写入130-written个

                        writeTrash(130-written,Lite);
                    }
                Lite.writeListHead("Entities",NBT::idCompound,0);
                Lite.writeListHead("PendingBlockTicks",NBT::idCompound,0);
                Lite.writeListHead("PendingFluidTiccks",NBT::idCompound,0);
                Lite.writeListHead("TileEntities",NBT::idCompound,0);
                {
                    int ArraySize;
                    //Lite.writeLong("aLong",1145141919810);
                    int Volume=size3D[0]*size3D[1]*size3D[2];
                    ArraySize=((Volume%8)?(Volume/8+1):Volume/8);
                    long long HackyVal=sizeof(long long);
                    char *inverser=(char*)&HackyVal;
                    short inverserIndex=7;

                    reportWorkingStatue(wind,workStatues::writing3D);

                Lite.writeLongArrayHead("BlockStates",ArraySize);
                for(int y=0;y<size3D[1];y++)
                    for(int z=0;z<size3D[2];z++)
                    {
                        for(int x=0;x<size3D[0];x++)
                        {
                            inverser[inverserIndex--]=Build(x,y,z);

                            if(inverserIndex<0)
                            {
                                inverserIndex=7;
                                Lite.writeLongDirectly("id",HackyVal);
                            }
                        }
                        progressAdd(wind,size3D[0]);
                    }

                if(!Lite.isListFinished())
                    Lite.writeLongDirectly("id",HackyVal);
                }
        Lite.endCompound();
        std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    Lite.endCompound();
    switch (mcVer)
    {
    case MC12:
        Lite.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
        Lite.writeInt("Version",4);
        break;
    case MC13:
        Lite.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
        Lite.writeInt("Version",5);
        break;
    case MC14:
        Lite.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
        Lite.writeInt("Version",5);
        break;
    case MC15:
        Lite.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
        Lite.writeInt("Version",5);
        break;
    case MC16:
        Lite.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
        Lite.writeInt("Version",5);
        break;
    case MC17:
        Lite.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
        Lite.writeInt("Version",5);
        break;
    case MC18:
        Lite.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
        Lite.writeInt("Version",5);
        break;
    default:
        std::cerr<<"Wrong game version!\n";
        break;
    }

    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    Lite.close();
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;

    reportWorkingStatue(wind,workStatues::none);

    if(!compressFile(unCompressed.data(),TargetName.data())) {
        reportError(wind,errorFlag::FAILED_TO_COMPRESS);
        return unCompressed;
    }
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;

    if(std::remove(unCompressed.data())!=0) {
        reportError(wind,errorFlag::FAILED_TO_REMOVE);
        return unCompressed;
    }

        return "";
}

void TokiSlopeCraft::exportAsStructure(const char *TargetName,
                                       char *FileName) const {
    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    std::string temp=exportAsStructure(TargetName);
    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    if(FileName!=nullptr)
        std::strcpy(temp.data(),FileName);
    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
}

std::string TokiSlopeCraft::exportAsStructure(const std::string &TargetName) const {
    if(kernelStep<builded) {
        reportError(wind,errorFlag::HASTY_MANIPULATION);
        return "Too hasty! export structure after you built!";
    }
    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    reportWorkingStatue(wind,workStatues::writingMetaInfo);
    progressRangeSet(wind,0,100+Build.size(),0);
    NBT::NBTWriter file;
    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    std::string unCompress=TargetName+".TokiNoBug";
        file.open(unCompress.data());
        //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
        file.writeListHead("entities",NBT::idByte,0);
        file.writeListHead("size",NBT::idInt,3);
            file.writeInt("This should never be shown",size3D[0]);
            file.writeInt("This should never be shown",size3D[1]);
            file.writeInt("This should never be shown",size3D[2]);

            reportWorkingStatue(wind,workStatues::writingBlockPalette);

            file.writeListHead("palette",NBT::idCompound,70);
                {
                    short written=((mcVer>=MC16)?59:52);
                    if(mcVer>=MC17)written=62;
                    std::vector<std::string> ProName,ProVal;
                    //bool isNetBlockId;
                    std::string netBlockId;

                    simpleBlock::dealBlockId("minecraft:air",netBlockId,&ProName,&ProVal);
                    writeBlock(netBlockId,ProName,ProVal,file);
                    for(short r=0;r<written;r++)
                    {
                        simpleBlock::dealBlockId(
                                    (mcVer>gameVersion::MC12)?(blockPalette[r].id):(blockPalette[r].idOld),
                                                 netBlockId,
                                                 &ProName,&ProVal);
                        writeBlock(netBlockId,ProName,ProVal,file);
                    }//到此写入了written+1个方块，还需要写入69-written个

                    writeTrash(69-written,file);
                }

            int BlockCount=0;
            for(int i=0;i<Build.size();i++)
                if(Build(i))BlockCount++;

            reportWorkingStatue(wind,workStatues::writing3D);

            //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
            file.writeListHead("blocks",NBT::idCompound,BlockCount);
                for(int x=0;x<size3D[0];x++)
                    for(int y=0;y<size3D[1];y++) {
                        for(int z=0;z<size3D[2];z++) {
                            if(!Build(x,y,z))continue;
                            file.writeCompound("This should never be shown");
                                file.writeListHead("pos",NBT::idInt,3);
                                    file.writeInt("This should never be shown",x);
                                    file.writeInt("This should never be shown",y);
                                    file.writeInt("This should never be shown",z);
                                file.writeInt("state",Build(x,y,z));
                            file.endCompound();
                        }
                        progressAdd(wind,size3D[2]);
                    }
                switch (mcVer)
                {
                case MC12:
                    file.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
                    break;
                case MC13:
                    file.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
                    break;
                case MC14:
                    file.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
                    break;
                case MC15:
                    file.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
                    break;
                case MC16:
                    file.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
                    break;
                case MC17:
                    file.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
                    break;
                case MC18:
                    file.writeInt("MinecraftDataVersion",mcVersion2VersionNumber(mcVer));
                    break;
                default:
                    std::cerr<<"Wrong game version!\n";
                    break;
                }

                //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    file.close();

    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    progressRangeSet(wind,0,100,100);
    reportWorkingStatue(wind,workStatues::none);

    if(!compressFile(unCompress.data(),TargetName.data())) {
        reportError(wind,errorFlag::FAILED_TO_COMPRESS);
        return unCompress;
    }
    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;

    if(std::remove(unCompress.data())!=0) {
        reportError(wind,errorFlag::FAILED_TO_REMOVE);
        return unCompress;
    }

    //std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
        return "";
}

int TokiSlopeCraft::getXRange() const {
    if(kernelStep<builded)return -1;
    return size3D[0];
}
int TokiSlopeCraft::getZRange() const {
    if(kernelStep<builded)return -1;
    return size3D[2];
}

const unsigned char * TokiSlopeCraft::getBuild(int *xSize, int *ySize, int *zSize) const {
    if(xSize!=nullptr)
        *xSize=getXRange();
    if(ySize!=nullptr)
        *ySize=getHeight();
    if(zSize!=nullptr)
        *zSize=getZRange();
    return Build.data();
}

const Eigen::Tensor<uchar,3> & TokiSlopeCraft::getBuild() const {
    return Build;
}
