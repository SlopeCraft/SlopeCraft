#include "SCWind.h"
#include "ui_SCWind.h"
#include <QFileDialog>
#include <QMessageBox>
#include <ranges>
#include <QApplication>

void SCWind::on_pb_add_image_clicked() noexcept {
  auto files =
      QFileDialog::getOpenFileNames(this, tr("选择图片"), "", "*.png;*.jpg");

  if (files.empty()) {
    return;
  }

  QString err;
  for (const auto &filename : files) {
    auto task = cvt_task::load(filename, err);

    if (!err.isEmpty()) {
      auto ret = QMessageBox::critical(
          this, tr("打开图像失败"),
          tr("无法打开图像 %1 。常见原因：图像尺寸太大。\n详细信息： %2")
              .arg(filename)
              .arg(err),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                       QMessageBox::StandardButton::Ignore});

      if (ret == QMessageBox::Ignore) {
        continue;
      } else {
        abort();
      }
    }

    this->tasks.emplace_back(task);
  }

  emit this->image_changed();
  if (this->ui->lview_pool_cvt->viewMode() == QListView::ViewMode::IconMode) {
    this->ui->lview_pool_cvt->doItemsLayout();
  }
}

void SCWind::on_pb_remove_image_clicked() noexcept {
  auto selected = this->ui->lview_pool_cvt->selectionModel()->selectedIndexes();

  if (selected.empty()) {
    return;
  }

  for (const auto &qmi : selected) {
    const int idx = qmi.row();
    this->tasks.erase(this->tasks.begin() + idx);
  }

  emit this->image_changed();
}

void SCWind::on_cb_lv_cvt_icon_mode_clicked() noexcept {
  const bool is_icon_mode = this->ui->cb_lv_cvt_icon_mode->isChecked();
  this->ui->lview_pool_cvt->setViewMode((is_icon_mode)
                                            ? (QListView::ViewMode::IconMode)
                                            : (QListView::ViewMode::ListMode));

  this->ui->lview_pool_cvt->setFlow(QListView::Flow::TopToBottom);

  this->ui->lview_pool_cvt->setSpacing(is_icon_mode ? 16 : 0);

  this->cvt_pool_model->refresh();
}

void SCWind::on_pb_load_preset_clicked() noexcept {
  static QString prev_dir{""};
  QString file = QFileDialog::getOpenFileName(this, tr("选择预设文件"),
                                              prev_dir, "*.sc_preset_json");

  if (file.isEmpty()) {
    return;
  }

  prev_dir = QFileInfo{file}.dir().canonicalPath();
  QString err;
  blockListPreset preset = load_preset(file, err);
  if (!err.isEmpty()) {
    QMessageBox::warning(this, tr("解析预设文件失败"),
                         tr("预设文件%1存在错误：%2").arg(file).arg(err));
    return;
  }

  this->ui->blm->loadPreset(preset);
}

void SCWind::on_pb_save_preset_clicked() noexcept {
  static QString prev_dir{""};
  QString file = QFileDialog::getSaveFileName(this, tr("保存当前预设"),
                                              prev_dir, "*.sc_preset_json");

  if (file.isEmpty()) {
    return;
  }

  prev_dir = QFileInfo{file}.dir().canonicalPath();

  blockListPreset preset = this->ui->blm->to_preset();
  {
    QString str = serialize_preset(preset);

    QFile qf{file};
    qf.open(QFile::OpenMode{QIODevice::WriteOnly | QIODevice::Text});

    if (!qf.isOpen()) {
      QMessageBox::warning(this, tr("保存预设文件失败"),
                           tr("无法生成预设文件%1 ，错误信息：%2")
                               .arg(file)
                               .arg(qf.errorString()));
      return;
    }

    qf.write(str.toUtf8());
    qf.close();
  }
}

inline int impl_select_blk_by_id(
    const std::vector<const SlopeCraft::AbstractBlock *> &blks,
    std::string_view keyword) noexcept {
  int result = -1;
  for (int idx = 0; idx < int(blks.size()); idx++) {
    std::string_view id{blks[idx]->getId()};
    const auto find = id.find(keyword);

    if (find != std::string_view::npos) {
      result = idx;
      break;
    }
  }

  return result;
}

void SCWind::on_pb_prefer_concrete_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::AbstractBlock *> &blks) -> int {
        return impl_select_blk_by_id(blks, "concrete");
      });
}

