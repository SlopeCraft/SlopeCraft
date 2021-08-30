#include "TokiBlock.h"

TokiBlock::TokiBlock()
{

}

inline void TokiBlock::setBaseColor(uchar mc) {
    baseColor=mc;
}
inline void TokiBlock::setId(const std::string &i) {
    id=i;
}
//inline void TokiBlock::setName(const std::string &) {}
inline void TokiBlock::setVersion(uchar v) {
    version=v;
}
inline void TokiBlock::setIdOld(const std::string &io) {
    idOld=io;
}
inline void TokiBlock::setNeedGlass(bool ns) {
    needGlass=ns;
}
inline void TokiBlock::setIsGlowing(bool ig) {
    isGlowing=ig;
}

inline uchar TokiBlock::getBaseColor() const {
    return baseColor;
}
inline std::string TokiBlock::getId() const {
    return id;
}
//inline std::string TokiBlock::getName() const {}
inline uchar TokiBlock::getVersion() const {
    return version;
}
inline std::string TokiBlock::getIdOld() const {
    return idOld;
}
inline bool TokiBlock::getNeedGlass() const {
    return needGlass;
}
inline bool TokiBlock::getIsGlowing() const {
    return isGlowing;
}

std::string TokiBlock::toPureBlockId() const {
    int i=id.find('[');
    if(i<0)return id;//isPureBlockId

    return id.substr(0,i);
}
void TokiBlock::toProperties(stringList * proName,stringList * proVal) const {
proName->clear();
proVal->clear();
}

QString TokiBlock::toJSON() const {
    QString js="{\n";
    char tab[]="    ";

    js+=tab;
        js+="\"mapColor\"";
        js+=':';
        js+=QString::number(baseColor);
    js+='\n';

    js+=tab;
        js+="\"id\"";
        js+=':';
        js+=QString::fromStdString("\""+id+"\"");
    js+='\n';

    js+=tab;
        js+="\"nameZH\"";
        js+=':';
        js+="\""+nameZH+"\"";
    js+='\n';

    js+=tab;
        js+="\"nameEN\"";
        js+=':';
        js+="\""+nameEN+"\"";
    js+='\n';

    js+=tab;
        js+="\"icon\"";
        js+=':';
        js+=QString::fromStdString("\""+iconPath+"\"");
    js+='\n';

    js+=tab;
        js+="\"version\"";
        js+=':';
        js+=QString::number(version);
    js+='\n';

    js+=tab;
        js+="\"idOld\"";
        js+=':';
        js+=QString::fromStdString("\""+idOld+"\"");
    js+='\n';

    js+=tab;
        js+="\"needGlass\"";
        js+=':';
        js+=QString::number(needGlass);
    js+='\n';

    js+=tab;
        js+="\"needGlass\"";
        js+=':';
        js+=QString::number(isGlowing);
    js+='\n';

    js+='}';
    return js;
}
