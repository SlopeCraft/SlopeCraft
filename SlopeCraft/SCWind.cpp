#include "SCWind.h"
#include "ui_SCWind.h"
#include <QFileDialog>
#include <QMessageBox>
#include <ranges>
#include <QApplication>

// #include "PoolWidget.h"
SCWind::SCWind(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::SCWind),
      kernel(SlopeCraft::SCL_createKernel()) {
  this->ui->setupUi(this);
  {
    const auto pid = QApplication::applicationPid();

    const QString sys_cache_dir = QDir::tempPath();
    const QString cache_dir =
        QStringLiteral("%1/SlopeCraft/pid=%2").arg(sys_cache_dir).arg(pid);

    this->kernel->setCacheDir(cache_dir.toLocal8Bit());

    this->kernel->setWindPtr(this);
    this->kernel->setProgressRangeSet(
        [](void *_self, int min, int max, int val) {
          SCWind *const self = reinterpret_cast<SCWind *>(_self);
          QProgressBar *const bar = self->current_bar();
          if (bar == nullptr) return;

          bar->setMinimum(min);
          bar->setMaximum(max);
          bar->setValue(val);
        });
    this->kernel->setProgressAdd([](void *_self, int delta) {
      SCWind *const self = reinterpret_cast<SCWind *>(_self);
      QProgressBar *const bar = self->current_bar();
      if (bar == nullptr) return;

      bar->setValue(bar->value() + delta);
    });

    this->kernel->setKeepAwake([](void *) { QApplication::processEvents(); });
  }
  // initialize cvt pool model
  {
    this->cvt_pool_model = new CvtPoolModel{this, &this->tasks};
    this->ui->lview_pool_cvt->setModel(this->cvt_pool_model);
    this->cvt_pool_model->set_listview(this->ui->lview_pool_cvt);
    connect(this->ui->lview_pool_cvt->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            &SCWind::when_cvt_pool_selectionChanged);

    this->export_pool_model = new PoolModel{this, &this->tasks};
    this->ui->lview_pool_export->setModel(this->export_pool_model);
    this->export_pool_model->set_listview(this->ui->lview_pool_export);

    connect(this, &SCWind::image_changed, this->cvt_pool_model,
            &CvtPoolModel::refresh);
    connect(this, &SCWind::image_changed, this->export_pool_model,
            &PoolModel::refresh);
  }

  // initialize blm
  {
    this->ui->blm->setup_basecolors(this->kernel);
    this->ui->blm->set_version_callback(
        [this]() { return this->selected_version(); });

    this->ui->blm->add_blocklist("./Blocks/FixedBlocks.json",
                                 "./Blocks/FixedBlocks");
    this->ui->blm->add_blocklist("./Blocks/CustomBlocks.json",
                                 "./Blocks/CustomBlocks");

    this->ui->blm->finish_blocklist();

    for (auto btnp : this->version_buttons()) {
      connect(btnp, &QRadioButton::toggled, this,
              &SCWind::when_version_buttons_toggled);
    }

    for (auto btnp : this->type_buttons()) {
      connect(btnp, &QRadioButton::toggled, this,
              &SCWind::when_type_buttons_toggled);
    }

    connect(this->ui->blm, &BlockListManager::changed, this,
            &SCWind::when_blocklist_changed);
  }

  for (QRadioButton *rbp : this->export_type_buttons()) {
    connect(rbp, &QRadioButton::clicked, this,
            &SCWind::when_export_type_toggled);
  }

  for (QRadioButton *rbp : this->preset_buttons_no_custom()) {
    connect(rbp, &QRadioButton::clicked, this, &SCWind::when_preset_clicked);
  }
  {
    for (QRadioButton *rbp : this->algo_buttons()) {
      connect(rbp, &QRadioButton::clicked, this,
              &SCWind::when_algo_btn_clicked);
    }
    connect(this->ui->cb_algo_dither, &QCheckBox::clicked, this,
            &SCWind::when_algo_btn_clicked);
  }

  {
    try {
      this->default_presets[0] =
          load_preset("./Blocks/Presets/vanilla.sc_preset_json");
      this->default_presets[1] =
          load_preset("./Blocks/Presets/cheap.sc_preset_json");
      this->default_presets[2] =
          load_preset("./Blocks/Presets/elegant.sc_preset_json");
      this->default_presets[3] =
          load_preset("./Blocks/Presets/shiny.sc_preset_json");
    } catch (std::exception &e) {
      QMessageBox::critical(this, tr("加载默认预设失败"),
                            tr("一个或多个内置的预设不能被解析。SlopeCraft "
                               "可能已经损坏，请重新安装。\n具体报错信息：\n%1")
                                .arg(e.what()));
      abort();
    }
  }

  this->when_preset_clicked();
}

