#include "VCWind.h"
#include "VC_block_class.h"
#include "advanced_qlist_widget_item.h"
#include "ui_VCWind.h"

#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <magic_enum.hpp>
#include <ranges>

VCWind::VCWind(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::VCWind), kernel(VCL_create_kernel()) {
  ui->setupUi(this);

  kernel->set_ui(this, callback_progress_range_set,
                 callback_progress_range_add);

  // this->ui->lv_rp;

  this->append_default_to_rp_or_bsl(this->ui->lw_rp, true);
  this->append_default_to_rp_or_bsl(this->ui->lw_bsl, false);

  this->refresh_gpu_info();
  this->select_default_device();

  // for test
  connect(this->ui->action_test01, &QAction::triggered, this,
          &VCWind::make_block_list_page);

  QAbstractButton *const clear_cache_when_toggle[] = {
      this->ui->rb_algo_RGB,   this->ui->rb_algo_RGB_Better,
      this->ui->rb_algo_HSV,   this->ui->rb_algo_Lab00,

      this->ui->rb_algo_Lab94, this->ui->rb_algo_XYZ,
      this->ui->cb_algo_dither};
  for (size_t i = 0; i < sizeof(clear_cache_when_toggle) / sizeof(void *);
       i++) {
    connect(clear_cache_when_toggle[i], &QAbstractButton::toggled, this,
            &VCWind::when_algo_dither_bottons_toggled);
  }
}

VCWind::~VCWind() { delete this->ui; }

void VCWind::callback_progress_range_set(void *__w, int min, int max,
                                         int val) noexcept {
  VCWind *const w = reinterpret_cast<VCWind *>(__w);
  w->ui->progressBar->setRange(min, max);
  w->ui->progressBar->setValue(val);
}

void VCWind::callback_progress_range_add(void *__w, int delta) noexcept {
  VCWind *const w = reinterpret_cast<VCWind *>(__w);
  const int cur_val = w->ui->progressBar->value();
  w->ui->progressBar->setValue(cur_val + delta);
}

// utilitiy functions
void VCWind::append_default_to_rp_or_bsl(QListWidget *qlw,
                                         bool is_rp) noexcept {

  const QString txt = is_rp ? VCWind::tr("原版资源包") : VCWind::tr("原版json");
  advanced_qlwi *aqlwi = new advanced_qlwi(txt, true);
  qlw->addItem(aqlwi);
}

// utilitiy functions
SCL_gameVersion VCWind::current_selected_version() const noexcept {
  if (ui->rdb_version_12->isChecked()) {
    return SCL_gameVersion::MC12;
  }
  if (ui->rdb_version_13->isChecked()) {
    return SCL_gameVersion::MC13;
  }
  if (ui->rdb_version_14->isChecked()) {
    return SCL_gameVersion::MC14;
  }
  if (ui->rdb_version_15->isChecked()) {
    return SCL_gameVersion::MC15;
  }
  if (ui->rdb_version_16->isChecked()) {
    return SCL_gameVersion::MC16;
  }
  if (ui->rdb_version_17->isChecked()) {
    return SCL_gameVersion::MC17;
  }
  if (ui->rdb_version_18->isChecked()) {
    return SCL_gameVersion::MC18;
  }
  if (ui->rdb_version_19->isChecked()) {
    return SCL_gameVersion::MC19;
  }
  abort();
}

VCL_face_t VCWind::current_selected_face() const noexcept {
  if (this->ui->rdb_direction_up->isChecked()) {
    return VCL_face_t::face_up;
  }
  if (this->ui->rdb_direction_side->isChecked()) {
    return VCL_face_t::face_north;
  }
  if (this->ui->rdb_direction_down->isChecked()) {
    return VCL_face_t::face_down;
  }
  abort();
  return {};
}

