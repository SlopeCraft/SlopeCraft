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

#include "CutterWind.h"
#include "ui_CutterWind.h"
#include <QFileDialog>
#include <QMessageBox>

CutterWind::CutterWind(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::CutterWind) {
  ui->setupUi(this);

  connect(ui->actionLoadImage, &QAction::triggered, this, &CutterWind::loadImg);
  connect(ui->actionSaveImage, &QAction::triggered, this, &CutterWind::saveImg);
  connect(ui->actionResize, &QAction::triggered, this, &CutterWind::resizeImg);
  connect(ui->actionCut, &QAction::triggered, this, &CutterWind::cutImg);

  ui->boxAspectRatioMode->addItem(tr("不保持比例"),
                                  Qt::AspectRatioMode::IgnoreAspectRatio);
  ui->boxAspectRatioMode->addItem(tr("保持比例缩小"),
                                  Qt::AspectRatioMode::KeepAspectRatio);
  ui->boxAspectRatioMode->addItem(
      tr("保持比例扩张"), Qt::AspectRatioMode::KeepAspectRatioByExpanding);

  ui->boxTransformationMode->addItem(
      tr("快速变换"), Qt::TransformationMode::FastTransformation);
  ui->boxTransformationMode->addItem(
      tr("平滑变换"), Qt::TransformationMode::SmoothTransformation);
}

CutterWind::~CutterWind() { delete ui; }

void CutterWind::loadImg() {
  QString path = QFileDialog::getOpenFileName(
      this, tr("选择图片"), "", tr("图片(*.png *.bmp *.jpg *.tif)"));
  if (path.isEmpty())
    return;

  img.load(path);
  img = img.convertToFormat(QImage::Format_ARGB32);
  if (img.isNull()) {
    QMessageBox::information(this, tr("打开图片失败"),
                             tr("图片格式损坏，或者图片过于巨大。"));
    return;
  }

  updateImg();

  path = path.replace("\\\\", "/");
  path = path.replace('\\', '/');

  QString fileNameWithSuffix =
      path.right(path.length() - path.lastIndexOf('/') - 1);

  netRawFileName = fileNameWithSuffix.left(fileNameWithSuffix.indexOf('.'));
  rawFileSuffix = fileNameWithSuffix.right(fileNameWithSuffix.length() -
                                           fileNameWithSuffix.indexOf('.'));
}

void CutterWind::updateImg() const {
  ui->imgDisplay->setPixmap(QPixmap::fromImage(img));

  ui->labelShowSize->setText(tr("图片尺寸（方块）：") +
                             QString::number(img.height()) + tr("行 , ") +
                             QString::number(img.width()) + tr("列"));
}

void CutterWind::saveImg() {
  QString name = QFileDialog::getSaveFileName(
      this, tr("保存图片"), "", tr("图片(*.png *.bmp *.jpg *.tif)"));
  if (name.isEmpty())
    return;

  img.save(name);
}

void CutterWind::resizeImg() {

  Qt::AspectRatioMode arm =
      Qt::AspectRatioMode(ui->boxAspectRatioMode->currentData().toInt());
  Qt::TransformationMode tm =
      Qt::TransformationMode(ui->boxTransformationMode->currentData().toInt());

  int rows = ui->scaledRows->value();
  int cols = ui->scaledCols->value();

  img = img.scaled(cols, rows, arm, tm);

  updateImg();
}

void CutterWind::cutImg() {
  QString dir =
      QFileDialog::getExistingDirectory(this, tr("选择输出文件夹"), "");

  if (dir.isEmpty())
    return;

  dir = dir.replace("\\\\", "/");
  dir = dir.replace('\\', '/');

  QImage part(QSize(128, 128), QImage::Format_ARGB32);

  const int imgRN = img.height();
  const int imgCN = img.width();

  const int mapRN = std::ceil(imgRN / 128.0);
  const int mapCN = std::ceil(imgCN / 128.0);

  const QString fileNamePrefix = dir + '/' + netRawFileName + '_';

  for (int mapR = 0; mapR < mapRN; mapR++) {
    for (int mapC = 0; mapC < mapCN; mapC++) {
      for (int rOffset = 0; rOffset < 128; rOffset++) {
        const int imgR = rOffset + 128 * mapR;
        const uint32_t *src = nullptr;
        if (imgR < imgRN)
          src = (const uint32_t *)img.constScanLine(imgR);

        uint32_t *dst = (uint32_t *)part.scanLine(rOffset);

        for (int cOffset = 0; cOffset < 128; cOffset++) {
          const int imgC = cOffset + 128 * mapC;
          if (imgC < imgCN && imgR < imgRN)
            dst[cOffset] = src[imgC];
          else
            dst[cOffset] = 0xFFFFFFFF;
        }
      }

      QString fileName =
          fileNamePrefix + QString::number(mapR + mapC * mapRN) + rawFileSuffix;
      part.save(fileName);
    }
  }
}
