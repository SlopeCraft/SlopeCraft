#include "VCWind.h"
#include "ui_VCWind.h"

#include <QFileDialog>
#include <QMessageBox>

QString VCWind::get_dir_for_export() noexcept {
  static QString prev{""};
  QString dir = QFileDialog::getExistingDirectory(
      this, VCWind::tr("选择输出位置"), prev, QFileDialog::Option::ReadOnly);
  if (!dir.isEmpty()) {
    prev = dir;
  }
  return dir;
}

void VCWind::flush_export_tabel() noexcept {
  {
    const int rows = this->ui->tw_build->rowCount();
    for (int r = 0; r < rows; r++) {
      this->ui->tw_build->removeRow(r);
    }
  }
  const int num_images = this->ui->lw_image_files->count();
  this->ui->tw_build->setRowCount(num_images);

  for (int r = 0; r < num_images; r++) {
    QListWidgetItem *const qlwi = this->ui->lw_image_files->item(r);

    auto it = this->image_cache.find(qlwi->text());
    assert(it != this->image_cache.end());

    // col 0, image filename
    {
      QTableWidgetItem *qtwi = new QTableWidgetItem;
      qtwi->setText(qlwi->text());
      qtwi->setFlags(Qt::ItemFlag::ItemIsEnabled |
                     Qt::ItemFlag::ItemIsSelectable);
      this->ui->tw_build->setItem(r, export_col_filename, qtwi);
    }

    // col 1, image size
    {
      QTableWidgetItem *qtwi = new QTableWidgetItem;

      qtwi->setText(VCWind::tr("%1, %2")
                        .arg(it->second.first.height())
                        .arg(it->second.first.width()));
      qtwi->setFlags(Qt::ItemFlag::ItemIsEnabled |
                     Qt::ItemFlag::ItemIsSelectable);
      this->ui->tw_build->setItem(r, export_col_imagesize, qtwi);
    }

    for (int c = 2; c < 8; c++) {
      QTableWidgetItem *qtwi = new QTableWidgetItem("");
      auto flag = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable;

      // col 7 is the convert progress, which is not editable
      if (c != export_col_progress) {
        flag |= Qt::ItemFlag::ItemIsEditable;
      }
      qtwi->setFlags(flag);
      this->ui->tw_build->setItem(r, c, qtwi);
    }
  }
}

void VCWind::on_pb_select_export_dir_clicked() noexcept {

  if (this->ui->combobox_export_type->currentIndex() < 0) {
    QMessageBox::warning(
        this, VCWind::tr("错误操作"),
        VCWind::tr(
            "设置任何一种导出类型的输出位置时，都需要在左侧的combo "
            "box中选择一个导出类型。请先选择一种导出类型，再设置导出位置。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok});
    return;
  }

  QString dir = this->get_dir_for_export();

  if (dir.isEmpty()) {
    return;
  }

  QString suffix;
  bool strip_image_extension = true;
  int dest_col = -1;
  switch (this->ui->combobox_export_type->currentIndex()) {
  case 0:
    suffix = ".litematic";
    dest_col = VCWind::export_col_lite;
    break;
  case 1:
    suffix = ".nbt";
    dest_col = VCWind::export_col_structure;
    break;
  case 2:
    suffix = ".schem";
    dest_col = VCWind::export_col_schem;
    break;
  case 3:
    suffix = "";
    dest_col = VCWind::export_col_converted;
    strip_image_extension = false;
    break;
  case 4:
    suffix = ".png";
    dest_col = VCWind::export_col_flagdiagram;
    break;
  default:
    abort();
    return;
  }

  const int c = dest_col;
  for (int r = 0; r < this->ui->tw_build->rowCount(); r++) {
    QFileInfo finfo(this->ui->tw_build->item(r, 0)->text());

    QString pure_name;

    if (strip_image_extension) {
      pure_name = finfo.baseName();
    } else {
      pure_name = finfo.fileName();
    }

    QString filename = dir + '/' + pure_name + suffix;

    this->ui->tw_build->item(r, c)->setText(filename);
  }
}

void VCWind::on_pb_execute_clicked() noexcept {}