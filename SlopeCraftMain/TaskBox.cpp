#include "TaskBox.h"

TaskBox::TaskBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskBox)
{
    ui->setupUi(this);

    connect(ui->ifExport3D,&QCheckBox::toggled,
            this,&TaskBox::onUserUsed);
    connect(ui->ifExportData,&QCheckBox::toggled,
            this,&TaskBox::onUserUsed);
    connect(ui->setMapBegSeq,&QSpinBox::valueChanged,
            this,&TaskBox::onUserUsed);
    connect(ui->imageName,&QLineEdit::editingFinished,
            this,&TaskBox::onImageChanged);



}

void TaskBox::on_BtnErase_clicked() {
    emit erase(this);
}

void TaskBox::onImageChanged() {
    QString imageFileName=ui->imageName->text();
    QPixmap img(imageFileName);
    ui->preView->setPixmap(img.scaledToHeight(ui->preView->height()));
    emit modified(this);
}

void TaskBox::onUserUsed() {

    uint begSeq=ui->setMapBegSeq->value();
    uint endSeq=begSeq+task->mapCount()-1;
    ui->setMapBegSeq->setSuffix(" ~ "+QString::number(endSeq));
    emit modified(this);
}

void TaskBox::on_browseImage_clicked() {

}

