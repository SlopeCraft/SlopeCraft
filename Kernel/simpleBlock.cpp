#include "simpleBlock.h"

simpleBlock::simpleBlock()
{
    id="";
    version=0;
    idOld="";
    needGlass=false;
    doGlow=false;
/*
    std::string id;
    uchar version;
    std::string idOld;
    bool needGlass;
    bool doGlow;
*/
}

bool simpleBlock::dealBlockId(const std::string & id ,
                            std::string & netBlockId,
                            stringList * proName,
                 stringList * proVal) {
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

void simpleBlock::copyFrom(const simpleBlock * src) {
    std::cerr<<"copy begin\n";
    this->version=src->version;
    std::cerr<<"version copied\n";
    this->doGlow=src->doGlow;
    std::cerr<<"doGlow copied\n";
    this->needGlass=src->needGlass;
    std::cerr<<"needGlass copied\n";
    id=""+src->id;
    std::cerr<<"id copied\n";
    this->idOld=src->idOld;
    std::cerr<<"idOld copied\n";
}
/*
simpleBlock& simpleBlock::operator =(const simpleBlock & src) {
    std::cerr<<"copy begin\n";
    this->version=src.version;
    std::cerr<<"version copied\n";
    this->doGlow=src.doGlow;
    std::cerr<<"doGlow copied\n";
    this->needGlass=src.needGlass;
    std::cerr<<"needGlass copied\n";
    id=""+src.id;
    std::cerr<<"id copied\n";
    this->idOld=src.idOld;
    std::cerr<<"idOld copied\n";
}*/
