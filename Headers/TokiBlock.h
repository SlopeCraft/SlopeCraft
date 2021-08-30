#ifndef TOKIBLOCK_H
#define TOKIBLOCK_H
#include <vector>
#include <string>
#include <iostream>
#include <QRadioButton>

typedef unsigned char uchar ;
typedef std::vector<std::string> stringList;
class TokiBlock
{
public:
    TokiBlock();
    /*
    TokiBlock(uchar,//mapColor
              const std::string &,//id
              const std::string &,//nameZH
              const std::string &,//nameEN
              uchar=0,//version
              const std::string & ="",//idOld
              bool=false,//needGlass
              bool=false//isGlowing
                      );*/
    std::string iconPath;

    QRadioButton * btn;

    QString nameZH;
    QString nameEN;

    /*INLINE*/ void setBaseColor(uchar);
    /*INLINE*/ void setId(const std::string &);
    ///*INLINE*/ void setName(const std::string &);
    /*INLINE*/ void setVersion(uchar);
    /*INLINE*/ void setIdOld(const std::string &);
    /*INLINE*/ void setNeedGlass(bool);
    /*INLINE*/ void setIsGlowing(bool);

    /*INLINE*/ uchar getBaseColor() const;
    /*INLINE*/ std::string getId() const;
    ///*INLINE*/ std::string getName() const;
    /*INLINE*/ uchar getVersion() const;
    /*INLINE*/ std::string getIdOld() const;
    /*INLINE*/ bool getNeedGlass() const;
    /*INLINE*/ bool getIsGlowing() const;

    std::string toPureBlockId() const;
    bool toProperties(std::string & ,stringList * proName,stringList * proVal) const;
#ifdef putBlockList
    QString toJSON() const;
#endif

private:
    uchar baseColor;
    std::string id;
    //std::string name;
    uchar version;
    std::string idOld;
    bool needGlass;
    bool isGlowing;

};

#endif // TOKIBLOCK_H
