#include <AdaptiveLabel.h>
#include "CompressEffectViewer.h"
#include "ui_CompressEffectViewer.h"

#include "SCWind.h"
#include <QFileDialog>
#include <QMessageBox>

CompressEffectViewer::CompressEffectViewer(
    SCWind* parent, const SlopeCraft::converted_image& cvted,
    const SlopeCraft::structure_3D& structure)
    : QDialog{parent}, ui{new Ui::CompressEffectViewer} {
  this->ui->setupUi(this);

  const int rows = cvted.rows();
  const int cols = cvted.cols();
  QImage img{cols, rows, QImage::Format_ARGB32};
  cvted.get_compressed_image(structure,
                             reinterpret_cast<uint32_t*>(img.scanLine(0)));

  this->ui->lb_display->setPixmap(QPixmap::fromImage(img));
}

CompressEffectViewer::~CompressEffectViewer() {}

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