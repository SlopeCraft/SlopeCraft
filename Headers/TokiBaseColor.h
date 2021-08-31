#ifndef TOKIBASECOLOR_H
#define TOKIBASECOLOR_H

#include "TokiBlock.h"
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include <QRgb>

class TokiBlock;
class TokiBaseColor;
class BlockList;

class TokiBaseColor : public QGroupBox , public QGridLayout
{
    Q_OBJECT
    friend class BlockList;
public:
    TokiBaseColor();
signals:
    /*
    void switchLan(bool isEng);
    void updateStatue(ushort ver);*/
private:
    std::vector<TokiBlock * > Blocks;
    void addTokiBlock(const std::string & ,//id
                        const  QString & ,//iconPath
                        const QString & ,//nameZH
                        const QString & ,//nameEN
                        uchar = 0,//version
                      const std::string & ="",//idOld
                      bool = false,//needGlass
                      bool = false);//isGlowing
private slots:
};

#endif // TOKIBASECOLOR_H