SCWind::~SCWind() {
  delete this->ui;

  {
    const char *cd = this->kernel->cacheDir();
    if (cd != nullptr) {
      QDir cache_dir{QString::fromLocal8Bit(cd)};
      if (cache_dir.exists()) {
        cache_dir.removeRecursively();
      }
    }
  }

  SlopeCraft::SCL_destroyKernel(this->kernel);
}

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

void SCWind::when_cvt_pool_selectionChanged() noexcept {
  const auto selected_idx = this->selected_cvt_task_idx();

  this->refresh_current_cvt_display(selected_idx);
}

#define SC_SLOPECRAFT_PRIVATEMACRO_VERSION_BUTTON_LIST              \
  {                                                                 \
    this->ui->rb_ver12, this->ui->rb_ver13, this->ui->rb_ver14,     \
        this->ui->rb_ver15, this->ui->rb_ver16, this->ui->rb_ver17, \
        this->ui->rb_ver18, this->ui->rb_ver19, this->ui->rb_ver20  \
  }

std::array<QRadioButton *, 20 - 12 + 1> SCWind::version_buttons() noexcept {
  return SC_SLOPECRAFT_PRIVATEMACRO_VERSION_BUTTON_LIST;
}

std::array<const QRadioButton *, 20 - 12 + 1> SCWind::version_buttons()
    const noexcept {
  return SC_SLOPECRAFT_PRIVATEMACRO_VERSION_BUTTON_LIST;
}

#define SC_SLOPECRAFT_PRIVATEMACRO_TYPE_BUTTON_LIST \
  { this->ui->rb_type_3d, this->ui->rb_type_flat, this->ui->rb_type_fileonly }

std::array<QRadioButton *, 3> SCWind::type_buttons() noexcept {
  return SC_SLOPECRAFT_PRIVATEMACRO_TYPE_BUTTON_LIST;
}

std::array<const QRadioButton *, 3> SCWind::type_buttons() const noexcept {
  return SC_SLOPECRAFT_PRIVATEMACRO_TYPE_BUTTON_LIST;
}

SCL_gameVersion SCWind::selected_version() const noexcept {
  auto btns = this->version_buttons();
  for (size_t idx = 0; idx < btns.size(); idx++) {
    if (btns[idx]->isChecked()) {
      return SCL_gameVersion(idx + 12);
    }
  }

  assert(false);

  return SCL_gameVersion::ANCIENT;
}

SCL_mapTypes SCWind::selected_type() const noexcept {
  if (this->ui->rb_type_3d->isChecked()) {
    return SCL_mapTypes::Slope;
  }

  if (this->ui->rb_type_flat->isChecked()) {
    return SCL_mapTypes::Flat;
  }

  if (this->ui->rb_type_fileonly->isChecked()) {
    return SCL_mapTypes::FileOnly;
  }

  assert(false);
  return {};
}