// utilitiy functions
void VCWind::create_resource_pack() noexcept {
  VCL_destroy_resource_pack(this->rp);

  std::vector<QByteArray> rp_filenames;
  std::vector<const char *> rpfiles_charp;
  rp_filenames.reserve(this->ui->lw_rp->count());

  for (int i = 0; i < this->ui->lw_rp->count(); i++) {
    const auto qlwi = this->ui->lw_rp->item(i);
    if (qlwi->checkState() != Qt::CheckState::Checked) {
      continue;
    }

    const auto aqlwi = dynamic_cast<const advanced_qlwi *>(qlwi);

    if (aqlwi->is_special()) {
      // ./Blocks/VCL/Vanilla_1_12_2.zip
      // ./Blocks/VCL/Vanilla_1_19_2.zip

      if (this->current_selected_version() == SCL_gameVersion::MC12) {
        rp_filenames.emplace_back("./Blocks_VCL/Vanilla_1_12_2.zip");
      } else {

        rp_filenames.emplace_back("./Blocks_VCL/Vanilla_1_19_2.zip");
      }
      continue;
    }

    const QString &filename = qlwi->text();
    rp_filenames.emplace_back(filename.toLocal8Bit());
  }

  rpfiles_charp.resize(rp_filenames.size());
  for (size_t i = 0; i < rp_filenames.size(); i++) {
    rpfiles_charp[i] = rp_filenames[i].data();
  }

  this->rp =
      VCL_create_resource_pack(rpfiles_charp.size(), rpfiles_charp.data());
}

// utilitiy functions
void VCWind::create_block_state_list() noexcept {
  VCL_destroy_block_state_list(this->bsl);
  std::vector<QByteArray> bsl_filenames;
  std::vector<const char *> jsonfiles_charp;
  bsl_filenames.reserve(this->ui->lw_bsl->count());

  for (int i = 0; i < this->ui->lw_bsl->count(); i++) {
    const auto qlwi = this->ui->lw_bsl->item(i);
    if (qlwi->checkState() != Qt::CheckState::Checked) {
      continue;
    }

    const auto aqlwi = dynamic_cast<const advanced_qlwi *>(qlwi);

    if (aqlwi->is_special()) {
      // ./Blocks/VCL/Vanilla_1_12_2.zip
      // ./Blocks/VCL/Vanilla_1_19_2.zip
      bsl_filenames.emplace_back("./Blocks_VCL/VCL_blocks_fixed.json");

      continue;
    }

    const QString &filename = qlwi->text();
    bsl_filenames.emplace_back(filename.toLocal8Bit());
  }

  jsonfiles_charp.resize(bsl_filenames.size());
  for (size_t i = 0; i < bsl_filenames.size(); i++) {
    jsonfiles_charp[i] = bsl_filenames[i].data();
  }

  this->bsl = VCL_create_block_state_list(jsonfiles_charp.size(),
                                          jsonfiles_charp.data());
}

// slot
void VCWind::on_pb_add_rp_clicked() noexcept {
  static QString prev_dir{""};
  QStringList zips = QFileDialog::getOpenFileNames(
      this, VCWind::tr("选择资源包"), prev_dir, "*.zip");
  if (zips.size() <= 0) {
    return;
  }

  prev_dir = QFileInfo(zips.front()).dir().absolutePath();

  for (QString &qstr : zips) {
    advanced_qlwi *aqlwi = new advanced_qlwi(qstr, false);
    this->ui->lw_rp->insertItem(0, aqlwi);
  }
}

// slot
void VCWind::on_pb_remove_rp_clicked() noexcept {
  auto list_qwli = this->ui->lw_rp->selectedItems();
  for (auto qlwi : list_qwli) {
    if (dynamic_cast<advanced_qlwi *>(qlwi)->is_special()) {
      continue;
    }
    this->ui->lw_rp->removeItemWidget(qlwi);
    delete qlwi;
  }
}

