#include "Block.h"

BlockWidget::BlockWidget(QWidget* parent, SlopeCraft::mc_block_interface* _blk)
    : QRadioButton(parent), block(_blk) {
  this->setText(this->block->getNameZH());
  QImage img{this->block->imageCols(), this->block->imageRows(),
             QImage::Format_ARGB32};
  this->block->getImage(reinterpret_cast<uint32_t*>(img.scanLine(0)));

  this->setIcon(QIcon{QPixmap::fromImage(img)});
};
void BlockWidget::update_lang(SCL_language lang) noexcept {
  switch (lang) {
    case SCL_language::Chinese:
      this->setText(QString::fromUtf8(this->block->getNameZH()));
      break;
    case SCL_language::English:
      this->setText(QString::fromUtf8(this->block->getNameEN()));
      break;
  }
}