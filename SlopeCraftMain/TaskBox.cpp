#include "TaskBox.h"

TaskBox::TaskBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskBox)
{
    ui->setupUi(this);

    connect(ui->imageName,&QLineEdit::textEdited,
            this,&TaskBox::onImageChanged);
}

void TaskBox::updateTask() {

    onImageChanged(task.src_imageName);

    ui->setMapBegSeq->setValue(0);

    ui->imageName->setText(task.src_imageName);

    ui->ifExport3D->setChecked(!task.dst_liteFileName.isEmpty());
    ui->liteName->setText(task.dst_liteFileName);

    ui->ifExportData->setChecked(!task.dst_liteFileName.isEmpty());
    ui->dataName->setText(task.dst_DataFileName);

    onTaskTypeUpdated();
}

void TaskBox::onTaskTypeUpdated() {
    ui->ifExport3D->setCheckable(TokiTask::canExportLite);
    if(!TokiTask::canExportLite) {
        ui->ifExport3D->setChecked(false);
    }
    ui->ifExportData->setCheckable(TokiTask::canExportData);
}

void TaskBox::on_BtnErase_clicked() {
    emit erase(this);
}

void TaskBox::onImageChanged(QString newImgName) {
    QPixmap temp(newImgName);
    if(temp.isNull()) {
        task.src_imageSize=TokiRC(0,0);
        ui->preView->setText(tr("Invalid image!"));
    } else {
        task.src_imageSize=TokiRC(temp.height(),temp.width());
        ui->preView->setPixmap(temp.scaledToHeight(128));
    }
    on_setMapBegSeq_valueChanged(ui->setMapBegSeq->value());
}

void TaskBox::on_browseImage_clicked() {
    QString newImg=QFileDialog::getOpenFileName(
                                                 this,
                                                 tr("选择图片"),
                                                 "./",
                                                 tr("图片(*.png *.bmp *.jpg *.tif *.GIF )"));
    if(newImg.isEmpty()) {
       return;
    }
    ui->imageName->setText(newImg);
    onImageChanged(newImg);
}

void TaskBox::on_ifExport3D_stateChanged(int arg1) {
    ui->liteName->setEnabled(arg1);
    ui->BtnBrowseLite->setEnabled(arg1);
}

void TaskBox::on_ifExportData_stateChanged(int arg1) {
    ui->dataName->setEnabled(arg1);
    ui->BtnBrowseData->setEnabled(arg1);
    ui->setMapBegSeq->setEnabled(arg1);
}

void TaskBox::on_setMapBegSeq_valueChanged(int curValue) {
    uint endSeq=curValue+task.mapCount()-1;
    ui->setMapBegSeq->setSuffix(" ~ "+QString::number(endSeq));
    task.dst_beginSeqNum=curValue;
    /*
    static bool isBusy=false;
    if(isBusy) {
        return;
    }
    isBusy=true;*/
    emit seqNumChanged(this);
    //isBusy=false;
}

void TaskBox::on_BtnBrowseData_clicked() {
    QString dstFolder=QFileDialog::getExistingDirectory(this,tr("请选择导出的文件夹"));
    if(dstFolder.isEmpty()) {
        return;
    }
    task.dst_DataFileName=dstFolder;
    updateTask();
}

