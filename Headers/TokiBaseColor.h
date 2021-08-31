#ifndef TOKIBASECOLOR_H
#define TOKIBASECOLOR_H

#include "TokiBlock.h"
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include <QRgb>
class TokiBaseColor : public QGroupBox , public QGridLayout
{
    Q_OBJECT
public:
    TokiBaseColor();
    std::vector<TokiBlock * > Blocks;
    void addTokiBlock(const std::string & ,//id
                        const  std::string & ,//iconPath
                        const QString & ,//nameZH
                        const QString & ,//nameEN
                        uchar = 0,//version
                      const std::string & ="",//idOld
                      bool = false,//needGlass
                      bool = false);//isGlowing

};

#endif // TOKIBASECOLOR_H