// slot
void VCWind::on_pb_add_bsl_clicked() noexcept {
  static QString prev_dir{""};
  QStringList jsons = QFileDialog::getOpenFileNames(
      this, VCWind::tr("选择方块id json文件"), prev_dir, "*.json");
  if (jsons.size() <= 0) {
    return;
  }

  prev_dir = QFileInfo(jsons.front()).dir().absolutePath();

  for (QString &qstr : jsons) {
    advanced_qlwi *aqlwi = new advanced_qlwi(qstr, false);
    this->ui->lw_bsl->insertItem(0, aqlwi);
  }
}

// slot
void VCWind::on_pb_remove_bsl_clicked() noexcept {
  auto list_qwli = this->ui->lw_bsl->selectedItems();
  for (auto qlwi : list_qwli) {
    if (dynamic_cast<advanced_qlwi *>(qlwi)->is_special()) {
      continue;
    }
    this->ui->lw_bsl->removeItemWidget(qlwi);
    delete qlwi;
  }
}

// slot
void VCWind::make_block_list_page() noexcept {
  this->create_resource_pack();
  this->create_block_state_list();

  const size_t num_blocks = VCL_get_blocks_from_block_state_list_match(
      this->bsl, this->current_selected_version(),
      this->current_selected_face(), nullptr, 0);

  std::vector<VCL_block *> buffer(num_blocks);
  for (auto &ptr_ref : buffer) {
    ptr_ref = nullptr;
  }

  VCL_get_blocks_from_block_state_list_match(
      this->bsl, this->current_selected_version(),
      this->current_selected_face(), buffer.data(), buffer.size());

  std::map<VCL_block_class_t, std::vector<VCL_block *>> blocks;
  auto VCL_block_class_t_values = magic_enum::enum_values<VCL_block_class_t>();

  // fill with empty
  for (auto bcl : VCL_block_class_t_values) {
    std::vector<VCL_block *> vec;
    vec.reserve(buffer.size() / VCL_block_class_t_values.size());
    blocks.emplace(bcl, std::move(vec));
  }

  for (VCL_block *blk : buffer) {
    const VCL_block_class_t bcl = VCL_get_block_class(blk);
    blocks[bcl].emplace_back(blk);
  }

  for (auto bcl : VCL_block_class_t_values) {

    VC_block_class *class_widget = nullptr;
    {
      auto it = this->map_VC_block_class.find(bcl);

      if (it == this->map_VC_block_class.end() || it->second == nullptr) {

        class_widget = new VC_block_class(this);
        this->map_VC_block_class.emplace(bcl, class_widget);
      } else {
        class_widget = it->second;
      }
    }

    const size_t idx = size_t(bcl);

    this->ui->gl_sa_blocks->addWidget(class_widget, idx, 0);
    class_widget->show();
    class_widget->setTitle(QString::number(idx) + " " +
                           QString::fromUtf8(magic_enum::enum_name(bcl)) +
                           " (" + QString::number(blocks[bcl].size()) + ") ");

    class_widget->set_blocks(blocks[bcl].size(), blocks[bcl].data(), 4);

    // set images for radio buttons
    for (const auto &pair : class_widget->blocks_vector()) {
      VCL_model *model = VCL_get_block_model(
          pair.first, this->rp, this->current_selected_face(), nullptr);

      if (model == nullptr) {
        continue;
      }

      int rows = 0, cols = 0;

      VCL_compute_projection_image(model, this->current_selected_face(), &rows,
                                   &cols, nullptr, 0);
      if (rows <= 0 || cols <= 0) {
        VCL_destroy_block_model(model);
        continue;
      }

      QImage img(cols, rows, QImage::Format::Format_ARGB32);

      if (img.isNull()) {
        VCL_destroy_block_model(model);
        continue;
      }
      memset(img.scanLine(0), 0xFF, img.sizeInBytes());

      const bool ok = VCL_compute_projection_image(
          model, this->current_selected_face(), nullptr, nullptr,
          (uint32_t *)img.scanLine(0), img.sizeInBytes());
      if (!ok) {
        VCL_destroy_block_model(model);
        continue;
      }

      pair.second->setIcon(QIcon(QPixmap::fromImage(img)));
      VCL_destroy_block_model(model);
    }
  }
}

