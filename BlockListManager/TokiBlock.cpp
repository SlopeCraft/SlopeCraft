#include "TokiBlock.h"

TokiBlock::TokiBlock(QRadioButton * _target,
                     const QJsonObject & json,
                     const QString & imgDir,
                     ushort _self,
                     QObject *parent ) : QObject(parent)
{
self=_self;
target=_target;

block.id=json.value("id").toString().toStdString();
block.version=json.value("version").toInt();
block.idOld=json.value("idOld").toString().toStdString();
block.needGlass=json.value("needGlass").toBool();
block.doGlow=json.value("isGlowing").toBool();
nameZH=json.value("nameZH").toString();
nameEN=json.value("nameEN").toString();

QString imgName=imgDir+"/"+json.value("icon").toString();

target->setText(nameZH);
target->setChecked(true);

connect(target,SIGNAL(clicked(bool)),this,SLOT(onTargetClicked(bool)));

if(!QFile(imgName).exists()) {
    qDebug()<<"错误！按钮"<<QString(block.id.data())<<"对应的图像"<<imgName<<"不存在！";
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

const simpleBlock * TokiBlock::getSimpleBlock() const {
    return &block;
}

QRadioButton * TokiBlock::getNCTarget() const {
    return target;
}
