#include "SCWind.h"
#include "ui_SCWind.h"
#include <QFileDialog>
#include <QMessageBox>
#include <ranges>
#include <QApplication>
#include <QDesktopServices>
#include <QRgb>

#include <magic_enum.hpp>

#include "PreviewWind.h"
#include "AiCvterParameterDialog.h"
#include "VersionDialog.h"
#include "TransparentStrategyWind.h"
#include "CompressEffectViewer.h"
#include "BlockListDialog.h"
#include "stat_memory.h"

#ifdef WIN32
const char *SC_image_filter = "*.png;*.jpg";
#else
const char *SC_image_filter = "*.png;;*.jpg";
#endif

void SCWind::on_pb_add_image_clicked() noexcept {
  auto files = QFileDialog::getOpenFileNames(
      this, tr("选择图片"), this->prev_load_image_dir, SC_image_filter);

  if (files.empty()) {
    return;
  }
  this->prev_load_image_dir = QFileInfo{files.front()}.dir().absolutePath();

  std::optional<TransparentStrategyWind::strategy> strategy_opt{std::nullopt};

  for (const auto &filename : files) {
    auto task_res = cvt_task::load(filename);

    if (!task_res) {
      auto ret = QMessageBox::critical(
          this, tr("打开图像失败"),
          tr("无法打开图像 %1。常见原因：图像尺寸太大。\n详细信息： %2")
              .arg(filename, task_res.error()),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                       QMessageBox::StandardButton::Ignore});

      if (ret == QMessageBox::Ignore) {
        continue;
      } else {
        abort();
      }
    }

    auto task = std::move(task_res.value());
    // have transparent pixels
    if (SlopeCraft::SCL_haveTransparentPixel(
            (const uint32_t *)task.original_image.scanLine(0),
            task.original_image.sizeInBytes() / sizeof(uint32_t))) {
      if (!strategy_opt.has_value()) {
        strategy_opt = TransparentStrategyWind::ask_for_strategy(this);
      }

      if (!strategy_opt.has_value()) {
        continue;
      }
      const auto &st = strategy_opt.value();
      SlopeCraft::SCL_preprocessImage(
          (uint32_t *)task.original_image.scanLine(0),
          task.original_image.sizeInBytes() / sizeof(uint32_t),
          st.pure_transparent, st.half_transparent, st.background_color);
    }

    this->tasks.emplace_back(std::move(task));
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

  std::vector<int> row;
  {
    row.reserve(selected.size());
    for (const auto &qmi : selected) {
      row.emplace_back(qmi.row());
    }
    std::sort(row.begin(), row.end(), [](int a, int b) { return a > b; });
  }

  for (const int idx : row) {
    this->tasks.erase(this->tasks.begin() + idx);
  }

  emit this->image_changed();
}

void SCWind::on_pb_replace_image_clicked() noexcept {
  auto selected = this->ui->lview_pool_cvt->selectionModel()->selectedIndexes();
  if (selected.empty()) {
    QMessageBox::warning(this, tr("请选择将被替换的图像"),
                         tr("必须先选择一个或多个图像，然后才能替换它们。"));
    return;
  }

  const QString file = QFileDialog::getOpenFileName(
      this, tr("选择图片"), this->prev_load_image_dir, SC_image_filter);
  if (file.isEmpty()) {
    return;
  }
  this->prev_load_image_dir = QFileInfo{file}.dir().absolutePath();

  auto task_res = cvt_task::load(file);
  if (!task_res) {
    auto ret = QMessageBox::critical(
        this, tr("打开图像失败"),
        tr("无法打开图像 %1。常见原因：图像尺寸太大。\n详细信息： %2")
            .arg(file, task_res.error()),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                     QMessageBox::StandardButton::Ignore});

    if (ret == QMessageBox::Ignore) {
      return;
    } else {
      abort();
    }
  }
  for (const auto &qmi : selected) {
    this->tasks[qmi.row()] = std::move(task_res.value());
  }
  this->cvt_pool_model->refresh();
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
                         tr("预设文件%1存在错误：%2").arg(file, err));
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
      QMessageBox::warning(
          this, tr("保存预设文件失败"),
          tr("无法生成预设文件%1，错误信息：%2").arg(file, qf.errorString()));
      return;
    }

    qf.write(str.toUtf8());
    qf.close();
  }
}

