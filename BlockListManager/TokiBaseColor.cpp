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

connect(checkBox,SIGNAL(clicked(bool)),this,SLOT(updateEnabled(bool)));
connect(checkBox,SIGNAL(clicked(bool)),this,SLOT(userClicked()));
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

    connect(tb,SIGNAL(radioBtnClicked(ushort)),this,SLOT(receiveClicked(ushort)));
    connect(this,SIGNAL(translate(Language)),tb,SLOT(translate(Language)));
    connect(tb,SIGNAL(radioBtnClicked(ushort)),this,SLOT(userClicked()));
}

void TokiBaseColor::makeLabel(QRgb color) {
    QLabel * qL=new QLabel("");
    layout->addWidget(qL,0,0,ceil(1+tbs.size()/2),1);
    QPalette pl;
    pl.setColor(QPalette::ColorRole::Text,Qt::black);
    if(baseColor!=0) {
        pl.setColor(QPalette::ColorRole::Base,QColor(0,0,0,0));
    } else {
        pl.setColor(QPalette::ColorRole::Base,QColor(color));
    }
    qL->setPalette(pl);
    qL->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::Policy::Preferred);
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
        tbs[0]->getNCTarget()->setChecked(true);
        tbs[0]->getNCTarget()->setEnabled(false);
        return;
    }
    std::vector<short>scores(tbs.size());
    for(ushort idx=0;idx<tbs.size();idx++) {
        if(tbs[idx]->getSimpleBlock()->version>=mcVer) {
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

const TokiBlock* TokiBaseColor::getTokiBlock() const {
    return tbs[selected];
}

bool TokiBaseColor::getEnabled() const {
    return isEnabled;
}

ushort TokiBaseColor::getSelected() const {
    return selected;
}
