#include "TokiBlock.h"

TokiBlock::TokiBlock(QRadioButton * _target,
                     const QJsonObject & json,
                     const QString & imgDir,
                     ushort _self,
                     QObject *parent ) : QObject(parent)
{
self=_self;
target=_target;

block.id=json.value("id").toString().toLocal8Bit();
block.version=json.value("version").toInt();
block.idOld=json.value("idOld").toString().toLocal8Bit();
block.needGlass=json.value("needGlass").toBool();
block.doGlow=json.value("isGlowing").toBool();
block.endermanPickable=json.value("endermanPickable").toBool();
block.burnable=json.value("burnable").toBool();
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
        int userChoice=QMessageBox::warning(nullptr,tr("错误：方块对应的图像不存在或不可用"),
                             tr("方块id：")+QString(block.id.data())+tr("\n缺失的图像：")+imgName+
                             tr("\n你可以点击Yes忽略这个错误，点击YesToAll屏蔽同类的警告，或者点击Close结束程序"),
                             QMessageBox::StandardButton::Yes,
                             QMessageBox::StandardButton::YesToAll,
                             QMessageBox::StandardButton::Close);

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

const simpleBlock *TokiBlock::getSimpleBlock() const {
    return &block;
}

QRadioButton * TokiBlock::getNCTarget() const {
    return target;
}
