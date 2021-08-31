#ifndef TOKIBASECOLOR_H
#define TOKIBASECOLOR_H

#include "TokiBlock.h"
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
class TokiBaseColor// : private QGroupBox , private QLayout
{
public:
    TokiBaseColor();

    QLabel * showColor;
    QCheckBox * Enable;
    std::vector<TokiBlock> Blocks;
    void createEnable();
    void createShowColor();

};

#endif // TOKIBASECOLOR_H