void VCWind::setup_basical_colorset() noexcept {
  if (VCL_is_basic_colorset_ok()) {
    return;
  }

  if (this->rp == nullptr) {
    this->create_resource_pack();
  }

  if (this->bsl == nullptr) {
    this->create_block_state_list();
  }

  VCL_set_resource_option option;
  option.exposed_face = this->current_selected_face();
  option.version = this->current_selected_version();
  option.max_block_layers = this->ui->sb_max_layers->value();

  const bool success = VCL_set_resource_copy(this->rp, this->bsl, option);

  if (!success) {
    const auto ret = QMessageBox::critical(
        this, VCWind::tr("资源包/方块状态列表json解析失败"),
        VCWind::tr("部分方块的投影图像计算失败，或者总颜色数量超过上限（65534）"
                   "。尝试移除解析失败的资源包/方块列表，或者减小最大层数。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close});
    if (ret == QMessageBox::StandardButton::Close) {
      exit(3);
      return;
    }
  }
}

void VCWind::setup_allowed_colorset() noexcept {
  if (VCL_is_allowed_colorset_ok()) {
    return;
  }
  this->setup_basical_colorset();
  std::vector<VCL_block *> blocks;
  this->selected_blocks(&blocks);

  const bool success = VCL_set_allowed_blocks(blocks.data(), blocks.size());

  if (!success) {
    const auto ret = QMessageBox::critical(
        this, VCWind::tr("设置可用方块失败"),
        VCWind::tr("可能是总颜色数量超过上限（65536），尝试移除解析失败的资源包"
                   "/方块列表，或者减小最大层数。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close});
    if (ret == QMessageBox::StandardButton::Close) {
      exit(3);
      return;
    }
  }
}

size_t
VCWind::selected_blocks(std::vector<VCL_block *> *blocks_dest) const noexcept {
  size_t counter = 0;
  if (blocks_dest != nullptr)
    blocks_dest->clear();
  for (auto &pair : this->map_VC_block_class) {
    counter += pair.second->selected_blocks(blocks_dest, true);
  }

  return counter;
}

void VCWind::on_tb_add_images_clicked() noexcept {
  static QString prev_dir{""};
  QStringList files =
      QFileDialog::getOpenFileNames(this, VCWind::tr("选择图片（可多选）"),
                                    prev_dir, "*.bmp;*.png;*.jpg;*.jpeg");
  if (files.size() <= 0) {
    return;
  }

  prev_dir = QFileInfo(files.front()).dir().absolutePath();

  for (auto filename : files) {
    if (!this->image_cache.contains(filename)) {
      this->image_cache.emplace(filename, std::pair<QImage, QImage>{});
    }

    QImage img(filename);

    if (img.isNull()) {
      const auto ret = QMessageBox::warning(
          this, VCWind::tr("读取图片失败"),
          VCWind::tr("无法读取图片%"
                     "1。图片可能是不支持的格式，或者已经损坏"
                     "。图像过大也可能导致此错误。")
              .arg(filename),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore,
                                       QMessageBox::StandardButton::Close},
          QMessageBox::StandardButton::Ignore);

      if (ret == QMessageBox::StandardButton::Close) {
        exit(2);
        return;
      }

      this->image_cache.erase(filename);
      continue;
    }

    this->image_cache[filename].first =
        img.convertToFormat(QImage::Format_ARGB32);
    this->image_cache[filename].second = QImage{};

    QListWidgetItem *qlwi = new QListWidgetItem;
    qlwi->setText(filename);

    this->ui->lw_image_files->addItem(qlwi);
  }
}

void VCWind::on_tb_remove_images_clicked() noexcept {
  auto selected_items = this->ui->lw_image_files->selectedItems();

  for (auto item : selected_items) {
    this->ui->lw_image_files->removeItemWidget(item);

    auto it = this->image_cache.find(item->text());

    if (it != this->image_cache.end()) {
      this->image_cache.erase(it);
    }
  }
}

void VCWind::setup_image(const QImage &img) noexcept {
  this->setup_allowed_colorset();
  bool ok = this->kernel->set_image(
      img.height(), img.width(),
      reinterpret_cast<const uint32_t *>(img.scanLine(0)), true);
  if (!ok) {
    const auto ret = QMessageBox::critical(
        this, VCWind::tr("设置图片失败"),
        VCWind::tr("这个错误不应该发生的，可能是你点儿背。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close});

    if (ret == QMessageBox::StandardButton::Close) {
      exit(2);
      return;
    }
  }
}

SCL_convertAlgo VCWind::current_selected_algo() const noexcept {
  if (this->ui->rb_algo_RGB->isChecked()) {
    return SCL_convertAlgo::RGB;
  }
  if (this->ui->rb_algo_RGB_Better->isChecked()) {
    return SCL_convertAlgo::RGB_Better;
  }
  if (this->ui->rb_algo_HSV->isChecked()) {
    return SCL_convertAlgo::HSV;
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
  abort();
  return {};
}

void VCWind::show_image(decltype(image_cache)::iterator it) noexcept {
  this->ui->label_raw_image->setPixmap(QPixmap::fromImage(it->second.first));

  if (!it->second.second.isNull()) {
    this->ui->lable_converted->setPixmap(QPixmap::fromImage(it->second.second));
    return;
  }
  this->setup_allowed_colorset();
  this->setup_image(it->second.first);
  const bool ok = this->kernel->convert(this->current_selected_algo(),
                                        this->ui->cb_algo_dither->isChecked());
  if (!ok) {
    assert(false);
    return;
  }

  int64_t rows, cols;
  this->kernel->converted_image(nullptr, &rows, &cols, true);
  if (rows <= 0 || cols <= 0) {
    assert(false);
    return;
  }

  QImage img(cols, rows, QImage::Format_ARGB32);
  memset(img.scanLine(0), 0xFF, img.sizeInBytes());

  this->kernel->converted_image(reinterpret_cast<uint32_t *>(img.scanLine(0)),
                                nullptr, nullptr, true);

  it->second.second = img;
  this->ui->lable_converted->setPixmap(QPixmap::fromImage(img));
}

void VCWind::on_lw_image_files_itemClicked(QListWidgetItem *item) noexcept {

  auto it = this->image_cache.find(item->text());

  if (it == this->image_cache.end()) {
    return;
  }

  this->show_image(it);
}

void VCWind::clear_convert_cache() noexcept {
  this->ui->lable_converted->setPixmap(QPixmap());
  for (auto &pair : this->image_cache) {
    pair.second.second = QImage();
  }
}

void VCWind::on_cb_show_raw_size_stateChanged(int state) noexcept {
  bool autoscale = (state == 0);

  this->ui->label_raw_image->setScaledContents(autoscale);
  this->ui->lable_converted->setScaledContents(autoscale);
}

void VCWind::on_cb_show_raw_stateChanged(int state) noexcept {
  if (state) {
    this->ui->label_raw_image->show();
  } else {
    this->ui->label_raw_image->hide();
  }
}

void VCWind::on_cb_show_converted_stateChanged(int state) noexcept {
  if (state) {
    this->ui->lable_converted->show();
  } else {
    this->ui->lable_converted->hide();
  }
}

VCWind::convert_option VCWind::current_convert_option() const noexcept {
  return convert_option{this->current_selected_algo(),
                        this->ui->cb_algo_dither->isChecked()};
}

void VCWind::when_algo_dither_bottons_toggled() noexcept {
  static bool first_run{true};
  static convert_option prev;

  const auto current = this->current_convert_option();

  if (first_run || prev != current) {
    if (first_run) {
      first_run = false;
    }

    this->clear_convert_cache();

    prev = current;
  }
}