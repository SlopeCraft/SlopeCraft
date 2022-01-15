/*
 Copyright © 2021  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#include "TokiBlock.h"

using namespace SlopeCraft;

TokiBlock::TokiBlock(QRadioButton * _target,
                     const QJsonObject & json,
                     const QString & imgDir,
                     ushort _self,
                     QObject *parent ) : QObject(parent)
{
self=_self;
target=_target;

block=AbstractBlock::create();

block->setId(json.value("id").toString().toLocal8Bit());

block->setVersion(json.value("version").toInt());
block->setIdOld(json.value("idOld").toString().toLocal8Bit());
block->setNeedGlass(json.value("needGlass").toBool());
block->setDoGlow(json.value("isGlowing").toBool());
block->setEndermanPickable(json.value("endermanPickable").toBool());
block->setBurnable(json.value("burnable").toBool());
block->setWallUseable(json.value("wallUseable").toBool());
nameZH=json.value("nameZH").toString();
nameEN=json.value("nameEN").toString();
//std::cerr<<block.id<<"\n"<<block.idOld<<"\n";
QString imgName=imgDir+"/"+json.value("icon").toString();

target->setText(nameZH);
target->setChecked(true);

connect(target,&QRadioButton::clicked,this,&TokiBlock::onTargetClicked);
static bool showLater=true;
if((!QFile(imgName).exists()||QIcon(imgName).isNull())) {
    if(showLater) {
        QMessageBox::StandardButton userChoice
                =QMessageBox::warning(nullptr,tr("错误：方块对应的图像不存在或不可用"),
                             tr("方块id：")+QString::fromLocal8Bit(block->getId())
                                      +tr("\n缺失的图像：")+imgName+
                             tr("\n你可以点击Yes忽略这个错误，点击YesToAll屏蔽同类的警告，或者点击Close结束程序"),
                             {QMessageBox::StandardButton::Yes,
                             QMessageBox::StandardButton::YesToAll,
                             QMessageBox::StandardButton::Close});

        switch (userChoice) {
        case QMessageBox::StandardButton::Yes:
            break;
        case QMessageBox::StandardButton::YesToAll:
            showLater=false;
            break;
        default:
            exit(0);
        }
    }
    qDebug()<<"错误！按钮"<<QString::fromLocal8Bit(block->getId())<<"对应的图像"<<imgName<<"不存在！";
    return;
}
target->setIcon(QIcon(imgName));
/*
{
    "mapColor":0,
    "id":"glass",
    "nameZH":"玻璃",
    "nameEN":"Glass",
    "icon":"glass.png",
    "version":0,
    "idOld":"",
    "needGlass":false,
    "isGlowing":false
},
*/

}
void TokiBlock::translate(Language lang) {
    switch (lang) {
    case Language::ZH:
        target->setText(nameZH);
        break;
    case Language::EN:
        target->setText(nameEN);
        break;
    }
    return;
}

TokiBlock::~TokiBlock() {

}

void TokiBlock::onTargetClicked(bool isChecked) {
    if(isChecked)
        emit radioBtnClicked(self);
}

const QRadioButton * TokiBlock::getTarget() const {
    return target;
}

const AbstractBlock *TokiBlock::getSimpleBlock() const {
    return block;
}

QRadioButton * TokiBlock::getNCTarget() const {
    return target;
}