std::vector<int> SCWind::selected_indices() const noexcept {
  std::vector<int> ret;
  auto sel = this->ui->lview_pool_cvt->selectionModel()->selectedIndexes();
  ret.reserve(sel.size());
  for (auto &midx : sel) {
    ret.emplace_back(midx.row());
  }
  return ret;
}

std::optional<int> SCWind::selected_cvt_task_idx() const noexcept {
  auto sel = this->ui->lview_pool_cvt->selectionModel()->selectedIndexes();
  if (sel.size() <= 0) {
    return std::nullopt;
  }

  return sel.front().row();
}

SCL_convertAlgo SCWind::selected_algo() const noexcept {
  if (this->ui->rb_algo_RGB->isChecked()) {
    return SCL_convertAlgo::RGB;
  }
  if (this->ui->rb_algo_RGB_plus->isChecked()) {
    return SCL_convertAlgo::RGB_Better;
  }
  if (this->ui->rb_algo_Lab94->isChecked()) {
    return SCL_convertAlgo::Lab94;
  }
  if (this->ui->rb_algo_Lab00->isChecked()) {
    return SCL_convertAlgo::Lab00;
  }
  if (this->ui->rb_algo_XYZ->isChecked()) {
    return SCL_convertAlgo::XYZ;
  }
  if (this->ui->rb_algo_GACvter->isChecked()) {
    return SCL_convertAlgo::gaCvter;
  }

  assert(false);
  return {};
}

bool SCWind::is_dither_selected() const noexcept {
  return this->ui->cb_algo_dither->isChecked();
}

void SCWind::when_version_buttons_toggled() noexcept {
  this->ui->blm->when_version_updated();
  this->when_blocklist_changed();
}

void SCWind::when_type_buttons_toggled() noexcept {
  this->when_blocklist_changed();
  this->update_button_states();
}

void SCWind::when_blocklist_changed() noexcept {
  this->kernel_set_type();
  this->ui->rb_preset_custom->setChecked(true);
  // this->ui->rb_preset_
}

void SCWind::kernel_set_type() noexcept {
  std::vector<uint8_t> a;
  std::vector<const SlopeCraft::AbstractBlock *> b;

  this->ui->blm->get_blocklist(a, b);

  assert(a.size() == b.size());
  if (!this->kernel->setType(this->selected_type(), this->selected_version(),
                             reinterpret_cast<const bool *>(a.data()),
                             b.data())) {
    QMessageBox::warning(this, tr("设置方块列表失败"),
                         tr("您设置的方块列表可能存在错误"));
    return;
  }
  int num_colors{0};
  this->kernel->getAvailableColors(nullptr, nullptr, &num_colors);

  this->ui->lb_avaliable_colors->setText(
      tr("可用颜色数量：%1").arg(num_colors));
}

#define SC_SLOPECRAFT_PREIVATEMACRO_EXPORT_TYPE_BUTTONS                        \
  {                                                                            \
    this->ui->rb_export_lite, this->ui->rb_export_nbt, this->ui->rb_export_WE, \
        this->ui->rb_export_flat_diagram, this->ui->rb_export_fileonly         \
  }

std::array<QRadioButton *, 5> SCWind::export_type_buttons() noexcept {
  return SC_SLOPECRAFT_PREIVATEMACRO_EXPORT_TYPE_BUTTONS;
}
std::array<const QRadioButton *, 5> SCWind::export_type_buttons()
    const noexcept {
  return SC_SLOPECRAFT_PREIVATEMACRO_EXPORT_TYPE_BUTTONS;
}

std::array<QRadioButton *, 4> SCWind::preset_buttons_no_custom() noexcept {
  return {this->ui->rb_preset_vanilla, this->ui->rb_preset_cheap,
          this->ui->rb_preset_elegant, this->ui->rb_preset_shiny};
}

