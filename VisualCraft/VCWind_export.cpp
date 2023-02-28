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
    const QListWidgetItem *const qlwi = this->ui->lw_image_files->item(r);

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
      } else {
        qtwi->setText("0 %");
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

void VCWind::on_pb_execute_clicked() noexcept {
  this->setup_allowed_colorset();
  const auto opt = this->current_convert_option();
  bool success = true;

  for (int r = 0; r < this->ui->tw_build->rowCount(); r++) {
    const QString &image_filename = this->ui->tw_build->item(r, 0)->text();
    auto it = this->image_cache.find(image_filename);

    if (it == this->image_cache.end()) {
      QMessageBox::critical(
          this, VCWind::tr("致命逻辑错误"),
          VCWind::tr("导出页码表格中的图片\"%1\"不能在this->image_"
                     "cache中找到对应的缓存。请将这个错误反馈给软件开发者。")
              .arg(image_filename));
      abort();
    }
    const QString &converted_image_dest_path =
        this->ui->tw_build->item(r, VCWind::export_col_converted)->text();
    // not generated in this version
    const QString &diagram_dest =
        this->ui->tw_build->item(r, VCWind::export_col_flagdiagram)->text();
    const QString &lite_dest =
        this->ui->tw_build->item(r, VCWind::export_col_lite)->text();
    const QString &nbt_dest =
        this->ui->tw_build->item(r, VCWind::export_col_structure)->text();
    const QString &schem_dest =
        this->ui->tw_build->item(r, VCWind::export_col_schem)->text();

    const bool need_to_build =
        !(lite_dest.isEmpty() && nbt_dest.isEmpty() && schem_dest.isEmpty());
    const bool need_to_convert =
        need_to_build ||
        !(converted_image_dest_path.isEmpty() && diagram_dest.isEmpty());
    if (!need_to_convert) {
      this->ui->tw_build->item(r, VCWind::export_col_progress)
          ->setText("100 %");
      continue;
    }

    const int task_num =
        (need_to_convert) + (!converted_image_dest_path.isEmpty()) +
        (!diagram_dest.isEmpty()) + (need_to_build) + (!lite_dest.isEmpty()) +
        (!nbt_dest.isEmpty()) + (!schem_dest.isEmpty());
    int task_finished = 0;

    this->setup_image(it->second.first);

    success = this->kernel->convert(opt.algo, opt.dither);
    if (!success) {
      abort();
    }

    task_finished++;
    this->ui->tw_build->item(r, VCWind::export_col_progress)
        ->setText(
            QStringLiteral("%i %").arg(100.0f * task_finished / task_num));

    QImage new_img(it->second.first.height(), it->second.first.width(),
                   QImage::Format_ARGB32);
    this->kernel->converted_image((uint32_t *)new_img.scanLine(0), nullptr,
                                  nullptr, true);
    it->second.second = new_img;

    if (!converted_image_dest_path.isEmpty()) {
      success = new_img.save(converted_image_dest_path);
      if (!success) {
        const auto ret = QMessageBox::critical(
            this, VCWind::tr("保存转化后图像失败"),
            VCWind::tr("QImage未能生成\"%1\"。").arg(converted_image_dest_path),
            QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                         QMessageBox::StandardButton::Ignore},
            QMessageBox::StandardButton::Close);
        if (ret == QMessageBox::StandardButton::Ignore) {
          continue;
        } else {
          abort();
          return;
        }
      }
    }
    task_finished++;
    this->ui->tw_build->item(r, VCWind::export_col_progress)
        ->setText(
            QStringLiteral("%i %").arg(100.0f * task_finished / task_num));

    if (!need_to_build) {
      this->ui->tw_build->item(r, VCWind::export_col_progress)
          ->setText("100 %");
      continue;
    }

    success = this->kernel->build();
    if (!success) {

      const auto ret = QMessageBox::critical(
          this, VCWind::tr("构建三维结构失败"),
          VCWind::tr("VisualCraftL不能为图像\"%1\"构建三维结构。")
              .arg(image_filename),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                       QMessageBox::StandardButton::Ignore},
          QMessageBox::StandardButton::Close);
      if (ret == QMessageBox::StandardButton::Ignore) {
        continue;
      } else {
        abort();
        return;
      }
    }
    task_finished++;
    this->ui->tw_build->item(r, VCWind::export_col_progress)
        ->setText(
            QStringLiteral("%i %").arg(100.0f * task_finished / task_num));

    if (!lite_dest.isEmpty()) {
      success = this->kernel->export_litematic(
          lite_dest.toLocal8Bit().data(),
          this->ui->pte_lite_name->toPlainText().toUtf8().data(),
          this->ui->pte_lite_regionname->toPlainText().toUtf8().data());
      if (!success) {
        const auto ret = QMessageBox::critical(
            this, VCWind::tr("导出litematica失败"),
            VCWind::tr("VisualCraftL不能为图像\"%1\"生成投影文件\"%2\"。")
                .arg(image_filename)
                .arg(lite_dest),
            QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                         QMessageBox::StandardButton::Ignore},
            QMessageBox::StandardButton::Close);
        if (ret == QMessageBox::StandardButton::Ignore) {
          continue;
        } else {
          abort();
          return;
        }
      }
      task_finished++;
      this->ui->tw_build->item(r, VCWind::export_col_progress)
          ->setText(
              QStringLiteral("%i %").arg(100.0f * task_finished / task_num));
    }

    if (!nbt_dest.isEmpty()) {
      success = this->kernel->export_structure(
          nbt_dest.toLocal8Bit().data(),
          this->ui->cb_structure_is_air_void->isChecked());
      if (!success) {
        const auto ret = QMessageBox::critical(
            this, VCWind::tr("导出原版结构方块文件失败"),
            VCWind::tr("VisualCraftL不能为图像\"%1\"生成结构方块文件\"%2\"。")
                .arg(image_filename)
                .arg(nbt_dest),
            QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                         QMessageBox::StandardButton::Ignore},
            QMessageBox::StandardButton::Close);
        if (ret == QMessageBox::StandardButton::Ignore) {
          continue;
        } else {
          abort();
          return;
        }
      }
      task_finished++;
      this->ui->tw_build->item(r, VCWind::export_col_progress)
          ->setText(
              QStringLiteral("%i %").arg(100.0f * task_finished / task_num));
    }

    if (!schem_dest.isEmpty()) {
      const int offset[3] = {this->ui->sb_offset_x->value(),
                             this->ui->sb_offset_y->value(),
                             this->ui->sb_offset_z->value()};
      const int weoffset[3] = {this->ui->sb_weoffset_x->value(),
                               this->ui->sb_weoffset_y->value(),
                               this->ui->sb_weoffset_z->value()};
      QString mods_str = this->ui->pte_weshem_mods->toPlainText();
      std::vector<QByteArray> mods;
      std::vector<const char *> mods_charp;
      {
        QStringList mods_q = mods_str.split('\n');
        for (auto &qstr : mods_q) {
          mods.emplace_back(qstr.toUtf8());
          mods_charp.emplace_back(mods.back().data());
        }
      }

      success = this->kernel->export_WESchem(
          schem_dest.toLocal8Bit().data(), offset, weoffset,
          this->ui->pte_weschem_name->toPlainText().toUtf8().data(),
          mods_charp.data(), mods_charp.size());
      if (!success) {
        const auto ret = QMessageBox::critical(
            this, VCWind::tr("导出World Edit原理图失败"),
            VCWind::tr(
                "VisualCraftL不能为图像\"%1\"生成World Edit原理图\"%2\"。")
                .arg(image_filename)
                .arg(schem_dest),
            QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                         QMessageBox::StandardButton::Ignore},
            QMessageBox::StandardButton::Close);
        if (ret == QMessageBox::StandardButton::Ignore) {
          continue;
        } else {
          abort();
          return;
        }
      }

      task_finished++;
      this->ui->tw_build->item(r, VCWind::export_col_progress)
          ->setText(
              QStringLiteral("%i %").arg(100.0f * task_finished / task_num));
    }
    this->ui->tw_build->item(r, VCWind::export_col_progress)->setText("100 %");
  }

  for (auto &pair : this->image_cache) {
    this->setup_image(pair.second.first);
  }
}