void SCWind::on_pb_prefer_wool_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::AbstractBlock *> &blks) -> int {
        return impl_select_blk_by_id(blks, "wool");
      });
}

void SCWind::on_pb_prefer_glass_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::AbstractBlock *> &blks) -> int {
        return impl_select_blk_by_id(blks, "stained_glass");
      });
}

void SCWind::on_pb_prefer_planks_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::AbstractBlock *> &blks) -> int {
        return impl_select_blk_by_id(blks, "planks");
      });
}

void SCWind::on_pb_prefer_logs_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::AbstractBlock *> &blks) -> int {
        return impl_select_blk_by_id(blks, "log");
      });
}

void SCWind::on_pb_cvt_current_clicked() noexcept {
  const auto sel = this->selected_cvt_task_idx();

  if (!sel.has_value()) {
    QMessageBox::warning(this, tr("未选择图像"),
                         tr("请在左侧任务池选择一个图像"));
    return;
  }

  this->kernel_set_image(sel.value());
  this->kernel_convert_image();
  this->tasks[sel.value()].set_converted();

  this->kernel_make_cvt_cache();
  this->refresh_current_cvt_display(sel.value(), true);
  this->ui->tw_cvt_image->setCurrentIndex(1);

  emit this->image_changed();
  // this->ui->
}

void SCWind::on_pb_cvt_all_clicked() noexcept {
  for (int idx = 0; idx < (int)this->tasks.size(); idx++) {
    auto &task = this->tasks[idx];
    if (task.is_converted) {
      continue;
    }

    this->kernel_set_image(idx);
    this->kernel_convert_image();
    this->kernel_make_cvt_cache();
    task.set_converted();
  }
  emit this->image_changed();
}

void SCWind::on_pb_save_converted_clicked() noexcept {
  const auto selected = this->selected_indices();
  if (selected.size() <= 0) {
    QMessageBox::warning(this, tr("未选择图像"),
                         tr("请在左侧任务池选择一个或多个图像"));
    return;
  }
  static QString prev_dir{""};
  if (selected.size() == 1) {
    QString filename = QFileDialog::getSaveFileName(this, tr("保存转化后图像"),
                                                    prev_dir, "*.png;*.jpg");
    if (filename.isEmpty()) {
      return;
    }
    prev_dir = QFileInfo{filename}.dir().dirName();

    const int idx = selected.front();

    this->export_current_cvted_image(idx, filename);
    return;
  }

  QString out_dir =
      QFileDialog::getExistingDirectory(this, tr("保存转化后图像"), prev_dir);
  if (out_dir.isEmpty()) {
    return;
  }
  prev_dir = out_dir;

  bool yes_to_all_replace_existing{false};
  bool no_to_all_replace_existing{false};
  for (int idx : selected) {
    auto &task = this->tasks[idx];
    QString filename = QStringLiteral("%1/%2.png")
                           .arg(out_dir)
                           .arg(QFileInfo{task.filename}.baseName());

    if (QFile{filename}.exists()) {
      if (no_to_all_replace_existing) {
        continue;
      }
      if (yes_to_all_replace_existing) {
        goto save_image;
      }

      auto ret = QMessageBox::warning(
          this, tr("将要覆盖已存在的图像"),
          tr("%1将被覆盖，确认覆盖吗？").arg(filename),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Yes,
                                       QMessageBox::StandardButton::No,
                                       QMessageBox::StandardButton::YesToAll,
                                       QMessageBox::StandardButton::NoToAll});
      bool replace{false};
      switch (ret) {
        case QMessageBox::StandardButton::Yes:
          replace = true;
          break;
        case QMessageBox::StandardButton::YesToAll:
          replace = true;
          yes_to_all_replace_existing = true;
          break;
        case QMessageBox::StandardButton::NoToAll:
          replace = false;
          no_to_all_replace_existing = true;
          break;
        default:
          replace = false;
          break;
      }

      if (!replace) {
        continue;
      }
    }

  save_image:

    this->export_current_cvted_image(idx, filename);
  }
}

void SCWind::on_cb_compress_lossy_toggled(bool checked) noexcept {
  this->ui->sb_max_height->setEnabled(checked);
}

