#include "TokiBaseColor.h"

uchar TokiBaseColor::mcVer=17;

TokiBaseColor::TokiBaseColor(uchar _baseColor,
                             QGridLayout* _layout,
                             QObject *parent) : QObject(parent)
{
layout=_layout;
baseColor=_baseColor;
//负责创建QCheckBox和对应的弹簧

layout->addWidget(checkBox=new QCheckBox("启用"),0,1);
checkBox->setChecked(false);
checkBox->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Expanding,QSizePolicy::Policy::Preferred));
checkBox->setEnabled(baseColor!=0);

connect(checkBox,&QCheckBox::clicked,this,&TokiBaseColor::updateEnabled);
connect(checkBox,&QCheckBox::clicked,this,&TokiBaseColor::userClicked);
connect(this,&TokiBaseColor::translate,this,&TokiBaseColor::translateCheckBox);
//创建弹簧
QSpacerItem * si=new QSpacerItem(40,20,
                                 QSizePolicy::Policy::Expanding,QSizePolicy::Policy::Preferred);
layout->addItem(si,0,2);

isEnabled=true;
tbs.clear();
selected=65535;
versionCheck();
}

TokiBaseColor::~TokiBaseColor() {
    for(ushort i=0;i<tbs.size();i++)
        delete tbs[i];
}
void TokiBaseColor::addTokiBlock(const QJsonObject & json,
                  const QString & imgDir) {
    //qDebug("addTokiBlock被调用");
    QRadioButton * qrb=new QRadioButton;
    int rows=1+tbs.size()/2;
    int cols=1+tbs.size()%2;
    layout->addWidget(qrb,rows,cols);
    TokiBlock * tb=new TokiBlock(qrb,json,imgDir,tbs.size());
    tbs.push_back(tb);

    if(tb->getSimpleBlock()->version>mcVer)
    {
        tb->getNCTarget()->setEnabled(false);
        tb->getNCTarget()->setChecked(false);
    }

    connect(tb,&TokiBlock::radioBtnClicked,this,&TokiBaseColor::receiveClicked);
    connect(this,&TokiBaseColor::translate,tb,&TokiBlock::translate);
    connect(tb,&TokiBlock::radioBtnClicked,this,&TokiBaseColor::userClicked);
    //qDebug("add a TokiBlock");
}

void TokiBaseColor::makeLabel(QRgb color) {
    QLabel * qL=new QLabel("");
    layout->addWidget(qL,0,0,ceil(1+tbs.size()/2.0),1);
    QPalette pl;
    pl.setColor(QPalette::ColorRole::Text,Qt::black);
    pl.setColor(QPalette::ColorRole::Window,QColor(qRed(color),qGreen(color),qBlue(color),255*bool(baseColor)));

    qL->setPalette(pl);
    qL->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::Policy::Preferred);
    qL->setMinimumWidth(30);
    qL->setFrameShape(QFrame::Shape::StyledPanel);
    qL->setFrameShadow(QFrame::Shadow::Plain);
    qL->setLineWidth(1);
    qL->setAutoFillBackground(true);
}

void TokiBaseColor::receiveClicked(ushort _selected) {
    selected=_selected%tbs.size();
    versionCheck();
}

bool TokiBaseColor::isAllOverVersion() const {//判断是否所有方块都超版本了
    bool isAllOver=false;
    for(auto it=tbs.cbegin();it!=tbs.cend();it++) {
        isAllOver+=((*it)->block.version>mcVer);
        if(isAllOver)break;
    }
    return isAllOver;
}

void TokiBaseColor::versionCheck() {
    if(tbs.size()<=0) {
        selected=65535;
        return;}
    if(tbs.size()==1) {
        selected=0;
        tbs[0]->getNCTarget()->setChecked(true);
        tbs[0]->getNCTarget()->setEnabled(false);
        return;
    }
    std::vector<short>scores(tbs.size());
    for(ushort idx=0;idx<tbs.size();idx++) {
        if(tbs[idx]->getSimpleBlock()->version<=mcVer) {
            scores[idx]=(tbs[idx]->getTarget()->isChecked())?100:51;
            tbs[idx]->getNCTarget()->setEnabled(true);
        }
        else {
            scores[idx]=(idx<=0);
            tbs[idx]->getNCTarget()->setEnabled(false);
        }
    }
    ushort maxIndex=0;
    for(ushort idx=0;idx<scores.size();idx++) {
        if(scores[idx]>scores[maxIndex])
            maxIndex=idx;
    }
    selected=maxIndex;
    if(!tbs[selected]->getTarget()->isChecked()) {
        tbs[selected]->getNCTarget()->setChecked(true);
    }

    checkBox->setEnabled(!isAllOverVersion());
    if(!checkBox->isEnabled()) {
        checkBox->setChecked(false);
    }
    isEnabled=checkBox->isChecked();

}

void TokiBaseColor::setSelected(ushort sel) {
    tbs[sel]->getNCTarget()->setChecked(true);
}

void TokiBaseColor::updateEnabled(bool isChecked) {
    isEnabled=isChecked;
    versionCheck();
}

void TokiBaseColor::translateCheckBox(Language lang) {
    switch (lang) {
    case ZH:
        checkBox->setText("启用");
        break;
    case EN:
        checkBox->setText("Enable");
        break;
    }

}

const TokiBlock* TokiBaseColor::getTokiBlock() const {
    qDebug("getTokiBlock");
    qDebug()<<selected;
    return tbs[selected];
}

bool TokiBaseColor::getEnabled() const {
    return isEnabled;
}

ushort TokiBaseColor::getSelected() const {
    return selected;
}
