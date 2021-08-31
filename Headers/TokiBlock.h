#ifndef TOKIBLOCK_H
#define TOKIBLOCK_H
#include <vector>
#include <string>
#include <iostream>
#include <QRadioButton>

class TokiBlock;
class TokiBaseColor;
class BlockList;

typedef unsigned char uchar ;
typedef std::vector<std::string> stringList;

class simpleBlock//block class without qt
{
public:
    simpleBlock();
    std::string id;
    std::string name;
    uchar version;
    std::string idOld;
    bool needGlass;
    bool isGlowing;
    std::string toPureBlockId() const;
    bool toProperties(std::string & ,stringList * proName,stringList * proVal) const;
};

class TokiBlock :public QRadioButton,private simpleBlock
{
    Q_OBJECT
    friend class TokiBaseColor;
    friend class BlockList;
public:
    TokiBlock();

    /*INLINE*/ //void setBaseColor(uchar);
    /*INLINE*/ void setId(const std::string &);
    ///*INLINE*/ void setName(const std::string &);
    /*INLINE*/ void setVersion(uchar);
    /*INLINE*/ void setIdOld(const std::string &);
    /*INLINE*/ void setNeedGlass(bool);
    /*INLINE*/ void setIsGlowing(bool);

    /*INLINE*/ //uchar getBaseColor() const;
    /*INLINE*/ std::string getId() const;
    ///*INLINE*/ std::string getName() const;
    /*INLINE*/ uchar getVersion() const;
    /*INLINE*/ std::string getIdOld() const;
    /*INLINE*/ bool getNeedGlass() const;
    /*INLINE*/ bool getIsGlowing() const;

    //std::string toPureBlockId() const;
    //bool toProperties(std::string & ,stringList * proName,stringList * proVal) const;
    simpleBlock * toSimpleBlock() const;
private slots:
    void setLanguage(bool isEng);
    void updateStatue(uchar ver);
#ifdef putBlockList
    QString toJSON() const;
#endif

private:
    static QString basePath;
    QString nameZH;
    QString nameEN;

};

#endif // TOKIBLOCK_H
