#include "BlockBrowser.h"
#include "VCWind.h"
#include "VC_block_class.h"
#include "ui_BlockBrowser.h"
#include <QFileDialog>
#include <QMessageBox>
#include <magic_enum.hpp>

BlockBrowser::BlockBrowser(QWidget *parent)
    : QWidget(parent), ui(new Ui::BlockBrowser) {
  ui->setupUi(this);

  this->fecth_content();

  connect(this->ui->combobox_select_blk, &QComboBox::currentIndexChanged, this,
          &BlockBrowser::update_display);
  connect(this->ui->combobox_select_face, &QComboBox::currentIndexChanged, this,
          &BlockBrowser::update_display);
  connect(this->ui->sb_scale_ratio, &QSpinBox::valueChanged, this,
          &BlockBrowser::update_display);
}

BlockBrowser::~BlockBrowser() { delete this->ui; }

VCWind *BlockBrowser::parent() noexcept {
  return dynamic_cast<VCWind *>(QWidget::parentWidget());
}
const VCWind *BlockBrowser::parent() const noexcept {
  return dynamic_cast<const VCWind *>(QWidget::parentWidget());
}

void BlockBrowser::fecth_content() noexcept {
  this->ui->combobox_select_blk->clear();
  this->ui->combobox_select_face->clear();

  for (const auto &pair_class : this->parent()->block_class_widgets()) {
    for (const auto &pair_blk : pair_class.second->blocks_vector()) {
      this->ui->combobox_select_blk->addItem(
          QString::fromUtf8(VCL_get_block_name(pair_blk.first, true)),
          QVariant::fromValue((void *)pair_blk.first));
    }
  }

  for (auto face : magic_enum::enum_values<VCL_face_t>()) {
    auto str = magic_enum::enum_name(face);

    this->ui->combobox_select_face->addItem(str.data(), int(face));
  }
}

void BlockBrowser::update_display() noexcept {

  if (this->ui->combobox_select_blk->currentIndex() < 0) {
    return;
  }

  if (this->ui->combobox_select_face->currentIndex() < 0) {
    return;
  }

  const int scale = this->ui->sb_scale_ratio->value();
  VCL_block *blk =
      (VCL_block *)this->ui->combobox_select_blk->currentData().value<void *>();
  if (blk == nullptr) {
    abort();
  }

  this->ui->tb_view_id->setText(QString::fromUtf8(VCL_get_block_id(blk)));

  VCL_face_t face =
      this->ui->combobox_select_face->currentData().value<VCL_face_t>();

  VCL_model *md =
      VCL_get_block_model(blk, VCL_get_resource_pack(), face, nullptr);

  if (md == nullptr) {
    return;
  }

  int raw_image_rows = 0, raw_image_cols = 0;

  VCL_compute_projection_image(md, face, &raw_image_rows, &raw_image_cols,
                               nullptr, 0);

  QImage raw_image(raw_image_cols, raw_image_rows, QImage::Format_ARGB32);
  memset(raw_image.scanLine(0), 0xFF, raw_image.sizeInBytes());

  bool ok = VCL_compute_projection_image(md, face, nullptr, nullptr,
                                         (uint32_t *)raw_image.scanLine(0),
                                         raw_image.sizeInBytes());
  if (!ok) {
    VCL_destroy_block_model(md);
    return;
  }

  const int scaled_rows = raw_image_rows * scale;
  const int scaled_cols = raw_image_cols * scale;

  QImage scaled_img(scaled_cols, scaled_rows, QImage::Format_ARGB32);

  for (int sr = 0; sr < scaled_rows; sr++) {
    uint32_t *const dst = (uint32_t *)scaled_img.scanLine(sr);

    const int rr = sr / scale;
    const uint32_t *const src = (const uint32_t *)raw_image.constScanLine(rr);

    for (int sc = 0; sc < scaled_cols; sc++) {
      const int rc = sc / scale;

      dst[sc] = src[rc];
    }
  }

  this->ui->label_image->setPixmap(QPixmap::fromImage(scaled_img));

  VCL_destroy_block_model(md);
}

void BlockBrowser::on_pb_save_current_image_clicked() noexcept {
  static QString prev_dir = "";
  QString filename = QFileDialog::getSaveFileName(
      this, BlockBrowser::tr("保存当前图片"), prev_dir, "*.png;*.jpg");
  if (filename.isEmpty()) {
    return;
  }
  // update prev_dir
  {
    QFileInfo fi(filename);
    prev_dir = fi.dir().absolutePath();
  }

  const bool success = this->ui->label_image->pixmap().save(filename);

  if (!success) {
    QMessageBox::warning(
        this, BlockBrowser::tr("保存图片失败"),
        BlockBrowser::tr("不知道怎么回事，反正就是没存上。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok});
  }
}