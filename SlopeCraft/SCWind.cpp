#include "SCWind.h"
#include "ui_SCWind.h"
#include <QFileDialog>
#include <QMessageBox>
#include <ranges>

// #include "PoolWidget.h"
SCWind::SCWind(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::SCWind),
      kernel(SlopeCraft::SCL_createKernel()) {
  this->ui->setupUi(this);
  {
    this->kernel->setWindPtr(this);
    this->kernel->setAlgoProgressAdd([](void *, int) {});
    this->kernel->setAlgoProgressRangeSet([](void *, int, int, int) {});
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
  {
    for (QRadioButton *rbp : this->export_type_buttons()) {
      connect(rbp, &QRadioButton::clicked, this,
              &SCWind::when_export_type_toggled);
    }
  }
  {
    for (QRadioButton *rbp : this->preset_buttons_no_custom()) {
      connect(rbp, &QRadioButton::clicked, this, &SCWind::when_preset_clicked);
    }
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

  { this->when_preset_clicked(); }
}

SCWind::~SCWind() {
  delete this->ui;
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
  auto sel = this->ui->lview_pool_cvt->selectionModel()->selectedIndexes();

  if (sel.size() <= 0) {
    this->ui->lb_raw_image->setPixmap({});
    this->ui->lb_cvted_image->setPixmap({});
    return;
  }

  const int idx = sel.front().row();

  this->ui->lb_raw_image->setPixmap(
      QPixmap::fromImage(this->tasks[idx].original_image));
#warning load converted image here
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