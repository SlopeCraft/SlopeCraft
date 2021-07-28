#include "previewwind.h"
#include "ui_previewwind.h"

PreviewWind::PreviewWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreviewWind)
{
    ui->setupUi(this);

}

PreviewWind::~PreviewWind()
{
    delete ui;
}

QString blockCount2string(int count,int setCount)
{
    if(count>=0&&count<=(setCount-1))
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
    for(auto i=BlockCount.begin();i!=BlockCount.end();i++)
    {
        TotalBlockCount+=*i;
    }

    ui->Size->setText(QString::number(size[0])+"×"+QString::number(size[1])+"×"+QString::number(size[2]));
    ui->Volume->setText(QString::number(size[0]*size[1]*size[2]));
    ui->BlockCount->setText(QString::number(TotalBlockCount));

    auto area=ui->MaterialArea;
    QLabel *iconShower,*idShower,*countShower;
    QLabel *Spacer;
    int rows=area->columnCount(),colOffset=0;

    for(int i=0;i<(signed)Src.size();i++)
    {
        area->addWidget(iconShower=new QLabel("Test1"),rows,colOffset);
        area->addWidget(idShower=new QLabel("Test2"),rows,colOffset);
        area->addWidget(countShower=new QLabel("Test3"),rows,colOffset+1);
        iconShower->setPixmap(Src[i]->icon().pixmap(Src[i]->iconSize()));
        iconShower->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        idShower->setText("      "+Src[i]->text());
        countShower->setText(QString::number(BlockCount[i]));
        CountLabel.push_back(countShower);
        colOffset=2*(!colOffset);
        rows+=i%2;
    }

    //area->addWidget(new QSpacerItem(1,1,QSizePolicy::Preferred,QSizePolicy::Expanding),area->columnCount(),0);
    area->addWidget(Spacer=new QLabel(""),area->columnCount()+1,0);
    Spacer->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
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