inline int impl_select_blk_by_id(
    const std::vector<const SlopeCraft::mc_block_interface *> &blks,
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
      [](const std::vector<const SlopeCraft::mc_block_interface *> &blks)
          -> int { return impl_select_blk_by_id(blks, "concrete"); });
}

void SCWind::on_pb_prefer_wool_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::mc_block_interface *> &blks)
          -> int { return impl_select_blk_by_id(blks, "wool"); });
}

void SCWind::on_pb_prefer_glass_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::mc_block_interface *> &blks)
          -> int { return impl_select_blk_by_id(blks, "stained_glass"); });
}

void SCWind::on_pb_prefer_planks_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::mc_block_interface *> &blks)
          -> int { return impl_select_blk_by_id(blks, "planks"); });
}

void SCWind::on_pb_prefer_logs_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::mc_block_interface *> &blks)
          -> int { return impl_select_blk_by_id(blks, "log"); });
}

void SCWind::on_pb_prefer_slabs_clicked() noexcept {
  this->ui->blm->select_block_by_callback(
      [](const std::vector<const SlopeCraft::mc_block_interface *> &blks)
          -> int { return impl_select_blk_by_id(blks, "_slab"); });
}

void SCWind::on_pb_select_all_clicked() noexcept {
  for (size_t bc = 0; bc < this->ui->blm->num_basecolor_widgets(); bc++) {
    this->ui->blm->basecolorwidget_at(bc)->set_enabled(true);
  }
}
void SCWind::on_pb_deselect_all_clicked() noexcept {
  for (size_t bc = 1; bc < this->ui->blm->num_basecolor_widgets(); bc++) {
    this->ui->blm->basecolorwidget_at(bc)->set_enabled(false);
  }
}
void SCWind::on_pb_invselect_clicked() noexcept {
  for (size_t bc = 1; bc < this->ui->blm->num_basecolor_widgets(); bc++) {
    BaseColorWidget *bcw = this->ui->blm->basecolorwidget_at(bc);
    assert(bcw not_eq nullptr);
    bcw->set_enabled(not(bcw->is_enabled()));
  }
}

void SCWind::on_pb_cvt_current_clicked() noexcept {
  const auto sel = this->selected_cvt_task_idx();

  if (!sel.has_value()) {
    QMessageBox::warning(this, tr("未选择图像"),
                         tr("请在左侧任务池选择一个图像"));
    return;
  }

  {
    auto cvted = this->convert_image(sel.value());
    if (!cvted) {
      return;
    }
    this->tasks[sel.value()].set_converted(this->current_color_table(),
                                           this->current_convert_option(),
                                           std::move(cvted));
  }

  //  this->kernel_make_cvt_cache();
  this->refresh_current_cvt_display(sel.value());
  this->ui->tw_cvt_image->setCurrentIndex(1);

  emit this->image_changed();
  // this->ui->
}

