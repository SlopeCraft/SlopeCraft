#include "TokiBaseColor.h"

TokiBaseColor::TokiBaseColor()
{
    Blocks.clear();
}

void TokiBaseColor::addTokiBlock(const std::string & id,//id
                    const  QString & iconPath,//iconPath
                    const QString & nameZH,//nameZH
                    const QString & nameEN,//nameEN
                    uchar ver,//version
                  const std::string & idOld,//idOld
                  bool needGlass,//needGlass
                  bool isGlowing)//isGlowing
{
    TokiBlock * temp;
    int nowCount=Blocks.size()+1;
    addWidget(temp=new TokiBlock,nowCount%2+1,nowCount/2+1);
    Blocks.push_back(temp);

    temp->setId(id);
    QIcon ico(TokiBlock::basePath+"/"+iconPath);
    temp->setIcon(ico);
    temp->setVersion(ver);
    temp->nameZH=nameZH;
    temp->nameEN=nameEN;
    if(ver<12)
        temp->idOld=idOld;
    else
        temp->idOld="";
    temp->needGlass=needGlass;
    temp->isGlowing=isGlowing;
}