#define SC_SLOPECRAFT_PRIVATEMARCO_ALGO_BUTTONS           \
  {                                                       \
    this->ui->rb_algo_RGB, this->ui->rb_algo_RGB_plus,    \
        this->ui->rb_algo_Lab94, this->ui->rb_algo_Lab00, \
        this->ui->rb_algo_XYZ, this->ui->rb_algo_GACvter  \
  }

std::array<const QRadioButton *, 6> SCWind::algo_buttons() const noexcept {
  return SC_SLOPECRAFT_PRIVATEMARCO_ALGO_BUTTONS;
}

std::array<QRadioButton *, 6> SCWind::algo_buttons() noexcept {
  return SC_SLOPECRAFT_PRIVATEMARCO_ALGO_BUTTONS;
}

QProgressBar *SCWind::current_bar() noexcept {
  const int cid = this->ui->tw_main->currentIndex();
  switch (cid) {
    case 1:
      return this->ui->pbar_cvt;
    case 2:
      return this->ui->pbar_export;
      break;
    default:
      return nullptr;
  }
}

void SCWind::update_button_states() noexcept {
  {
    const bool disable_3d = (this->selected_type() == SCL_mapTypes::FileOnly);
    std::array<QRadioButton *, 3> rb_export_3d_types{this->ui->rb_export_lite,
                                                     this->ui->rb_export_nbt,
                                                     this->ui->rb_export_WE};
    for (auto rbp : rb_export_3d_types) {
      rbp->setDisabled(disable_3d);
    }

    std::array<QPushButton *, 4> pb_export{
        this->ui->pb_export_all, this->ui->pb_build3d,
        this->ui->pb_preview_compress_effect, this->ui->pb_preview_materials};
    for (QPushButton *pbp : pb_export) {
      pbp->setDisabled(disable_3d);
    }

    if (disable_3d) {
      this->ui->rb_export_fileonly->setChecked(true);
    }
  }
  {
    const bool enable_flatdiagram =
        (this->selected_type() == SCL_mapTypes::Flat);

    this->ui->rb_export_flat_diagram->setEnabled(enable_flatdiagram);
  }
}

void SCWind::when_preset_clicked() noexcept {
  int final_idx = -1;

  for (int idx = 0; idx < (int)this->preset_buttons_no_custom().size(); idx++) {
    if (this->preset_buttons_no_custom()[idx]->isChecked()) {
      final_idx = idx;
      break;
    }
  }

  assert(final_idx >= 0);

  if (!this->ui->blm->loadPreset(this->default_presets[final_idx])) {
    QMessageBox::warning(this, tr("应用预设失败"), "");
    return;
  }

  this->preset_buttons_no_custom()[final_idx]->setChecked(true);
}

void SCWind::when_export_type_toggled() noexcept {
  const bool page_3d = !this->ui->rb_export_fileonly->isChecked();
  if (page_3d) {
    this->ui->sw_export->setCurrentIndex(0);
  } else {
    this->ui->sw_export->setCurrentIndex(1);
  }

  const auto btns = this->export_type_buttons();

  for (int idx = 0; idx < 4; idx++) {
    if (btns[idx]->isChecked()) {
      this->ui->tw_export_options->setCurrentIndex(idx);
    }
  }

  this->update_button_states();
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

void SCWind::kernel_set_image(int idx) noexcept {
  assert(idx >= 0);
  assert(idx < (int)this->tasks.size());

  if (this->kernel->queryStep() < SCL_step::wait4Image) {
    this->kernel_set_type();
  }

  const QImage &raw = this->tasks[idx].original_image;
  this->kernel->setRawImage((const uint32_t *)raw.scanLine(0), raw.height(),
                            raw.width(), false);
}

void SCWind::kernel_convert_image() noexcept {
  assert(this->kernel->queryStep() >= SCL_step::convertionReady);

  if (!this->kernel->convert(this->selected_algo(),
                             this->is_dither_selected())) {
    QMessageBox::warning(this, tr("转化图像失败"), tr(""));

    return;
  }
}

void SCWind::kernel_make_cache() noexcept {
  std::string err;
  err.resize(4096);
  SlopeCraft::StringDeliver sd{err.data(), err.size()};

  if (!this->kernel->saveCache(sd)) {
    QString qerr = QString::fromUtf8(sd.data);
    QMessageBox::warning(this, tr("缓存失败"),
                         tr("未能创建缓存文件，错误信息：\n%1").arg(qerr));
  }
}

bool SCWind::kernel_check_colorset_hash() noexcept {
  return this->kernel->check_colorset_hash();
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

  this->kernel_make_cache();
  this->refresh_current_cvt_display(sel.value(), true);
  this->ui->tw_cvt_image->setCurrentIndex(1);
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
    this->kernel_make_cache();
    task.set_converted();
  }
}

