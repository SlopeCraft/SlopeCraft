#include "Block.h"

BlockWidget::BlockWidget(QWidget* parent, SlopeCraft::AbstractBlock* _blk)
    : QRadioButton(parent), block(_blk) {
  this->setText(this->block->getNameZH());
  QImage img{this->block->imageCols(), this->block->imageRows(),
             QImage::Format_ARGB32};
  this->block->getImage(reinterpret_cast<uint32_t*>(img.scanLine(0)), true);

  this->setIcon(QIcon{QPixmap::fromImage(img)});
};