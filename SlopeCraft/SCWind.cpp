#include "SCWind.h"
#include "ui_SCWind.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QTableWidget>
#include <magic_enum.hpp>
#include <QDesktopServices>

const QString SCWind::update_url{
    "https://api.github.com/repos/SlopeCraft/SlopeCraft/releases"};

// #include "PoolWidget.h"
SCWind::SCWind(QWidget *parent) : QMainWindow(parent), ui(new Ui::SCWind) {
  this->ui->setupUi(this);

  this->connect_slots();
  {
    // create translators
    const char *const translator_filenames[] = {
        ":/i18n/SlopeCraft_en_US.qm", ":/i18n/BlockListManager_en_US.qm",
        ":/i18n/VersionDialog_en_US.qm", ":/i18n/MemoryPolicyDialog_en_US.qm"};
    /*this->translators.reserve(sizeof(translator_filenames) /
                              sizeof(const char *));
                              */
    for (const char *tf : translator_filenames) {
      QTranslator *t = new QTranslator{this};
      QString filename = QString::fromUtf8(tf);
      const bool ok = t->load(filename);
      if (!ok) {
        QMessageBox::warning(this, "Failed to load translate file",
                             QStringLiteral("Failed to load %1").arg(filename));
      }

      this->translators.emplace_back(t);
    }
  }

  // initialize blm
  {
    this->ui->blm->setup_basecolors();
    this->ui->blm->set_version_callback(
        [this]() { return this->selected_version(); });

    QDir::setCurrent(QCoreApplication::applicationDirPath());
    const QString blocks_dir_path = QStringLiteral("./Blocks");
    const QDir blocks_dir{blocks_dir_path};
    if (not blocks_dir.exists()) {
      QMessageBox::critical(
          this, tr("无法加载方块列表"),
          tr("存储方块列表的文件夹 \"%1\" 不存在，或不是文件夹。")
                  .arg(blocks_dir_path) +
              tr("SlopeCraft 必须退出。"));
      exit(1);
    }
    if (not this->ui->blm->add_blocklist(
            QStringLiteral("%1/%2").arg(blocks_dir_path, "FixedBlocks.zip"))) {
      QMessageBox::critical(
          this, tr("无法加载方块列表"),
          tr("无法加载 FixedBlocks.zip ，SlopeCraft 缺乏最基础的方块列表。") +
              tr("SlopeCraft 必须退出。"));
      exit(1);
    }
    QString default_loaded[] = {"CustomBlocks.zip"};
    QString fail_list;
    int fail_counter = 0;
    for (auto file : default_loaded) {
      assert(file not_eq "FixedBlocks.zip");
      const QString abs_name =
          QStringLiteral("%1/%2").arg(blocks_dir_path, file);
      if (not this->ui->blm->add_blocklist(abs_name)) {
        fail_counter++;
        fail_list.append(abs_name);
        fail_list.append('\n');
      }
    }
    this->ui->blm->finish_blocklist();
    if (fail_counter > 0) {
      QMessageBox::warning(
          this, tr("部分方块列表加载失败"),
          tr("以下 %1 "
             "个方块列表文件无法被加载：\n%"
             "2\n由于它们不是必需，你可以忽略此错误并继续使用。")
              .arg(fail_counter)
              .arg(fail_list));
    }

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

  // initialize cvt pool model
  {
    this->cvt_pool_model = new CvtPoolModel{this};
    this->ui->lview_pool_cvt->setModel(this->cvt_pool_model);
    this->cvt_pool_model->set_listview(this->ui->lview_pool_cvt);
    connect(this->ui->lview_pool_cvt->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            &SCWind::when_cvt_pool_selectionChanged);

    this->export_pool_model = new ExportPoolModel{this};
    this->ui->lview_pool_export->setModel(this->export_pool_model);
    this->export_pool_model->set_listview(this->ui->lview_pool_export);
    connect(this->ui->lview_pool_export->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            &SCWind::when_export_pool_selectionChanged);

    connect(this, &SCWind::image_changed, this->cvt_pool_model,
            &CvtPoolModel::refresh);
    connect(this, &SCWind::image_changed, this->export_pool_model,
            &ExportPoolModel::refresh);
    connect(this, &SCWind::image_changed, [this]() {
      this->ui->lview_pool_cvt->doItemsLayout();
      this->ui->lview_pool_export->doItemsLayout();
    });

    connect(this->ui->tw_cvt_image, &QTabWidget::currentChanged, this,
            &SCWind::when_cvt_pool_selectionChanged);
  }
  {
    this->export_table_model = new ExportTableModel{this};
    this->ui->tview_export_fileonly->setModel(this->export_table_model);

    connect(this, &SCWind::image_changed, this->export_table_model,
            &ExportTableModel::refresh);
    connect(this->ui->sb_file_start_idx, &QSpinBox::valueChanged,
            this->export_table_model, &ExportTableModel::refresh);
    connect(this, &SCWind::image_changed, this->export_table_model,
            &ExportTableModel::refresh);

    connect(this->ui->lview_pool_export->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            &SCWind::when_data_file_command_changed);
    connect(this->ui->sb_file_start_idx, &QSpinBox::valueChanged, this,
            &SCWind::when_data_file_command_changed);
    connect(this->ui->cb_mc_version_geq_1_20_5, &QCheckBox::clicked, this,
            &SCWind::when_data_file_command_changed);
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
  // setup presets
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
  // initialize combobox for map facing
  {
    const std::array<QString, 3> key1{tr("墙面"), tr("顶面"), tr("底面")};
    const std::array<QString, 4> key2{tr("北"), tr("南"), tr("东"), tr("西")};
    for (auto facing : magic_enum::enum_values<SCL_map_facing>()) {
      const int idx = static_cast<int>(facing);
      const QString text = tr("%1，向%2").arg(key1[idx / 4], key2[idx % 4]);
      this->ui->cb_map_direction->addItem(text, QVariant::fromValue(facing));
    }
    this->ui->cb_map_direction->setCurrentIndex(0);
  }

  this->when_preset_clicked();

  connect(this->ui->pb_manage_block_list, &QPushButton::clicked, this,
          &SCWind::on_ac_blocklist_triggered);
}

SCWind::~SCWind() {
  delete this->ui;
  {
    QDir cache_dir{this->cache_root_dir()};
    if (cache_dir.exists()) {
      cache_dir.removeRecursively();
    }
  }
}

QString SCWind::cache_root_dir() const noexcept {
  const auto pid = QApplication::applicationPid();

  const QString sys_cache_dir = QDir::tempPath();
  const QString cache_dir =
      QStringLiteral("%1/SlopeCraft/pid=%2").arg(sys_cache_dir).arg(pid);
  return cache_dir;
}

SlopeCraft::color_table *SCWind::current_color_table() noexcept {
  auto settings = colortable_settings{this->ui->blm->current_selection(),
                                      this->selected_type()};
  {
    auto find = this->color_tables.find(settings);
    if (find not_eq this->color_tables.end()) {
      return find->second.get();
    }
  }
  {
    std::vector<uint8_t> a;
    std::vector<const SlopeCraft::mc_block_interface *> b;

    this->ui->blm->get_blocklist(a, b);
    SlopeCraft::color_table_create_info ci;
    ci.map_type = this->selected_type();
    ci.mc_version = this->selected_version();
    for (size_t i = 0; i < 64; i++) {
      ci.blocks[i] = b[i];
      ci.basecolor_allow_LUT[i] = a[i];
    }
    std::unique_ptr<SlopeCraft::color_table, SlopeCraft::deleter> ptr{
        SlopeCraft::SCL_create_color_table(ci)};
    if (ptr == nullptr) {
      //      QMessageBox::warning(this, tr("设置方块列表失败"),
      //                           tr("您设置的方块列表可能存在错误"));
      return nullptr;
    }

    auto it = this->color_tables.emplace(settings, std::move(ptr));
    return it.first->second.get();
  }
}

SlopeCraft::ui_callbacks SCWind::ui_callbacks() const noexcept {
  return SlopeCraft::ui_callbacks{
      .wind = const_cast<SCWind *>(this),
      .cb_keep_awake = [](void *) { QApplication::processEvents(); },
      .cb_report_error =
          [](void *wind, SCL_errorFlag ef, const char *msg) {
            reinterpret_cast<SCWind *>(wind)->report_error(ef, msg);
          },
      .cb_report_working_status =
          [](void *wind, SCL_workStatus ws) {
            SCWind *self = reinterpret_cast<SCWind *>(wind);
            const QString status_str = SCWind::workStatus_to_string(ws);
            QString wind_title;
            if (status_str.isEmpty()) {
              wind_title = SCWind::default_wind_title();
            } else {
              wind_title = QStringLiteral("%1  |  %2")
                               .arg(SCWind::default_wind_title(), status_str);
            }
            self->setWindowTitle(wind_title);
          },
  };
}

SlopeCraft::progress_callbacks progress_callback(QProgressBar *bar) noexcept {
  return SlopeCraft::progress_callbacks{
      .widget = bar,
      .cb_set_range =
          [](void *widget, int min, int max, int val) {
            if (widget == nullptr) {
              return;
            }
            QProgressBar *bar = reinterpret_cast<QProgressBar *>(widget);
            bar->setMinimum(min);
            bar->setMaximum(max);
            bar->setValue(val);
          },
      .cb_add =
          [](void *widget, int delta) {
            if (widget == nullptr) {
              return;
            }
            QProgressBar *bar = reinterpret_cast<QProgressBar *>(widget);
            bar->setValue(bar->value() + delta);
          }};
}

void SCWind::when_cvt_pool_selectionChanged() noexcept {
  const auto selected_idx = this->selected_cvt_task_idx();

  this->refresh_current_cvt_display(selected_idx);
}

#define SC_SLOPECRAFT_PRIVATEMACRO_VERSION_BUTTON_LIST              \
  {                                                                 \
    this->ui->rb_ver12, this->ui->rb_ver13, this->ui->rb_ver14,     \
        this->ui->rb_ver15, this->ui->rb_ver16, this->ui->rb_ver17, \
        this->ui->rb_ver18, this->ui->rb_ver19, this->ui->rb_ver20, \
        this->ui->rb_ver21                                          \
  }

std::array<QRadioButton *, 21 - 12 + 1> SCWind::version_buttons() noexcept {
  return SC_SLOPECRAFT_PRIVATEMACRO_VERSION_BUTTON_LIST;
}

std::array<const QRadioButton *, 21 - 12 + 1> SCWind::version_buttons()
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
  return SCL_mapTypes::Slope;
  // return {};
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
  auto front = sel.front();
  if (not front.isValid()) {
    return std::nullopt;
  }

  const int idx = front.row();
  if (idx < 0 or idx >= this->tasks.size()) {
    // In some corner cases this is true
    // Return nullopt so that we don't return a invalid index
    return std::nullopt;
  }
  return idx;
}

std::vector<cvt_task *> SCWind::selected_export_task_list() const noexcept {
  auto selected_eidx =
      this->ui->lview_pool_export->selectionModel()->selectedIndexes();
  std::vector<cvt_task *> ret;
  ret.reserve(selected_eidx.size());
  for (auto &midx : selected_eidx) {
    ret.emplace_back(
        this->export_pool_model->export_idx_to_task_ptr(midx.row()));
  }
  return ret;
}
cvt_task *SCWind::selected_export_task() const noexcept {
  auto selected = this->selected_export_task_list();
  if (selected.empty()) {
    return nullptr;
  }

  return selected.front();
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
  return SCL_convertAlgo::RGB_Better;
  // return {};
}

bool SCWind::is_dither_selected() const noexcept {
  return this->ui->cb_algo_dither->isChecked();
}

bool SCWind::is_lossless_compression_selected() const noexcept {
  return this->ui->cb_compress_lossless->isChecked();
}
bool SCWind::is_lossy_compression_selected() const noexcept {
  return this->ui->cb_compress_lossy->isChecked();
}
int SCWind::current_max_height() const noexcept {
  return this->ui->sb_max_height->value();
}

SCL_compressSettings SCWind::current_compress_method() const noexcept {
  auto result = static_cast<int>(SCL_compressSettings::noCompress);
  if (this->is_lossless_compression_selected()) {
    result = result bitor int(SCL_compressSettings::NaturalOnly);
  }
  if (this->is_lossy_compression_selected()) {
    result = result bitor int(SCL_compressSettings::ForcedOnly);
  }
  return static_cast<SCL_compressSettings>(result);
}

bool SCWind::is_glass_bridge_selected() const noexcept {
  return this->ui->cb_glass_bridge->isChecked();
}
int SCWind::current_glass_brigde_interval() const noexcept {
  return this->ui->sb_glass_bridge_interval->value();
}
SCL_glassBridgeSettings SCWind::current_glass_method() const noexcept {
  if (this->is_glass_bridge_selected()) {
    return SCL_glassBridgeSettings::withBridge;
  }
  return SCL_glassBridgeSettings::noBridge;
}

bool SCWind::is_fire_proof_selected() const noexcept {
  return this->ui->cb_fireproof->isChecked();
}
bool SCWind::is_enderman_proof_selected() const noexcept {
  return this->ui->cb_enderproof->isChecked();
}
bool SCWind::is_connect_mushroom_selected() const noexcept {
  return this->ui->cb_connect_mushroom->isChecked();
}

SlopeCraft::build_options SCWind::current_build_option() const noexcept {
  return SlopeCraft::build_options{
      .max_allowed_height = (uint16_t)this->current_max_height(),
      .bridge_interval = (uint16_t)this->current_glass_brigde_interval(),
      .compress_method = this->current_compress_method(),
      .glass_method = this->current_glass_method(),
      .fire_proof = this->is_fire_proof_selected(),
      .enderman_proof = this->is_enderman_proof_selected(),
      .connect_mushrooms = this->is_connect_mushroom_selected(),
      .ui = this->ui_callbacks(),
      .main_progressbar = progress_callback(this->ui->pbar_export),
      .sub_progressbar = {}};
}

SCWind::export_type SCWind::selected_export_type() const noexcept {
  auto btns = this->export_type_buttons();
  static_assert(btns.size() == 5);

  constexpr std::array<export_type, 5> export_type_list{
      export_type::litematica, export_type::vanilla_structure,
      export_type::WE_schem,   export_type::flat_diagram,
      export_type::data_file,
  };
  for (int i = 0; i < 5; i++) {
    if (btns[i]->isChecked()) {
      return export_type_list[i];
    }
  }

  return SCWind::export_type::litematica;
  // return {};
}

void SCWind::when_version_buttons_toggled(bool checked) noexcept {
  if (not checked) {
    return;
  }
  this->ui->blm->when_version_updated();
  this->when_blocklist_changed();

  // When mc version is not 1.20, it must be greater or less than 1.20.5, this
  // checkbox is useless and can be fixed
  const bool fix_geq_btn =
      (this->selected_version() not_eq SCL_gameVersion::MC20);
  if (this->selected_version() not_eq SCL_gameVersion::MC20) {
    this->ui->cb_mc_version_geq_1_20_5->setChecked(this->selected_version() >
                                                   SCL_gameVersion::MC20);
  }
  this->ui->cb_mc_version_geq_1_20_5->setDisabled(fix_geq_btn);
}

void SCWind::when_type_buttons_toggled(bool checked) noexcept {
  if (not checked) {
    return;
  }
  this->when_blocklist_changed();
  this->update_button_states();
  {
    auto valid_buttons = this->valid_export_type_buttons(this->selected_type());
    for (auto btnp : valid_buttons) {
      if (btnp->isChecked()) {
        return;
      }
    }
    valid_buttons[0]->click();
  }
  // this->when_export_type_toggled();
}

void SCWind::when_blocklist_changed() noexcept {
  this->set_colorset();
  this->ui->rb_preset_custom->setChecked(true);
  // this->ui->rb_preset_
}

void SCWind::set_colorset() noexcept {
  auto color_table = this->current_color_table();
  const int num_colors =
      (color_table not_eq nullptr) ? color_table->colors().num_colors : 0;

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

std::vector<QRadioButton *> SCWind::valid_export_type_buttons(
    SCL_mapTypes type) const noexcept {
  switch (type) {
    case SCL_mapTypes::Slope:
      return {this->ui->rb_export_lite, this->ui->rb_export_nbt,
              this->ui->rb_export_WE, this->ui->rb_export_fileonly};
    case SCL_mapTypes::Flat:
      return SC_SLOPECRAFT_PREIVATEMACRO_EXPORT_TYPE_BUTTONS;
    case SCL_mapTypes::FileOnly:
      return {this->ui->rb_export_fileonly};
  }
  return {};
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

SlopeCraft::convert_option SCWind::current_convert_option() noexcept {
  return SlopeCraft::convert_option{
      .caller_api_version = SC_VERSION_U64,
      .algo = this->selected_algo(),
      .dither = this->is_dither_selected(),
      .ai_cvter_opt = this->GA_option,
      .progress = progress_callback(this->ui->pbar_cvt),
      .ui = this->ui_callbacks(),
  };
}

std::unique_ptr<SlopeCraft::converted_image, SlopeCraft::deleter>
SCWind::convert_image(int idx) noexcept {
  assert(idx >= 0);
  assert(idx < (int)this->tasks.size());
  return this->convert_image(this->tasks[idx]);
}

std::unique_ptr<SlopeCraft::converted_image, SlopeCraft::deleter>
SCWind::convert_image(const cvt_task &task) noexcept {
  auto ctable = this->current_color_table();
  if (ctable == nullptr) {
    QMessageBox::critical(
        this, tr("没有可用颜色"),
        tr("没有勾选任何颜色，无法转化图像。请至少勾选3~16种颜色。"));
    return nullptr;
  }

  const auto num_blocks = ctable->num_blocks();
  if (num_blocks <= 3) {
    const auto reply = QMessageBox::warning(
        this, tr("勾选颜色太少"),
        tr("仅仅勾选了%"
           "1种颜色，颜色过少，转化效率可能非常差。您可以点Yes继续"
           "转化，但非常建议请尽量多勾选一些颜色。")
            .arg(num_blocks),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Yes,
                                     QMessageBox::StandardButton::No},
        QMessageBox::StandardButton::No);
    if (reply not_eq QMessageBox::StandardButton::Yes) {
      return nullptr;
    }
  }

  const QImage &raw = task.original_image;
  {
    SlopeCraft::const_image_reference img{
        .data = (const uint32_t *)raw.scanLine(0),
        .rows = static_cast<size_t>(raw.height()),
        .cols = static_cast<size_t>(raw.width()),
    };
    auto cvted_img = ctable->convert_image(img, this->current_convert_option());

    return std::unique_ptr<SlopeCraft::converted_image, SlopeCraft::deleter>{
        cvted_img};
  }
}

const SlopeCraft::converted_image &SCWind::convert_if_need(
    cvt_task &task) noexcept {
  const auto table = this->current_color_table();
  const auto opt = this->current_convert_option();
  if (not task.is_converted_with(table, opt)) {
    auto cvted = this->convert_image(task);
    assert(cvted);
    task.set_converted(table, opt, std::move(cvted));
  }

  auto &cvted = task.converted_images[{table, opt}].converted_image;
  assert(cvted != nullptr);
  return *cvted;
}

std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter> SCWind::build_3D(
    const SlopeCraft::converted_image &cvted) noexcept {
  auto ctable = this->current_color_table();
  const auto opt = this->current_build_option();
  auto str = ctable->build(cvted, opt);
  return std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter>{str};
}

std::tuple<const SlopeCraft::converted_image &,
           const SlopeCraft::structure_3D &>
SCWind::convert_and_build_if_need(cvt_task &task) noexcept {
  const auto table = this->current_color_table();
  const auto &cvted = this->convert_if_need(task);

  assert(task.is_converted_with(table, this->current_convert_option()));
  auto &cvt_result =
      task.converted_images.at({table, this->current_convert_option()});
  const auto build_opt = this->current_build_option();

  if (auto str_3D = cvt_result.load_build_cache(*table, build_opt,
                                                this->cache_root_dir())) {
    // The 3D structure is built, it exists in memory or can be loaded
    return {cvted, *str_3D};
  }
  // Build 3D structure now
  auto s = this->build_3D(cvted);
  auto ptr = s.get();
  assert(ptr != nullptr);
  cvt_result.set_built(build_opt, std::move(s));
  return {cvted, *ptr};
}

// void SCWind::kernel_make_cvt_cache() noexcept {
//   std::string err;
//   err.resize(4096);
//   SlopeCraft::string_deliver sd{err.data(), err.size()};
//
//   if (!this->kernel->saveConvertCache(sd)) {
//     QString qerr = QString::fromUtf8(sd.data);
//     QMessageBox::warning(this, tr("缓存失败"),
//                          tr("未能创建缓存文件，错误信息：\n%1").arg(qerr));
//   }
// }

// QImage SCWind::get_converted_image_from_kernel() const noexcept {
//   assert(this->kernel->queryStep() >= SCL_step::converted);
//
//   const int rows = this->kernel->getImageRows();
//   const int cols = this->kernel->getImageCols();
//
//   QImage img{cols, rows, QImage::Format_ARGB32};
//
//   this->kernel->getConvertedImage(nullptr, nullptr, (uint32_t
//   *)img.scanLine(0),
//                                   false);
//
//   return img;
// }

QImage get_converted_image(const SlopeCraft::converted_image &cvted) noexcept {
  QImage img{
      QSize{static_cast<int>(cvted.cols()), static_cast<int>(cvted.rows())},
      QImage::Format::Format_ARGB32};
  cvted.get_converted_image(reinterpret_cast<uint32_t *>(img.scanLine(0)));
  return img;
}

void SCWind::refresh_current_cvt_display(
    std::optional<int> selected_idx) noexcept {
  if (not selected_idx.has_value()) {
    this->ui->lb_raw_image->setPixmap({});
    this->ui->lb_cvted_image->setPixmap({});
    this->ui->lb_map_shape->setText("");
    return;
  }

  const int idx = selected_idx.value();

  this->ui->lb_raw_image->setPixmap(
      QPixmap::fromImage(this->tasks[idx].original_image));
  {
    auto shape = this->tasks[idx].original_image.size();
    const int map_rows = std::ceil(shape.height() / 128.0f);
    const int map_cols = std::ceil(shape.width() / 128.0f);
    this->ui->lb_map_shape->setText(
        tr("%1行，%2列").arg(map_rows).arg(map_cols));
  }

  auto &task = this->tasks[selected_idx.value()];

  auto it = task.converted_images.find(
      {this->current_color_table(), this->current_convert_option()});
  if (it != task.converted_images.end() &&
      it->second.converted_image != nullptr) {
    this->ui->lb_cvted_image->setPixmap(
        QPixmap::fromImage(get_converted_image(*it->second.converted_image)));
    return;
  }

  this->ui->lb_cvted_image->setPixmap({});
}

// void SCWind::mark_all_task_unconverted() noexcept {
//   for (auto &task : this->tasks) {
//     task.converted_img = nullptr;
//     task.structure = nullptr;
//   }
// }

void SCWind::when_algo_btn_clicked() noexcept {
  this->cvt_pool_model->refresh();
  this->refresh_current_cvt_display(this->selected_cvt_task_idx());
}

void SCWind::export_current_cvted_image(int idx, QString filename) noexcept {
  assert(idx >= 0);
  assert(idx < (int)this->tasks.size());

  auto &task = this->tasks[idx];
  auto cvted = task.get_converted_image(this->current_color_table(),
                                        this->current_convert_option());
  if (cvted == nullptr) {
    const auto ret = QMessageBox::warning(
        this, tr("无法保存第%1个转化后图像").arg(idx + 1),
        tr("该图像未被转化，或者转化之后修改了颜色表/转化算法。请重新转化它。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok,
                                     QMessageBox::StandardButton::Ignore});
    if (ret == QMessageBox::StandardButton::Ok) {
      auto cvted_uptr = this->convert_image(task);
      if (cvted_uptr == nullptr) {
        return;
      }
      task.set_converted(this->current_color_table(),
                         this->current_convert_option(), std::move(cvted_uptr));
      cvted = task.get_converted_image(this->current_color_table(),
                                       this->current_convert_option());
      if (cvted == nullptr) {
        return;
      }
    } else {
      return;
    }
  }

  auto img = get_converted_image(*cvted);
  bool ok = img.save(filename);

  if (!ok) {
    QMessageBox::warning(
        this, tr("保存图像失败"),
        tr("保存%1时失败。可能是因为文件路径错误，或者图片格式不支持。")
            .arg(filename));
    return;
  }
}

// void SCWind::kernel_build_3d() noexcept {
//   if (!this->kernel->build(this->current_build_option())) {
//     QMessageBox::warning(this, tr("构建三维结构失败"),
//                          tr("构建三维结构时，出现错误。可能是因为尝试跳步。"));
//     return;
//   }
// }

void SCWind::refresh_current_build_display(cvt_task *taskp) noexcept {
  this->ui->lb_show_3dsize->setText(tr("大小："));
  this->ui->lb_show_block_count->setText(tr("方块数量："));
  if (taskp == nullptr) {
    return;
  }

  auto &task = *taskp;
  const auto cvted_it = task.get_convert_result(this->current_color_table(),
                                                this->current_convert_option());
  if (cvted_it == task.converted_images.end()) {
    return;
  }
  if (auto str_with_info = cvted_it->second.get_build_cache_with_info_noload(
          *this->current_color_table(), this->current_build_option(),
          this->cache_root_dir())) {
    //    const SlopeCraft::structure_3D *str_3D = str_with_info->handle.get();
    const auto x = str_with_info->shape[0], y = str_with_info->shape[1],
               z = str_with_info->shape[2];

    const auto block_count = str_with_info->block_count;
    this->ui->lb_show_3dsize->setText(
        tr("大小： %1 × %2 × %3").arg(x).arg(y).arg(z));
    this->ui->lb_show_block_count->setText(tr("方块数量：%1").arg(block_count));
  }

  //  if (this->selected_export_type() == SCWind::export_type::data_file) {
  //    this->when_data_file_command_changed();
  //  }
}

tl::expected<QString, QString> SCWind::get_command(
    const SlopeCraft::converted_image &cvted, int begin_idx) const noexcept {
  QString command;
  SlopeCraft::ostream_wrapper os{
      .handle = &command,
      .callback_write_data =
          [](const void *data, size_t len, void *handle) {
            QString *buf = reinterpret_cast<QString *>(handle);
            QString temp =
                QString::fromUtf8(reinterpret_cast<const char *>(data),
                                  static_cast<qsizetype>(len));
            buf->append(temp);
          },
  };
  bool after_1_20_5;
  if (this->selected_version() not_eq SCL_gameVersion::MC20) {
    after_1_20_5 = true;
  } else {
    after_1_20_5 = this->ui->cb_mc_version_geq_1_20_5->isChecked();
  }

  SlopeCraft::map_data_file_give_command_options opt{};
  opt.destination = &os;
  opt.begin_index = begin_idx;
  opt.after_1_12 = (this->selected_version() > SCL_gameVersion::MC12);
  opt.after_1_20_5 = after_1_20_5;
  const bool ok = cvted.get_map_command(opt);
  if (!ok) {
    return tl::make_unexpected(tr("生成命令失败：\n%1").arg(command));
  }
  return command;
}

void SCWind::when_export_pool_selectionChanged() noexcept {
  this->refresh_current_build_display(this->selected_export_task());
}

QString extension_of_export_type(SCWind::export_type et) noexcept {
  switch (et) {
    case SCWind::export_type::litematica:
      return "litematic";
    case SCWind::export_type::vanilla_structure:
      return "nbt";
    case SCWind::export_type::WE_schem:
      return "schem";
    case SCWind::export_type::flat_diagram:
      return "flat-diagram.png";
    case SCWind::export_type::data_file:
      return "dat";
  }

  return "Invalid_export_type";
}

std::optional<SlopeCraft::litematic_options> SCWind::current_litematic_option(
    QString &err) const noexcept {
  err.clear();
  static std::string litename;
  static std::string region_name;

  litename = this->ui->le_lite_name->text().toUtf8().data();
  region_name = this->ui->le_lite_region_name->text().toUtf8().data();
  if (litename.empty()) {
    litename = "UnnamedLitematica";
  }
  if (region_name.empty()) {
    region_name = "UnnamedRegion";
  }

  return SlopeCraft::litematic_options{
      .caller_api_version = SC_VERSION_U64,
      .litename_utf8 = litename.data(),
      .region_name_utf8 = region_name.data(),
      .ui = this->ui_callbacks(),
      .progressbar = progress_callback(this->ui->pbar_export),
  };
}

std::optional<SlopeCraft::vanilla_structure_options> SCWind::current_nbt_option(
    QString &err) const noexcept {
  err.clear();

  return SlopeCraft::vanilla_structure_options{
      .is_air_structure_void = this->ui->cb_nbt_air_void->isChecked(),
      .ui{},
      .progressbar{},
  };
}

std::optional<SlopeCraft::WE_schem_options> SCWind::current_schem_option(
    QString &err) const noexcept {
  err.clear();

  SlopeCraft::WE_schem_options ret;

  {
    const std::array<QLineEdit *, 3> le_offset{this->ui->le_WE_offset_X,
                                               this->ui->le_WE_offset_Y,
                                               this->ui->le_WE_offset_Z};

    for (size_t idx = 0; idx < le_offset.size(); idx++) {
      bool ok;

      ret.offset[idx] = le_offset[idx]->text().toInt(&ok);
      if (!ok) {
        err = tr("WE 原理图参数有错：输入给 offset 的值\"%"
                 "1\"不是一个有效的坐标，应当输入一个整数。")
                  .arg(le_offset[idx]->text());
        return std::nullopt;
      }
    }
  }

  {
    const std::array<QLineEdit *, 3> le_weoffset{this->ui->le_WE_weoffset_X,
                                                 this->ui->le_WE_weoffset_Y,
                                                 this->ui->le_WE_weoffset_Z};

    for (size_t idx = 0; idx < le_weoffset.size(); idx++) {
      bool ok;

      ret.we_offset[idx] = le_weoffset[idx]->text().toInt(&ok);
      if (!ok) {
        err = tr("WE 原理图参数有错：输入给 we offset 的值\"%"
                 "1\"不是一个有效的数字，应当输入一个整数。")
                  .arg(le_weoffset[idx]->text());
        return std::nullopt;
      }
    }
  }
  static std::string region_name;
  region_name = this->ui->le_WE_region_name->text().toUtf8().data();

  static std::vector<const char *> mod_charp;
  {
    static std::vector<std::string> mod_names;

    const auto mod_names_q =
        this->ui->le_WE_mods->toPlainText().replace("\r\n", "\n").split('\n');

    mod_names.resize(mod_names_q.size());
    mod_charp.resize(mod_names_q.size());

    for (int idx = 0; idx < mod_names_q.size(); idx++) {
      mod_names[idx] = mod_names_q[idx].toUtf8().data();
      mod_charp[idx] = mod_names[idx].c_str();
    }
  }

  ret.num_required_mods = mod_charp.size();
  ret.required_mods_name_utf8 = mod_charp.data();

  ret.ui = this->ui_callbacks();
  ret.progressbar = progress_callback(this->ui->pbar_export);

  return ret;
}

std::optional<SlopeCraft::flag_diagram_options>
SCWind::current_flatdiagram_option(QString &err) const noexcept {
  err.clear();

  int row_margin = this->ui->sb_flatdiagram_hmargin->value();
  int col_margin = this->ui->sb_flatdiagram_vmargin->value();

  if (row_margin <= 0 || col_margin <= 0) {
    err =
        tr("平面示意图的分割线间距无效：水平间距为 %1，垂直间距为 %2， "
           "但间距必须为正数。");
    return std::nullopt;
  }

  if (!this->ui->cb_flatdiagram_hline->isChecked()) {
    row_margin = -1;
  }

  if (!this->ui->cb_flatdiagram_vline->isChecked()) {
    col_margin = -1;
  }

  return SlopeCraft::flag_diagram_options{
      .caller_api_version = SC_VERSION_U64,
      .split_line_row_margin = row_margin,
      .split_line_col_margin = col_margin,
      .ui = this->ui_callbacks(),
      .progressbar = progress_callback(this->ui->pbar_export),
  };
}

int SCWind::current_map_begin_seq_number() const noexcept {
  return this->ui->sb_file_start_idx->value();
}

void SCWind::report_error(::SCL_errorFlag flag, const char *msg) noexcept {
  if (flag == SCL_errorFlag::NO_ERROR_OCCUR) {
    return;
  }
  using sb = QMessageBox::StandardButton;
  using sbs = QMessageBox::StandardButtons;

  auto flag_name = magic_enum::enum_name(flag);

  const QString errmsg = tr("错误类型：%1，错误码：%2。详细信息：\n%3")
                             .arg(flag_name.data())
                             .arg(int(flag))
                             .arg(msg);

  const auto ret = QMessageBox::critical(
      this, tr("SlopeCraft 出现错误"),
      tr("%1\n\n点击 Ok 以忽略这个错误，点击 Close 将退出 SlopeCraft。")
          .arg(errmsg),
      sbs{sb::Ok, sb::Close});

  if (ret == sb::Close) {
    exit(0);
  }

  return;
}

void SCWind::set_lang(::SCL_language lang) noexcept {
  this->language = lang;
  for (auto trans : this->translators) {
    if (this->language == ::SCL_language::Chinese) {
      QApplication::removeTranslator(trans);
    } else {
      QApplication::installTranslator(trans);
    }
  }
  this->ui->retranslateUi(this);

  this->ui->blm->when_lang_updated(lang);
}

QString impl_default_title() noexcept {
  return QStringLiteral("SlopeCraft %1").arg(SlopeCraft::SCL_getSCLVersion());
}

const QString &SCWind::default_wind_title() noexcept {
  static const QString title = impl_default_title();
  return title;
}

QString SCWind::workStatus_to_string(::SCL_workStatus status) noexcept {
  switch (status) {
    case SlopeCraft::workStatus::none:
      break;
    case SlopeCraft::workStatus::buidingHeighMap:
      return tr("正在构建高度矩阵");
    case SlopeCraft::workStatus::building3D:
      return tr("正在构建三维结构");
    case SlopeCraft::workStatus::collectingColors:
      return tr("正在收集整张图片的颜色");
    case SlopeCraft::workStatus::compressing:
      return tr("正在压缩立体地图画");
    case SlopeCraft::workStatus::constructingBridges:
      return tr("正在为立体地图画搭桥");
    case SlopeCraft::workStatus::converting:
      return tr("正在匹配颜色");
    case SlopeCraft::workStatus::dithering:
      return tr("正在使用抖动仿色");
    case SlopeCraft::workStatus::flippingToWall:
      return tr("正在将平板地图画变为墙面地图画");
    case SlopeCraft::workStatus::writing3D:
      return tr("正在写入三维结构");
    case SlopeCraft::workStatus::writingBlockPalette:
      return tr("正在写入方块列表");
    case SlopeCraft::workStatus::writingMapDataFiles:
      return tr("正在写入地图数据文件");
    case SlopeCraft::workStatus::writingMetaInfo:
      return tr("正在写入基础信息");
  }

  return {};
}

std::tuple<const SlopeCraft::converted_image *,
           const SlopeCraft::structure_3D *>
SCWind::load_selected_3D() noexcept {
  auto taskp = this->selected_export_task();
  if (taskp == nullptr) {
    QMessageBox::warning(this, tr("未选择图像"),
                         tr("请在左侧任务池选择一个图像"));
    return {nullptr, nullptr};
  }
  assert(taskp != nullptr);

  cvt_task &task = *taskp;
  const ptrdiff_t index = &task - this->tasks.data();
  assert(index >= 0 && index < ptrdiff_t(this->tasks.size()));
  if (!task.is_converted_with(this->current_color_table(),
                              this->current_convert_option())) {
    QMessageBox::warning(this, tr("该图像尚未被转化"),
                         tr("必须先转化一个图像，然后再为它构建三维结构"));
    return {nullptr, nullptr};
  }
  QString errtitle;
  QString errmsg;
  // try to load cache
  auto [cvted_img, structure_3D] =
      [this, &task, &errtitle,
       &errmsg]() -> std::pair<const SlopeCraft::converted_image *,
                               const SlopeCraft::structure_3D *> {
    auto it = task.converted_images.find(convert_input{
        this->current_color_table(), this->current_convert_option()});
    if (it == task.converted_images.end()) {
      errtitle = tr("该图像尚未被转化");
      errmsg =
          tr("可能是在转化完成之后又修改了转化算法，因此之前的转化无效。必须重"
             "新转化该图像。");
      return {nullptr, nullptr};
    }
    auto str_3D = it->second.load_build_cache(*this->current_color_table(),
                                              this->current_build_option(),
                                              this->cache_root_dir());

    if (str_3D == nullptr) {
      errtitle = tr("尚未构建三维结构");
      errmsg = tr(
          "在预览材料表之前，必须先构建三维结构。出现这个警告，可能是因为你"
          "在构建三维结构之后，又修改了三维结构的选项，因此之前的结果无效。");
      return {it->second.converted_image.get(), nullptr};
    }
    return {it->second.converted_image.get(), str_3D};
  }();

  if (!errtitle.isEmpty()) {
    QMessageBox::warning(this, errtitle, errmsg);
    return {nullptr, nullptr};
  }

  return {cvted_img, structure_3D};
}

bool SCWind::should_auto_cache(bool suppress_warnings) noexcept {
  bool result = false;
  const auto self_used = get_self_memory_info();
  QString error_template =
      tr("这不是严重的问题，你可以直接忽略这个警告，或者把它反馈给开发者，"
         "不影响正常使用。只是 Slopecraft "
         "可能占用更多的内存。\n详细信息：\n%1");

  if (not self_used) {
    if (not suppress_warnings) {
      QMessageBox::warning(
          this, tr("获取本进程的内存占用失败"),
          error_template.arg(QString::fromLocal8Bit(self_used.error().c_str())),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok});
    }
  } else {
    result = this->mem_policy.should_cache(self_used.value()) or result;
  }

  const auto system_info = get_system_memory_info();
  if (not system_info) {
    if (not suppress_warnings) {
      QMessageBox::warning(
          this, tr("获取操作系统内存占用失败"),
          error_template.arg(QString::fromLocal8Bit(self_used.error().c_str())),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok});
    }
  } else {
    result = this->mem_policy.should_cache(system_info.value()) or result;
  }
  return result;
}