void SCWind::on_pb_cvt_all_clicked() noexcept {
  for (int idx = 0; idx < (int)this->tasks.size(); idx++) {
    auto &task = this->tasks[idx];
    if (task.is_converted_with(this->current_color_table(),
                               this->current_convert_option())) {
      continue;
    }
    auto cvted = this->convert_image(idx);
    task.set_converted(this->current_color_table(),
                       this->current_convert_option(), std::move(cvted));
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
                           .arg(out_dir, QFileInfo{task.filename}.baseName());

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
      bool replace;
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
  if (this->should_auto_cache(false)) {
    [[maybe_unused]] const auto report = this->auto_cache_3D();
    ;
  }

  auto task_ptr = this->selected_export_task();
  if (task_ptr == nullptr) {
    QMessageBox::warning(this, tr("未选择图像"),
                         tr("请在左侧任务池选择一个图像"));
    return;
  }
  assert(task_ptr != nullptr);

  cvt_task &task = *task_ptr;

  if (!task.is_converted_with(this->current_color_table(),
                              this->current_convert_option())) [[unlikely]] {
    QMessageBox::warning(this, tr("该图像尚未被转化"),
                         tr("必须先转化一个图像，然后再为它构建三维结构"));
    return;
  }

  const int gidx = task_ptr - this->tasks.data();
  auto table = this->current_color_table();
  const auto cvt_option = this->current_convert_option();
  if (!task.is_converted_with(table, cvt_option)) {
    task.set_converted(table, cvt_option, this->convert_image(gidx));
  }
  auto &cvted =
      task.converted_images.find(convert_input{table, cvt_option})->second;
  //    this->kernel_set_image(gidx);
  //    if (!this->kernel->loadConvertCache(this->selected_algo(),
  //                                        this->is_dither_selected())) {
  //      this->kernel_convert_image();
  //    }

  const auto build_option = this->current_build_option();
  if (!cvted.is_built_with(build_option)) {
    auto structure = this->build_3D(*cvted.converted_image);
    if (structure not_eq nullptr) {
      cvted.set_built(build_option, std::move(structure));
    }
  }
  // load cache if converted
  this->refresh_current_build_display(&task);
}

void SCWind::on_pb_preview_materials_clicked() noexcept {
  if (this->should_auto_cache(false)) {
    this->auto_cache_3D();
  }

  auto [cvted_img, structure_3D] = this->load_selected_3D();
  if (cvted_img == nullptr || structure_3D == nullptr) {
    return;
  }

  PreviewWind *pw = new PreviewWind{this};

  pw->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  pw->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  pw->setWindowFlag(Qt::WindowType::Window, true);

  // connect(this, &VCWind::signal_allowed_colorset_changed, pw,
  // &QWidget::deleteLater);

  pw->show();

  pw->setup_data(*this->current_color_table(), *structure_3D);
}

void SCWind::on_pb_preview_compress_effect_clicked() noexcept {
  if (this->should_auto_cache(false)) {
    this->auto_cache_3D();
  }

  auto [cvted_img, structure_3D] = this->load_selected_3D();
  if (cvted_img == nullptr || structure_3D == nullptr) {
    return;
  }

  CompressEffectViewer *cev =
      new CompressEffectViewer{this, *cvted_img, *structure_3D};

  cev->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  cev->setWindowFlag(Qt::WindowType::Window, true);
  // cev->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  //  bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  cev->show();
}

#define SC_PRIVATE_MACRO_PROCESS_IF_ERR_on_pb_export_all_clicked \
  if (!temp.has_value()) {                                       \
    process_err(err);                                            \
    return;                                                      \
  }

#define SC_PRIVATE_MARCO_PROCESS_EXPORT_ERROR_on_pb_export_all_clicked \
  if (report_err_fun(export_name, taskp) ==                            \
      QMessageBox::StandardButton::Ignore) {                           \
    continue;                                                          \
  } else {                                                             \
    return;                                                            \
  }

void SCWind::on_pb_export_all_clicked() noexcept {
  {
    auto ctable = this->current_color_table();
    if (ctable == nullptr) {
      QMessageBox::critical(
          this, tr("没有可用颜色"),
          tr("没有勾选任何颜色，无法转化图像。请至少勾选3~16种颜色。"));
      return;
    }
  }

  auto tasks_to_export = this->selected_export_task_list();

  const auto export_type = this->selected_export_type();
  SlopeCraft::litematic_options opt_lite;
  SlopeCraft::vanilla_structure_options opt_nbt;
  SlopeCraft::WE_schem_options opt_WE;
  SlopeCraft::flag_diagram_options opt_fd;
  {
    QString err;
    auto process_err = [this](const QString &err) {
      QMessageBox::warning(this, tr("导出设置有错"),
                           tr("导出设置存在如下错误：\n%1").arg(err));
    };
    switch (export_type) {
      case export_type::litematica: {
        auto temp = this->current_litematic_option(err);
        SC_PRIVATE_MACRO_PROCESS_IF_ERR_on_pb_export_all_clicked;
        opt_lite = temp.value();
        break;
      }
      case export_type::vanilla_structure: {
        auto temp = this->current_nbt_option(err);
        SC_PRIVATE_MACRO_PROCESS_IF_ERR_on_pb_export_all_clicked;
        opt_nbt = temp.value();
        break;
      }
      case export_type::WE_schem: {
        auto temp = this->current_schem_option(err);
        SC_PRIVATE_MACRO_PROCESS_IF_ERR_on_pb_export_all_clicked;
        opt_WE = temp.value();
        break;
      }
      case export_type::flat_diagram: {
        auto temp = this->current_flatdiagram_option(err);
        SC_PRIVATE_MACRO_PROCESS_IF_ERR_on_pb_export_all_clicked;
        opt_fd = temp.value();
        break;
      }
      default:
        QMessageBox::warning(this, tr("你点错按钮了"),
                             tr("导出为纯文件地图画的按钮在另外一页。按理来说"
                                "你不应该能点击这个"
                                "按钮，这可能是一个小小的 bug（特性）。"));
        return;
    }
  }

  if (tasks_to_export.empty()) {
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
        .arg(dir, QFileInfo{t.filename}.baseName(),
             extension_of_export_type(this->selected_export_type()));
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
    if (this->should_auto_cache(false)) {
      this->auto_cache_3D();
    }

    auto [cvted, str3D] = this->convert_and_build_if_need(*taskp);

    const std::string export_name =
        get_export_name(*taskp).toLocal8Bit().data();

    auto report_err_fun = [this](std::string_view export_name,
                                 const cvt_task *taskp) {
      return QMessageBox::warning(
          this, tr("导出失败"),
          tr("导出%1时失败。原图像文件名为%"
             "2\n点击 Ignore 将跳过这个图像，点击 Cancel 将放弃导出任务。")
              .arg(export_name.data(), taskp->filename),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore,
                                       QMessageBox::StandardButton::Cancel});
    };

    switch (export_type) {
      case export_type::litematica:
        if (!str3D.export_litematica(export_name.c_str(), opt_lite)) {
          SC_PRIVATE_MARCO_PROCESS_EXPORT_ERROR_on_pb_export_all_clicked;
        }
        break;

      case export_type::vanilla_structure:
        if (!str3D.export_vanilla_structure(export_name.c_str(), opt_nbt)) {
          SC_PRIVATE_MARCO_PROCESS_EXPORT_ERROR_on_pb_export_all_clicked;
        }
        break;
      case export_type::WE_schem:
        if (!str3D.export_WE_schem(export_name.c_str(), opt_WE)) {
          SC_PRIVATE_MARCO_PROCESS_EXPORT_ERROR_on_pb_export_all_clicked;
        }
        break;
      case export_type::flat_diagram:
        if (!str3D.export_flat_diagram(export_name.c_str(),
                                       *this->current_color_table(), opt_fd)) {
          SC_PRIVATE_MARCO_PROCESS_EXPORT_ERROR_on_pb_export_all_clicked;
        }
        break;
      default:
        return;
    }
  }
}

