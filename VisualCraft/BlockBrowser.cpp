/*
 Copyright © 2021-2023  TokiNoBug
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
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "BlockBrowser.h"
#include "VCWind.h"
#include "VC_block_class.h"
#include "ui_BlockBrowser.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <magic_enum.hpp>

BlockBrowser::BlockBrowser(QWidget *parent)
    : QWidget(parent), ui(new Ui::BlockBrowser) {
  ui->setupUi(this);

  this->fetch_content();

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

void BlockBrowser::fetch_content() noexcept {
  // fetch content for available blocks
  this->ui->combobox_select_blk->clear();
  this->ui->combobox_select_face->clear();

  for (const auto &pair_class : this->parent()->block_class_widgets()) {
    for (const auto &pair_blk : pair_class.second->blocks_vector()) {
      this->ui->combobox_select_blk->addItem(
          QString::fromUtf8(
              VCL_get_block_name(pair_blk.first, ::is_language_ZH)),
          QVariant::fromValue((void *)pair_blk.first));
    }
  }

  for (auto face : magic_enum::enum_values<VCL_face_t>()) {
    auto str = magic_enum::enum_name(face);

    this->ui->combobox_select_face->addItem(str.data(), int(face));
  }

  // fecth content for all blocks

  const size_t num_blocks_all = VCL_get_blocks_from_block_state_list(
      VCL_get_block_state_list(), nullptr, 0);

  std::vector<VCL_block *> blks;
  blks.resize(num_blocks_all);

  const size_t num_blocks_all_2 = VCL_get_blocks_from_block_state_list(
      VCL_get_block_state_list(), blks.data(), num_blocks_all);

  if (num_blocks_all_2 != num_blocks_all) {
    // logical error
    QMessageBox::critical(
        this, BlockBrowser::tr("致命逻辑错误"),
        QStringLiteral("num_blocks_all_2(%1) != num_blocks_all(%2)")
            .arg(num_blocks_all)
            .arg(num_blocks_all_2));
    exit(4);
    return;
  }

  std::sort(blks.begin(), blks.end(), VCL_compare_block);

  for (VCL_block *blk : blks) {
    this->ui->combobox_select_blk_all->addItem(
        QString::fromUtf8(VCL_get_block_name(blk, ::is_language_ZH)),
        QVariant::fromValue((void *)blk));
  }
  {
    this->ui->tw_version->setRowCount(20 - 12 + 1);
    this->ui->tw_version->setColumnCount(2);
    // if (false)
    for (int r = 0; r < this->ui->tw_version->rowCount(); r++) {
      for (int c = 0; c < this->ui->tw_version->columnCount(); c++) {
        QTableWidgetItem *qtwi = new QTableWidgetItem;
        qtwi->setFlags(Qt::ItemFlags{Qt::ItemFlag::ItemIsEnabled,
                                     Qt::ItemFlag::ItemIsSelectable});
        if (c == 0) {
          qtwi->setText("1." + QString::number(r + 12));
        }
        this->ui->tw_version->setItem(r, c, qtwi);
      }
    }
  }

  {
    auto attributes = magic_enum::enum_values<VCL_block_attribute_t>();
    this->ui->tw_attribute->setRowCount(attributes.size());
    this->ui->tw_attribute->setColumnCount(2);
    // if (false)
    for (int r = 0; r < this->ui->tw_attribute->rowCount(); r++) {
      for (int c = 0; c < this->ui->tw_attribute->columnCount(); c++) {
        QTableWidgetItem *qtwi = new QTableWidgetItem;
        qtwi->setFlags(Qt::ItemFlags{Qt::ItemFlag::ItemIsEnabled,
                                     Qt::ItemFlag::ItemIsSelectable});

        if (c == 0) {
          qtwi->setText(magic_enum::enum_name(attributes[r]).data());
        }
        this->ui->tw_attribute->setItem(r, c, qtwi);
      }
    }
  }

  // this->clear_second_page_content();
}

void BlockBrowser::update_display() noexcept {
  if (this->ui->combobox_select_blk->currentIndex() < 0) {
    return;
  }

  VCL_block *const blk =
      (VCL_block *)this->ui->combobox_select_blk->currentData().value<void *>();

  for (int i = 0; i < this->ui->combobox_select_blk_all->count(); i++) {
    VCL_block *const blk2 =
        (VCL_block *)this->ui->combobox_select_blk_all->itemData(i)
            .value<void *>();
    if (blk2 == blk) {
      this->ui->combobox_select_blk_all->setCurrentIndex(i);
      break;
    }
  }

  if (this->ui->combobox_select_face->currentIndex() < 0) {
    return;
  }

  const int scale = this->ui->sb_scale_ratio->value();
  if (blk == nullptr) {
    abort();
  }

  this->ui->tb_view_id->setText(QString::fromUtf8(VCL_get_block_id(blk)));

  VCL_face_t face =
      this->ui->combobox_select_face->currentData().value<VCL_face_t>();

  VCL_model *md = VCL_get_block_model(blk, VCL_get_resource_pack());

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

  this->ui->label_image->setText("");
  this->ui->label_image->setPixmap(QPixmap::fromImage(scaled_img));
  this->ui->label_image->setAlignment(Qt::Alignment{
      Qt::AlignmentFlag::AlignHCenter, Qt::AlignmentFlag::AlignVCenter});

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

void BlockBrowser::on_combobox_select_blk_all_currentIndexChanged(
    int idx) noexcept {
  if (idx < 0) {
    return;
  }

  VCL_block *blk = (VCL_block *)this->ui->combobox_select_blk_all->currentData()
                       .value<void *>();
  assert(blk != nullptr);

  this->ui->label_show_block_class->setText(
      BlockBrowser::tr("方块类别：") +
      magic_enum::enum_name(VCL_get_block_class(blk)).data());

  this->ui->tb_block_name_EN_all->setText(
      QString::fromUtf8(VCL_get_block_name(blk, false)));

  this->ui->tb_block_name_ZH_all->setText(
      QString::fromUtf8(VCL_get_block_name(blk, true)));

  this->ui->tb_blockid_all->setText(VCL_get_block_id(blk));

  for (int v = 12; v <= 20; v++) {
    const int r = v - 12;
    QTableWidgetItem *qtwi = this->ui->tw_version->item(r, 1);
    assert(qtwi != nullptr);

    const bool is_suitable =
        VCL_is_block_suitable_for_version(blk, SCL_gameVersion(v));

    qtwi->setCheckState(is_suitable ? Qt::CheckState::Checked
                                    : Qt::CheckState::Unchecked);
    QString str{""};

    if (is_suitable) {
      str =
          QString::fromUtf8(VCL_get_block_id_version(blk, SCL_gameVersion(v)));
    }

    qtwi->setText(str);
  }

  auto attributes = magic_enum::enum_values<VCL_block_attribute_t>();

  for (size_t r = 0; r < attributes.size(); r++) {
    QTableWidgetItem *qtwi = this->ui->tw_attribute->item(r, 1);
    assert(qtwi != nullptr);

    qtwi->setCheckState(VCL_get_block_attribute(blk, attributes[r])
                            ? Qt::CheckState::Checked
                            : Qt::CheckState::Unchecked);
  }
}