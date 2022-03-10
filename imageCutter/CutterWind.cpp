#include "CutterWind.h"
#include "ui_CutterWind.h"
#include <QFileDialog>
#include <QMessageBox>

CutterWind::CutterWind(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CutterWind) {
    ui->setupUi(this);

    connect(ui->actionLoadImage,&QAction::triggered,
            this,&CutterWind::loadImg);
}

CutterWind::~CutterWind() {
    delete ui;
}


void CutterWind::loadImg() {
    QString path=QFileDialog::getOpenFileName(this,
                                               tr("选择图片"),
                                               "",
                                               tr("图片(*.png *.bmp *.jpg *.tif)"));
    if(path.isEmpty())
        return;

    img.load(path);
    if(img.isNull()) {
        QMessageBox::information(this,tr("打开图片失败"),tr("图片格式损坏，或者图片过于巨大。"));
        return;
    }

    updateImg();
}

void CutterWind::updateImg() const {
    ui->imgDisplay->setPixmap(QPixmap::fromImage(img));

    ui->labelShowSize->setText(tr("图片尺寸（方块）：")
                               +QString::number(img.height())
                               +tr("行 , ")
                               +QString::number(img.width())
                               +tr("列"));
}

void CutterWind::saveImg() const {

}



void CutterWind::resizeImg() {

}