void SCWind::on_pb_export_file_clicked() noexcept {
  static QString prev_dir{""};
  const QString dir =
      QFileDialog::getExistingDirectory(this, tr("设置导出位置"), prev_dir);

  if (dir.isEmpty()) {
    return;
  }
  prev_dir = dir;

  const int seq_first = this->current_map_begin_seq_number();
  {
    int exisiting_num = 0;
    QString to_be_replaced{""};
    to_be_replaced.reserve(4096);

    const int seq_last =
        this->tasks.map_range_of(seq_first, this->tasks.size() - 1).last;

    for (int seq = seq_first; seq <= seq_last; seq++) {
      QString filename = map_data_filename(dir, seq);
      if (QFile{filename}.exists()) {
        if (exisiting_num > 0) {
          to_be_replaced.push_back('\n');
        }
        to_be_replaced.append(filename);
        exisiting_num++;
      }
    }

    if (exisiting_num > 0) {
      const auto ret = QMessageBox::warning(
          this, tr("%1 个文件将被替换").arg(exisiting_num),
          tr("以下文件将被替换：\n%1\n点击 Yes "
             "将替换它们，点击 No 将取消这次导出。")
              .arg(to_be_replaced),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Yes,
                                       QMessageBox::StandardButton::No});

      if (ret != QMessageBox::StandardButton::Yes) {
        return;
      }
    }
  }

  size_t fail_count = 0;
  QString fail_tasks = "";
  for (int idx = 0; idx < int(this->tasks.size()); idx++) {
    auto &task = this->tasks.at(idx);
    auto &cvted_img = this->convert_if_need(task);
    //    this->kernel_set_image(idx);
    //    bool need_to_convert{true};
    //    if (task.is_converted()) {
    //      if (this->kernel->loadConvertCache(this->selected_algo(),
    //                                         this->is_dither_selected())) {
    //        need_to_convert = false;
    //      }
    //    }
    //
    //    if (need_to_convert) {
    //      this->kernel_convert_image();
    //    }
    //
    const int cur_seq_beg = this->tasks.map_range_of(seq_first, idx).first;
    const auto dir_name = dir.toLocal8Bit();
    //
    const SlopeCraft::map_data_file_options option{
        .caller_api_version = SC_VERSION_U64,
        .folder_path = dir_name.data(),
        .begin_index = cur_seq_beg,
        .progress = progress_callback(this->ui->pbar_export),
        .ui = this->ui_callbacks(),
    };
    const bool ok = cvted_img.export_map_data(option);
    if (!ok) {
      fail_count++;
      fail_tasks.append(task.filename);
      fail_tasks.push_back('\n');
    }
  }

  if (fail_count > 0) {
    QMessageBox::warning(this, tr("%1 个图片导出失败").arg(fail_count),
                         tr("导出失败的图片依次为：\n%1").arg(fail_tasks));
  }
}