QImage SCWind::get_converted_image_from_kernel() const noexcept {
  assert(this->kernel->queryStep() >= SCL_step::converted);

  const int rows = this->kernel->getImageRows();
  const int cols = this->kernel->getImageCols();

  QImage img{cols, rows, QImage::Format_ARGB32};

  this->kernel->getConvertedImage(nullptr, nullptr, (uint32_t *)img.scanLine(0),
                                  false);

  return img;
}

void SCWind::refresh_current_cvt_display(
    std::optional<int> selected_idx,
    bool is_image_coneverted_in_kernel) noexcept {
  if (!selected_idx.has_value()) {
    this->ui->lb_raw_image->setPixmap({});
    this->ui->lb_cvted_image->setPixmap({});
    return;
  }

  const int idx = selected_idx.value();

  this->ui->lb_raw_image->setPixmap(
      QPixmap::fromImage(this->tasks[idx].original_image));

  if (is_image_coneverted_in_kernel) {
    assert(this->kernel->queryStep() >= SCL_step::converted);

    this->ui->lb_cvted_image->setPixmap(
        QPixmap::fromImage(this->get_converted_image_from_kernel()));
    return;
  }

  this->ui->lb_cvted_image->setPixmap({});
  if (!this->tasks[idx].is_converted) {
    return;
  }

  if (!kernel_check_colorset_hash()) {
    this->mark_all_task_unconverted();
    this->image_changed();
    return;
  }

  this->kernel_set_image(idx);

  if (!this->kernel->load_convert_cache(this->selected_algo(),
                                        this->is_dither_selected())) {
    return;
  }

  this->ui->lb_cvted_image->setPixmap(
      QPixmap::fromImage(this->get_converted_image_from_kernel()));
}

void SCWind::mark_all_task_unconverted() noexcept {
  for (auto &task : this->tasks) {
    task.set_unconverted();
  }
}

void SCWind::when_algo_btn_clicked() noexcept {
  this->refresh_current_cvt_display(this->selected_cvt_task_idx());
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

void SCWind::export_current_cvted_image(int idx, QString filename) noexcept {
  assert(idx >= 0);
  assert(idx < (int)this->tasks.size());

  this->kernel_set_image(idx);
  if (!this->kernel->load_convert_cache(this->selected_algo(),
                                        this->is_dither_selected())) {
    const auto ret = QMessageBox::warning(
        this, tr("无法保存第%1个转化后图像").arg(idx + 1),
        tr("该图像未被转化，或者转化之后修改了颜色表/转化算法。请重新转化它。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok,
                                     QMessageBox::StandardButton::Ignore});
    if (ret == QMessageBox::StandardButton::Ok) {
      this->kernel_convert_image();
      this->kernel_make_cache();
      this->tasks[idx].set_converted();
    } else {
      return;
    }
  }

  bool ok = this->get_converted_image_from_kernel().save(filename);
  if (!ok) {
    QMessageBox::warning(
        this, tr("保存图像失败"),
        tr("保存%1时失败。可能是因为文件路径错误，或者图片格式不支持。")
            .arg(filename));
    return;
  }
}