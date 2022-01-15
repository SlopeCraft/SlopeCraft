/*
 Copyright © 2021-2022  TokiNoBug
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

#include "previewwind.h"
#include "ui_previewwind.h"

PreviewWind::PreviewWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreviewWind)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose,false);

}

PreviewWind::~PreviewWind()
{
    delete ui;
}

QString blockCount2string(int count,int setCount)
{
    if(count>0&&count<=(setCount-1))
        return QString::number(count)+QObject::tr("个");
    if(count>=(setCount*27))
        return QString::number(count/(setCount*27))+QObject::tr("盒")+blockCount2string(count%(setCount*27));
    if(count>=setCount&&count<(setCount*27))
        return QString::number(count/setCount)+QObject::tr("组")+blockCount2string(count%setCount);
    return "";
}

void PreviewWind::ShowMaterialList()
{
    TotalBlockCount=0;
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    for(auto i=BlockCount.begin();i!=BlockCount.end();i++)
    {
        TotalBlockCount+=*i;
    }
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;

    ui->Size->setText(QString::number(size[0])
            +"×"+QString::number(size[1])
            +"×"+QString::number(size[2]));
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    ui->Volume->setText(QString::number(size[0]*size[1]*size[2]));
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
    ui->BlockCount->setText(QString::number(TotalBlockCount));
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;

    auto area=ui->MaterialArea;
    QLabel *iconShower,*idShower,*countShower;
    QLabel *Spacer;
    std::cerr<<"area==nullptr is "<<(area==nullptr)<<std::endl;
    int rows=area->columnCount(),colOffset=0;
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;

    for(int i=0;i<(signed)Src.size();i++)
    {
        std::cerr<<"i="<<i<<std::endl;
        area->addWidget(iconShower=new QLabel("Test1"),rows,colOffset);
        area->addWidget(idShower=new QLabel("Test2"),rows,colOffset);
        area->addWidget(countShower=new QLabel("Test3"),rows,colOffset+1);
        std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
        qDebug()<<""<<Src[i]->text()<<" , "<<Src[i]->text().size();
        iconShower->setPixmap(Src[i]->icon().pixmap(Src[i]->iconSize()));
        std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
        iconShower->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        idShower->setText("      "+Src[i]->text());
        countShower->setText(QString::number(BlockCount[i]));
        countShower->setAlignment(Qt::AlignHCenter);
        CountLabel.push_back(countShower);
        colOffset=2*(!colOffset);
        rows+=i%2;
    }
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;

    //area->addWidget(new QSpacerItem(1,1,QSizePolicy::Preferred,QSizePolicy::Expanding),area->columnCount(),0);
    area->addWidget(Spacer=new QLabel(""),area->rowCount(),0);
    Spacer->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    std::cerr<<__FILE__<<" , "<<__LINE__<<std::endl;
}

void PreviewWind::on_SwitchUnit_clicked(bool checked)
{
    if(checked)
    for(int i=0;i<(signed)BlockCount.size();i++)
    {
        if(Src[i]==Water)
        {
            CountLabel[i]->setText(blockCount2string(BlockCount[i],1));
            continue;
        }
        CountLabel[i]->setText(blockCount2string(BlockCount[i]));
    }
    else
    {
        for(int i=0;i<(signed)BlockCount.size();i++)
        {
            CountLabel[i]->setText(QString::number(BlockCount[i]));
        }
    }
}

void PreviewWind::showConvertedImage(const QImage & img) {
    ui->showImg->setPixmap(QPixmap::fromImage(img));
}