void SCWind::on_ac_GAcvter_options_triggered() noexcept {
  AiCvterParameterDialog *acpd = new AiCvterParameterDialog{this};

  acpd->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  acpd->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  acpd->setWindowFlag(Qt::WindowType::Window, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  acpd->show();
}

void SCWind::on_ac_cache_dir_open_triggered() noexcept {
  auto cache_dir = this->cache_root_dir();
  QDesktopServices::openUrl(QUrl::fromLocalFile(cache_dir));
}

void SCWind::on_ac_clear_cache_triggered() noexcept {
  const QString cache_dir_name = this->cache_root_dir();
  QDir cache_dir{cache_dir_name};

  if (!cache_dir.exists()) {
    return;
  }

  emit this->image_changed();

  const auto entries = cache_dir.entryList();

  auto remove_cache_fun = [](QString filename) -> bool {
    if (QFileInfo{filename}.isFile()) {
      return QFile{filename}.remove();
    } else {
      return QDir{filename}.removeRecursively();
    }
  };

  for (const auto &name : entries) {
    if (name == "." || name == "..") {
      continue;
    }

    QString filename = QStringLiteral("%1/%2").arg(cache_dir_name, name);

    while (true) {
      if (remove_cache_fun(filename)) {
        break;
      }

      const auto ret = QMessageBox::warning(
          this, tr("删除缓存失败"),
          tr("无法删除文件或文件夹\"%1\"。\n点击 Ignore 以跳过，点击 Retry "
             "以重试，点击 Cancel 以取消这次操作")
              .arg(filename),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore,
                                       QMessageBox::StandardButton::Retry,
                                       QMessageBox::StandardButton::Cancel});

      if (ret == QMessageBox::StandardButton::Retry) {
        continue;
      }
      if (ret == QMessageBox::StandardButton::Ignore) {
        break;
      }
      return;  // QMessageBox::StandardButton::Cancel and for closing the
               // dialog
    }
  }
}

void SCWind::connect_slots() noexcept {
  // ac_contact_Github

  connect(this->ui->ac_lang_ZH, &QAction::triggered,
          [this]() { this->set_lang(::SCL_language::Chinese); });
  connect(this->ui->ac_lang_EN, &QAction::triggered,
          [this]() { this->set_lang(::SCL_language::English); });

  connect(this->ui->ac_contact_Github, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("https://github.com/SlopeCraft/SlopeCraft"));
  });
  connect(this->ui->ac_contact_Bilibili, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("https://space.bilibili.com/351429231"));
  });
  connect(this->ui->ac_report_bugs, &QAction::triggered, []() {
    QDesktopServices::openUrl(
        QUrl("https://github.com/SlopeCraft/SlopeCraft/issues/new/choose"));
  });
  // ac_check_updates

  connect(this->ui->ac_tutorial, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("https://slopecraft.readthedocs.io/"));
  });
  connect(this->ui->ac_faq, &QAction::triggered, []() {
    QDesktopServices::openUrl(QUrl("https://slopecraft.readthedocs.io/faq/"));
  });

  connect(this->ui->ac_check_updates, &QAction::triggered, [this]() {
    VersionDialog::start_network_request(this, "SlopeCraft",
                                         QUrl{SCWind::update_url},
                                         SCWind::network_manager(), true);
  });
}