SCWind::auto_cache_report SCWind::auto_cache_3D(
    [[maybe_unused]] bool cache_all) noexcept {
  //  const auto colortable = this->current_color_table();
  //  const auto build_opt = this->current_build_option();
  auto_cache_report report{
      .structures_cached = 0,
      .memory_saved = 0,
  };
  const auto self_mem_before = get_self_memory_info();
  const QString cache_root = this->cache_root_dir();

  auto go_through = [this, cache_root, cache_all]() -> size_t {
    size_t cached = 0;
    for (auto &task : this->tasks) {
      for (auto &[cvt_input, cvted] : task.converted_images) {
        // if we don't need to cache, return.
        if ((not cache_all) and (not this->should_auto_cache(true))) {
          return cached;
        }

        auto report = cvted.cache_all_structures(
            *cvt_input.table, *cvted.converted_image, cache_root);
        cached += report.cache_num;
      }
    }
    return cached;
  };

  report.structures_cached = go_through();
  const auto self_mem_current = get_self_memory_info();
  if (self_mem_before and self_mem_current) {
    report.memory_saved =
        self_mem_before.value().used - self_mem_current.value().used;
  }

  return report;
}

SlopeCraft::assembled_maps_options SCWind::current_assembled_maps_option()
    const noexcept {
  SlopeCraft::assembled_maps_options option;
  option.mc_version = this->selected_version();
  option.frame_variant = this->ui->cb_glowing_item_frame->isChecked()
                             ? SCL_item_frame_variant::glowing
                             : SCL_item_frame_variant::common;
  option.after_1_20_5 = this->ui->cb_mc_version_geq_1_20_5->isChecked();
  option.fixed_frame = this->ui->cb_fixed_frame->isChecked();
  option.invisible_frame = this->ui->cb_invisible_frame->isChecked();
  option.map_facing =
      this->ui->cb_map_direction->currentData().value<SCL_map_facing>();

  return option;
}