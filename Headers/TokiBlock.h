#ifndef TOKIBLOCK_H
#define TOKIBLOCK_H
#include <vector>
#include <string>

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

    inline void setBaseColor(uchar);
    inline void setId(const std::string &);
    //inline void setName(const std::string &);
    inline void setVersion(uchar);
    inline void setIdOld(const std::string &);
    inline void setNeedGlass(bool);
    inline void setIsGlowing(bool);

    inline uchar getBaseColor() const;
    inline std::string getId() const;
    //inline std::string getName() const;
    inline uchar getVersion() const;
    inline std::string getIdOld() const;
    inline bool getNeedGlass() const;
    inline bool getIsGlowing() const;

    std::string toPureBlockId() const;
    void toProperties(stringList * proName,stringList * proVal) const;

    QString toJSON() const;

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