void SCWind::on_ac_about_triggered() noexcept {
  QString info;
  info += QStringLiteral("SlopeCraft %1").arg(SlopeCraft::SCL_getSCLVersion());
  info += "\n\n";
  info +=
      tr("SlopeCraft 是一款由 ToKiNoBug 开发的立体地图画生成器，主要用于"
         "在 Minecraft "
         "中制造可以生存实装的立体地图画（但同样支持传统的平板地图画）。");
  info += "\n";
  info +=
      tr("本软件的开发持续集成与 macOS 软件适配由 iXOR Technology (Cubik65536 "
         "以及贡献者) 提供");
  info += "\n\n";
  info += tr("感谢 AbrasiveBoar902 为本软件的设计和优化贡献的力量");
  info += "\n";
  info += tr("感谢 67au 为本软件的 macOS 与 Linux 适配做出的贡献");
  info += "\n";
  info +=
      tr("SlopeCraft 在开发时使用了 Qt，zlib 和 eigen "
         "等开源库，对上述库的开发者表示感谢。");
  info += "\n\n";
  info += tr("本软件遵循 GPL-3.0 及以后版本 (GPL-3.0 or later) 协议开放源码。");
  info += "\n\n";
  info +=
      tr("Copyright © 2021-2023 SlopeCraft 开发者 (TokiNoBug, AbrasiveBoar, "
         "iXOR Technology, Mifan-T, 以及贡献者). 版权所有");
  QMessageBox::information(this, tr("关于 SlopeCraft"), info);
}

void SCWind::on_ac_get_current_colorlist_triggered() noexcept {
  constexpr int basecolors_per_row = 4;
  constexpr int basecolors_per_col = 16;

  static_assert(basecolors_per_row * basecolors_per_col == 64);

  constexpr int row_pixels = basecolors_per_row * 4;
  constexpr int col_pixels = basecolors_per_col * 1;

  static_assert(row_pixels * col_pixels == 256);
  static QString prev_dir{""};
  const QString dest_file =
      QFileDialog::getSaveFileName(this, tr("保存颜色表"), prev_dir, "*.png");

  if (dest_file.isEmpty()) {
    return;
  }

  prev_dir = QFileInfo{dest_file}.dir().path();

  QImage img(row_pixels, col_pixels, QImage::Format::Format_ARGB32);

  if (img.isNull()) {
    QMessageBox::warning(this, tr("保存颜色表失败"), tr("分配内存失败"));
    return;
  }

  img.fill(0x00FFFFFFU);

  uint32_t *const img_data = reinterpret_cast<uint32_t *>(img.scanLine(0));
  /*
  Eigen::Map<
      Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      map(reinterpret_cast<uint32_t *>(img.scanLine(0)), row_pixels,
          col_pixels);
*/
  //  uint32_t argb_colors[256];
  //  uint8_t map_colors[256];
  const auto color_ptrs = this->current_color_table()->colors();
  const int available_colors = color_ptrs.num_colors;

  for (int cidx = 0; cidx < available_colors; cidx++) {
    /*
    const int basecolor = (map_colors[cidx] / 4);
    const int shade = (map_colors[cidx] % 4);
    const int pixel_row = basecolor / basecolors_per_col;
    const int pixel_col = (basecolor % basecolors_per_col) * 4 + shade;
    */
    const uint32_t argb =
        QColor{static_cast<int>(color_ptrs.r_data[cidx] * 255),
               static_cast<int>(color_ptrs.g_data[cidx] * 255),
               static_cast<int>(color_ptrs.b_data[cidx] * 255)}
            .rgb();

    img_data[color_ptrs.map_data[cidx]] = argb;
    // map(map_colors[cidx]) = argb_colors[cidx];
  }

  if (!img.save(dest_file)) {
    QMessageBox::warning(this, tr("保存颜色表失败"),
                         tr("无法生成文件 %1").arg(dest_file));
  }
}

