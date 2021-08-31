#include "TokiBlock.h"
QString TokiBlock::basePath="";
TokiBlock::TokiBlock()
{

}

/*INLINE*/ /*void TokiBlock::setBaseColor(uchar mc) {
    baseColor=mc;
}*/
/*INLINE*/ void TokiBlock::setId(const std::string &i) {
    id=i;
}
///*INLINE*/ void TokiBlock::setName(const std::string &) {}
/*INLINE*/ void TokiBlock::setVersion(uchar v) {
    version=v;
}
/*INLINE*/ void TokiBlock::setIdOld(const std::string &io) {
    idOld=io;
}
/*INLINE*/ void TokiBlock::setNeedGlass(bool ns) {
    needGlass=ns;
}
/*INLINE*/ void TokiBlock::setIsGlowing(bool ig) {
    isGlowing=ig;
}

/*INLINE*/ /*uchar TokiBlock::getBaseColor() const {
    return baseColor;
}*/
/*INLINE*/ std::string TokiBlock::getId() const {
    return id;
}
///*INLINE*/ std::string TokiBlock::getName() const {}
/*INLINE*/ uchar TokiBlock::getVersion() const {
    return version;
}
/*INLINE*/ std::string TokiBlock::getIdOld() const {
    return idOld;
}
/*INLINE*/ bool TokiBlock::getNeedGlass() const {
    return needGlass;
}
/*INLINE*/ bool TokiBlock::getIsGlowing() const {
    return isGlowing;
}

std::string TokiBlock::toPureBlockId() const {
    int i=id.find('[');
    if(i<0)return id;//isPureBlockId

    return id.substr(0,i);
}
bool TokiBlock::toProperties(std::string & netBlockId,stringList * proName,stringList * proVal) const {
proName->clear();
proVal->clear();
if(id.back()!=']')
{
    netBlockId=id;
    return false;
}
short ReadBeg=id.find('[');
        //BlockId.indexOf('[');
short ReadEnd=id.find(']');
        //BlockId.indexOf(']');

if(ReadBeg<=0||ReadEnd<=0||ReadEnd<=ReadBeg)
{
    std::cerr<<"方块Id格式出现错误:"<<id;
    return false;
}

short ProIndex[2]={-1,-1},ProValIndex[2]={-1,-1};

netBlockId=id.substr(0,ReadBeg);
        //BlockId.mid(0,ReadBeg).toLower();

for(short read=ReadBeg;read<=ReadEnd;read++)
{
    switch(id.at(read))
    {
    case '[':     //代表找到了一个新的属性
        ProIndex[0]=read+1;
        continue;

    case '=':   //识别出了属性名，寻找属性值
        ProIndex[1]=read-1;
        ProValIndex[0]=read+1;
        continue;

    case ',':   //代表结束了一个属性，并找到了下一个属性
        ProValIndex[1]=read-1;
        proName->push_back(
                    id.substr(ProIndex[0],ProIndex[1]-ProIndex[0]+1));
                    //BlockId.mid(ProIndex[0],ProIndex[1]-ProIndex[0]+1).toLower());
        proVal->push_back(
                    id.substr(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1));
                    //BlockId.mid(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1).toLower());
        ProIndex[0]=-1;ProIndex[1]=-1;ProValIndex[0]=-1;ProValIndex[1]=-1;

        ProIndex[0]=read+1;
        continue;
    case ']':
        ProValIndex[1]=read-1;
        proName->push_back(
                    id.substr(ProIndex[0],ProIndex[1]-ProIndex[0]+1));
                    //BlockId.mid(ProIndex[0],ProIndex[1]-ProIndex[0]+1).toLower());
        proVal->push_back(
                    id.substr(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1));
                    //BlockId.mid(ProValIndex[0],ProValIndex[1]-ProValIndex[0]+1).toLower());
        continue;
    }
}
//qDebug()<<proValue->back()<<'='<<proValue->back();
return true;
}
#ifdef putBlockList
QString TokiBlock::toJSON() const {
    QString js="{\n";
    char tab[]="    ";

    js+=tab;
        js+="\"mapColor\"";
        js+=':';
        js+=QString::number(baseColor);
    js+=",\n";

    js+=tab;
        js+="\"id\"";
        js+=':';
        js+=QString::fromStdString("\""+id+"\"");
    js+=",\n";

    js+=tab;
        js+="\"nameZH\"";
        js+=':';
        js+="\""+nameZH+"\"";
    js+=",\n";

    js+=tab;
        js+="\"nameEN\"";
        js+=':';
        js+="\""+nameEN+"\"";
    js+=",\n";

    js+=tab;
        js+="\"icon\"";
        js+=':';
        js+=QString::fromStdString("\""+iconPath+"\"");
    js+=",\n";

    js+=tab;
        js+="\"version\"";
        js+=':';
        js+=QString::number(version);
    js+=",\n";

    js+=tab;
        js+="\"idOld\"";
        js+=':';
        js+=QString::fromStdString("\""+idOld+"\"");
    js+=",\n";

    js+=tab;
        js+="\"needGlass\"";
        js+=':';
        js+=(needGlass?"true":"false");
    js+=",\n";

    js+=tab;
        js+="\"isGlowing\"";
        js+=':';
        js+=(isGlowing?"true":"false");
    js+="\n";

    js+='}';
    return js;
}
#endif
