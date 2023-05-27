#include <AdaptiveLabel.h>
#include "CompressEffectViewer.h"
#include "ui_CompressEffectViewer.h"

#include "SCWind.h"
#include <QFileDialog>
#include <QMessageBox>

CompressEffectViewer::CompressEffectViewer(SCWind* parent)
    : QDialog{parent}, ui{new Ui::CompressEffectViewer} {
  this->ui->setupUi(this);

  auto kernel = parent->kernel_ptr();
  const int rows = kernel->getImageRows();
  const int cols = kernel->getImageCols();
  QImage img{rows, cols, QImage::Format_ARGB32};

  kernel->getCompressedImage(
      nullptr, nullptr, reinterpret_cast<uint32_t*>(img.scanLine(0)), false);

  this->ui->lb_display->setPixmap(QPixmap::fromImage(img));
}

CompressEffectViewer::~CompressEffectViewer() { delete this->ui; }

void CompressEffectViewer::on_pb_save_image_clicked() noexcept {
  static QString prev_dir;
  const QString filename = QFileDialog::getSaveFileName(
      this, tr("保存压缩后图片"), prev_dir, "*.png");
  if (filename.isEmpty()) {
    return;
  }

  prev_dir = QFileInfo{filename}.dir().path();

  auto pixmap = this->ui->lb_display->pixmap();
  if (!pixmap.save(filename)) {
    QMessageBox::warning(this, tr("保存图像失败"),
                         tr("无法保存图像 %1").arg(filename));
  }
}