void SCWind::on_ac_test_blocklist_triggered() noexcept {
  static QString prev_dir;
  QString filename =
      QFileDialog::getSaveFileName(this, tr("保存测试文件"), prev_dir, "*.nbt");
  if (filename.isEmpty()) {
    return;
  }
  prev_dir = QFileInfo{filename}.dir().path();

  std::vector<const SlopeCraft::mc_block_interface *> blks;
  std::vector<uint8_t> basecolors;
  for (uint8_t basecolor = 0; basecolor <= SlopeCraft::SCL_maxBaseColor();
       basecolor++) {
    const auto bcwp = this->ui->blm->basecolorwidget_at(basecolor);
    for (const auto &bwp : bcwp->block_widgets()) {
      blks.emplace_back(bwp->attached_block());
      basecolors.emplace_back(basecolor);
    }
  }

  assert(blks.size() == basecolors.size());

  std::string err;
  err.resize(4096);
  auto sd = SlopeCraft::string_deliver::from_string(err);

  SlopeCraft::test_blocklist_options opt;
  opt.block_count = blks.size();
  opt.block_ptrs = blks.data();
  opt.basecolors = basecolors.data();
  opt.err = &sd;

  if (!this->current_color_table()->generate_test_schematic(
          filename.toLocal8Bit().data(), opt)) {
    QString qerr = QString::fromUtf8(err.data());
    QMessageBox::warning(
        this, tr("输出测试文件失败"),
        tr("保存测试文件 %1 时出现错误。详细信息：\n%2").arg(filename, qerr));
  }
}

void SCWind::on_ac_cache_all_3d_triggered() noexcept {
  this->auto_cache_3D(true);
}

void SCWind::on_ac_memory_policy_triggered() noexcept {
  MemoryPolicyDialog diag{this, this->mem_policy};
  const auto result = diag.exec();

  if (result) {
    this->mem_policy = diag.current_value();
    this->auto_cache_3D();
  }
}

void SCWind::when_data_file_command_changed() noexcept {
  const auto export_tasks = this->selected_export_task_list();
  if (export_tasks.empty()) {
    this->ui->pte_command->clear();
    return;
  }
  if (export_tasks.size() >= 2) {
    this->ui->pte_command->setPlainText(
        tr("同时选中多个图片时，不显示 /give "
           "命令。如果想预览导出的命令，请只选择一个图片。"));
    return;
  }
  const auto task = export_tasks.front();
  if (task == nullptr) {
    this->ui->pte_command->clear();
    return;
  }
  const ptrdiff_t global_index = task - this->tasks.data();
  assert(global_index >= 0 and global_index < this->tasks.size());

  auto it = task->converted_images.find(
      {this->current_color_table(), this->current_convert_option()});
  if (it == task->converted_images.end() or
      it->second.converted_image == nullptr) {
    this->ui->pte_command->clear();
    return;
  }
  const SlopeCraft::converted_image &cvted = *it->second.converted_image;
  const auto range = this->tasks.map_range_of(
      this->ui->sb_file_start_idx->value(), global_index);

  auto command_res = this->get_command(cvted, range.first);
  this->ui->pte_command->setPlainText(command_res ? command_res.value()
                                                  : command_res.error());
}