void SCWind::on_pb_build3d_clicked() noexcept {
  auto taskopt = this->selected_export_task();
  if (!taskopt.has_value()) {
    QMessageBox::warning(this, tr("未选择图像"),
                         tr("请在左侧任务池选择一个图像"));
    return;
  }
  assert(taskopt.value() != nullptr);

  cvt_task &task = *taskopt.value();

  if (!task.is_converted) [[unlikely]] {
    QMessageBox::warning(this, tr("该图像尚未被转化"),
                         tr("必须先转化一个图像，然后再为它构建三维结构"));
    return;
  }

  this->kernel_build_3d();
  this->kernel_make_build_cache();

  task.set_built();
  this->refresh_current_build_display(&task, true);
}

void SCWind::on_pb_preview_materials_clicked() noexcept {
  auto taskopt = this->selected_export_task();
  if (!taskopt.has_value()) {
    QMessageBox::warning(this, tr("未选择图像"),
                         tr("请在左侧任务池选择一个图像"));
    return;
  }
  assert(taskopt.value() != nullptr);

  cvt_task &task = *taskopt.value();
  if (!task.is_converted) [[unlikely]] {
    QMessageBox::warning(this, tr("该图像尚未被转化"),
                         tr("必须先转化一个图像，然后再为它构建三维结构"));
    return;
  }
  QString errtitle;
  QString errmsg;
  // try to load cache
  [this, &errtitle, &errmsg]() {
    if (this->kernel->queryStep() >= SCL_step::builded) {
      return;
    }
    if (!this->kernel->loadConvertCache(this->selected_algo(),
                                        this->is_dither_selected())) {
      errtitle = tr("该图像尚未被转化");
      errmsg =
          tr("可能是在转化完成之后又修改了转化算法，因此之前的转化无效。必须重"
             "新转化该图像。");
      return;
    }
    if (!this->kernel->loadBuildCache(this->current_build_option())) {
      errtitle = tr("尚未构建三维结构");
      errmsg = tr(
          "在预览材料表之前，必须先构建三维结构。出现这个警告，可能是因为你"
          "在构建三维结构之后，又修改了三维结构的选项，因此之前的结果无效。");
    }
  }();

  if (!errtitle.isEmpty()) {
    QMessageBox::warning(this, errtitle, errmsg);
    return;
  }

#warning show mat list here
}

void SCWind::on_pb_export_all_clicked() noexcept {
  auto tasks_to_export = this->selected_export_task_list();

  if (tasks_to_export.size() <= 0) {
    QMessageBox::warning(this, tr("无可导出的任务"),
                         tr("任务池为空，请先转化一个或一些图像"));
    return;
  }
  QString dir;
  {
    static QString prev_dir{""};
    dir = QFileDialog::getExistingDirectory(this, tr("选择导出位置"), prev_dir,
                                            QFileDialog::Options{});
    if (dir.isEmpty()) {
      return;
    }
    prev_dir = dir;
  }

  auto get_export_name = [dir, this](const cvt_task &t) -> QString {
    return QStringLiteral("%1/%2.%3")
        .arg(dir)
        .arg(QFileInfo{t.filename}.baseName())
        .arg(extension_of_export_type(this->selected_export_type()));
  };

  // warn if some files will be covered
  {
    QString warning_list{};

    for (auto taskp : tasks_to_export) {
      QString filename = get_export_name(*taskp);
      if (QFile{filename}.exists()) {
        if (warning_list.isEmpty()) {
          warning_list = filename;
        } else {
          warning_list.append('\n');
          warning_list.append(filename);
        }
      }
    }

    if (!warning_list.isEmpty()) {
      auto ret = QMessageBox::warning(
          this, tr("将要覆盖已经存在的文件"),
          tr("确定要覆盖这些文件吗？以下文件将被覆盖：\n%1").arg(warning_list),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Yes,
                                       QMessageBox::StandardButton::No});
      if (ret != QMessageBox::StandardButton::Yes) {
        return;
      }
    }
  }

  for (auto taskp : tasks_to_export) {
    auto set_img_and_convert = [this, taskp]() {
      this->kernel_set_image(taskp - this->tasks.data());
      this->kernel_convert_image();
      taskp->set_converted();
    };

    // this step make sure the image is converted in kernel
    if (!taskp->is_converted) {
      set_img_and_convert();
    } else {
      if (!this->kernel->loadConvertCache(this->selected_algo(),
                                          this->is_dither_selected())) {
        set_img_and_convert();
      }
    }

    if (!taskp->is_built) {
      this->kernel_build_3d();
    } else {
      if (!this->kernel->loadBuildCache(this->current_build_option())) {
        this->kernel_build_3d();
      }
    }
    taskp->set_built();

#warning export here
  }
}