void SCWind::on_pb_export_data_command_clicked() noexcept {
  static QString prev_dir{""};
  const QString dir =
      QFileDialog::getExistingDirectory(this, tr("设置导出位置"), prev_dir);

  if (dir.isEmpty()) {
    return;
  }
  prev_dir = dir;
  const auto converted_tasks = this->tasks.converted_tasks(
      this->current_color_table(), this->current_convert_option());
  const int begin_idx = this->ui->sb_file_start_idx->value();
  int fail_count = 0;
  QString fail_messages;
  for (const auto [g_idx, task] : converted_tasks) {
    assert(task != nullptr);
    assert(g_idx < this->tasks.size());

    const auto map_range = this->tasks.map_range_of(begin_idx, g_idx);
    const auto command_res = this->get_command(
        *(task->converted_images
              .at({this->current_color_table(), this->current_convert_option()})
              .converted_image),
        map_range.first);
    const QString ofilename =
        QStringLiteral("%1/command_%2txt")
            .arg(dir, QFileInfo{task->filename}.baseName());
    if (not command_res) {
      fail_count++;
      fail_messages.append(
          tr("无法为 %1 生成命令：%2\n").arg(ofilename, command_res.error()));
      continue;
    }
    QFile ofile{ofilename, this};
    if (not ofile.open(QIODevice::OpenModeFlag::Text bitor
                       QIODevice::OpenModeFlag::Truncate bitor
                       QIODevice::OpenModeFlag::WriteOnly)) {
      fail_count++;
      fail_messages.append(
          tr("无法创建/打开文件 %1：%2\n").arg(ofilename, ofile.errorString()));
      continue;
    }
    ofile.write(command_res.value().toLocal8Bit());
    if (ofile.error()) {
      fail_count++;
      fail_messages.append(
          tr("无法写入文件 %1：%2\n").arg(ofilename, ofile.errorString()));
      continue;
    }
    ofile.close();
  }
  if (fail_count > 0) {
    QMessageBox::critical(this, tr("%1 个文件保存失败").arg(fail_count),
                          fail_messages);
  }
}
void SCWind::on_pb_export_data_vanilla_structure_clicked() noexcept {
  static QString prev_dir{""};
  const QString dir =
      QFileDialog::getExistingDirectory(this, tr("设置导出位置"), prev_dir);

  if (dir.isEmpty()) {
    return;
  }
  prev_dir = dir;

  const bool export_as_lite =
      this->ui->cb_export_assembled_format->currentIndex() <= 0;
  const auto converted_tasks = this->tasks.converted_tasks(
      this->current_color_table(), this->current_convert_option());
  const int begin_idx = this->ui->sb_file_start_idx->value();

  int fail_count = 0;
  QString fail_messages;
  auto lite_option = this->current_litematic_option(fail_messages).value();
  auto nbt_option = this->current_nbt_option(fail_messages).value();

  this->ui->pbar_export->setValue(0);
  this->ui->pbar_export->setMinimum(0);
  this->ui->pbar_export->setMaximum(converted_tasks.size());

  for (const auto [g_idx, task] : converted_tasks) {
    assert(task not_eq nullptr);
    assert(g_idx < this->tasks.size());

    const auto map_range = this->tasks.map_range_of(begin_idx, g_idx);
    auto option = this->current_assembled_maps_option();
    option.begin_index = map_range.first;

    const QString ofilename =
        QStringLiteral("%1/maps_%2.%3")
            .arg(dir, QFileInfo{task->filename}.baseName(),
                 export_as_lite ? "litematic" : "nbt");

    const auto &cvted =
        (task->converted_images
             .at({this->current_color_table(), this->current_convert_option()})
             .converted_image);

    QString err_temp = tr("SlopeCraftL 未提供详细报错信息。");
    auto report_err_cb = [&err_temp](SCL_errorFlag e, const char *msg) {
      err_temp =
          tr("错误码：%1，详情：%2").arg(magic_enum::enum_name(e).data(), msg);
    };
    // Set up ui callbacks;
    {
      SlopeCraft::ui_callbacks ui{};
      ui.wind = reinterpret_cast<void *>(&report_err_cb);
      using cb_type = decltype(report_err_cb);
      ui.cb_report_error = [](void *lambda, SCL_errorFlag e, const char *msg) {
        (reinterpret_cast<cb_type *>(lambda))->operator()(e, msg);
      };
      lite_option.ui = ui;
      nbt_option.ui = ui;
    }

    bool ok;
    if (export_as_lite) {
      ok = cvted->export_assembled_maps_litematic(
          ofilename.toLocal8Bit().data(), option, lite_option);
    } else {
      ok = cvted->export_assembled_maps_vanilla_structure(
          ofilename.toLocal8Bit().data(), option, nbt_option);
    }
    if (not ok) {
      fail_count++;
      fail_messages.append(tr("%1 生成失败，%2\n").arg(ofilename, err_temp));
    }

    this->ui->pbar_export->setValue(this->ui->pbar_export->value() + 1);
  }
  if (fail_count > 0) {
    QMessageBox::critical(this, tr("%1 个文件保存失败").arg(fail_count),
                          fail_messages);
  }
  this->ui->pbar_export->setValue(converted_tasks.size());
}

void SCWind::on_ac_blocklist_triggered() noexcept {
  auto dialog = new BlockListDialog{this, this->ui->blm};
  dialog->exec();
}