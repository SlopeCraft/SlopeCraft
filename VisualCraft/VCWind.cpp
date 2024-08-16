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

#include "VCWind.h"

#include <QCryptographicHash>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <iostream>
#include <magic_enum.hpp>
#include <QByteArrayView>
#include "VC_block_class.h"
#include "ui_VCWind.h"

using std::cout, std::endl;

uint8_t is_language_ZH = true;

VCL_config VCWind::config;

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
          &VCWind::setup_block_widgets);

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

  this->setup_ui_select_biome();
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
  const QString txt =
      is_rp ? VCWind::tr("原版资源包") : VCWind::tr("原版 json");
  QListWidgetItem *qlwi = new QListWidgetItem(txt);

  qlwi->setData(Qt::UserRole, true);
  qlwi->setCheckState(Qt::CheckState::Checked);
  qlwi->setIcon(QIcon(QApplication::style()->standardIcon(
      QStyle::StandardPixmap::SP_FileIcon)));
  qlw->addItem(qlwi);
}

void VCWind::setup_ui_select_biome() noexcept {
  this->ui->combobox_select_biome->clear();

  for (const auto &b : magic_enum::enum_values<VCL_biome_t>()) {
    this->ui->combobox_select_biome->addItem(
        QString::fromUtf8(VCL_biome_name(b, ::is_language_ZH)), int(b));
  }
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
  if (ui->rdb_version_20->isChecked()) {
    return SCL_gameVersion::MC20;
  }
  if (ui->rdb_version_21->isChecked()) {
    return SCL_gameVersion::MC21;
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

VCWind::basic_colorset_option VCWind::current_basic_option() const noexcept {
  basic_colorset_option ret;
  for (int i = 0; i < this->ui->lw_rp->count(); i++) {
    const auto qlwi = this->ui->lw_rp->item(i);
    if (qlwi->checkState() != Qt::CheckState::Checked) {
      continue;
    }

    if (qlwi->data(Qt::UserRole).toBool()) {
      ret.zips.emplace_back(
          QString::fromUtf8(
              VCWind::config.default_zips.at(this->current_selected_version()))
              .data());
      continue;
    }

    QString filename = qlwi->text();
    ret.zips.emplace_back(filename);
  }
  for (int i = 0; i < this->ui->lw_bsl->count(); i++) {
    const auto qlwi = this->ui->lw_bsl->item(i);
    if (qlwi->checkState() != Qt::CheckState::Checked) {
      continue;
    }

    if (qlwi->data(Qt::UserRole).toBool()) {
      for (const auto &jfilename : VCWind::config.default_jsons) {
        ret.jsons.emplace_back(QString::fromUtf8(jfilename.data()));
      }

      continue;
    }

    QString filename = qlwi->text();
    ret.jsons.emplace_back(filename);
  }
  ret.face = this->current_selected_face();
  ret.layers = this->ui->sb_max_layers->value();
  ret.version = this->current_selected_version();
  ret.biome =
      (VCL_biome_t)this->ui->combobox_select_biome->currentData().toInt();
  ret.is_leaves_transparent = this->ui->cb_leaves_transparent->isChecked();
  return ret;
}

QByteArray VCWind::checksum_basic_colorset_option(
    const basic_colorset_option &opt) const noexcept {
  QCryptographicHash hash(QCryptographicHash::Algorithm::Sha1);

  for (const auto &zip : opt.zips) {
    hash.addData(zip.toUtf8());
  }

  for (const auto &json : opt.jsons) {
    hash.addData(json.toUtf8());
  }

#if QT_VERSION > 0x060300
#define VC_PMACRO_QBAV_CLASS QByteArrayView
#else
#define VC_PMACRO_QBAV_CLASS QByteArray
#endif

  hash.addData(VC_PMACRO_QBAV_CLASS{(const char *)&opt.face, sizeof(opt.face)});
  hash.addData(
      VC_PMACRO_QBAV_CLASS{(const char *)&opt.version, sizeof(opt.version)});
  hash.addData(
      VC_PMACRO_QBAV_CLASS{(const char *)&opt.layers, sizeof(opt.layers)});
  hash.addData(
      VC_PMACRO_QBAV_CLASS{(const char *)&opt.biome, sizeof(opt.biome)});
  hash.addData(VC_PMACRO_QBAV_CLASS{(const char *)&opt.is_leaves_transparent,
                                    sizeof(opt.is_leaves_transparent)});

  return hash.result();
}

// utilitiy functions
VCL_resource_pack *VCWind::create_resource_pack(
    const basic_colorset_option &opt) noexcept {
  std::vector<QByteArray> rpfiles_qba;
  rpfiles_qba.reserve(opt.zips.size());
  std::vector<const char *> rpfiles_charp;
  rpfiles_charp.reserve(opt.zips.size());

  std::vector<QByteArray> rp_contents;
  std::vector<VCL_read_only_buffer> rp_buffer_references;

  for (auto &qstr : opt.zips) {
    rpfiles_qba.emplace_back(qstr.toUtf8());
    rpfiles_charp.emplace_back(rpfiles_qba.back().data());
    QFile file{qstr};
    if (not file.open(QIODevice::OpenModeFlag::ReadOnly |
                      QIODevice::OpenModeFlag::ExistingOnly)) {
      QMessageBox::critical(nullptr, tr("无法读取文件 %1").arg(qstr),
                            file.errorString());
      return nullptr;
    }
    rp_contents.emplace_back(file.readAll());
    const auto &content = rp_contents.back();
    rp_buffer_references.emplace_back(content.data(), content.size());
    file.close();
  }
  assert(rpfiles_qba.size() == rpfiles_charp.size());
  assert(rpfiles_charp.size() == rp_contents.size());
  assert(rp_contents.size() == rp_buffer_references.size());

  return VCL_create_resource_pack_from_buffers(
      rp_contents.size(), rp_buffer_references.data(), rpfiles_charp.data());
  // return VCL_create_resource_pack(rpfiles_charp.size(),
  // rpfiles_charp.data());
}

// utilitiy functions
VCL_block_state_list *VCWind::create_block_state_list(
    const basic_colorset_option &opt) noexcept {
  std::vector<QByteArray> bsl_filenames;
  std::vector<const char *> jsonfiles_charp;

  for (auto &qstr : opt.jsons) {
    bsl_filenames.emplace_back(qstr.toUtf8());
    jsonfiles_charp.emplace_back(bsl_filenames.back().data());
  }

  return VCL_create_block_state_list(jsonfiles_charp.size(),
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
    QListWidgetItem *qlwi = new QListWidgetItem(qstr);
    qlwi->setData(Qt::UserRole, false);
    qlwi->setCheckState(Qt::CheckState::Checked);
    qlwi->setIcon(QIcon(QApplication::style()->standardIcon(
        QStyle::StandardPixmap::SP_FileIcon)));
    this->ui->lw_rp->insertItem(0, qlwi);
  }
}

// slot
void VCWind::on_pb_remove_rp_clicked() noexcept {
  auto list_qwli = this->ui->lw_rp->selectedItems();
  for (auto qlwi : list_qwli) {
    if (qlwi->data(Qt::UserRole).toBool()) {
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
      this, VCWind::tr("选择方块 id json 文件"), prev_dir, "*.json");
  if (jsons.size() <= 0) {
    return;
  }

  prev_dir = QFileInfo(jsons.front()).dir().absolutePath();

  for (QString &qstr : jsons) {
    QListWidgetItem *qlwi = new QListWidgetItem(qstr);

    qlwi->setData(Qt::UserRole, false);
    qlwi->setCheckState(Qt::CheckState::Checked);
    qlwi->setIcon(QIcon(QApplication::style()->standardIcon(
        QStyle::StandardPixmap::SP_FileIcon)));
    this->ui->lw_bsl->insertItem(0, qlwi);
  }
}

// slot
void VCWind::on_pb_remove_bsl_clicked() noexcept {
  auto list_qwli = this->ui->lw_bsl->selectedItems();
  for (auto qlwi : list_qwli) {
    if (qlwi->data(Qt::UserRole).toBool()) {
      continue;
    }
    this->ui->lw_bsl->removeItemWidget(qlwi);
    delete qlwi;
  }
}

// slot
void VCWind::setup_block_widgets() noexcept {
  // this->setup_basical_colorset();

  for (auto &pair : this->map_VC_block_class) {
    this->ui->gl_sa_blocks->removeWidget(pair.second);
    delete pair.second;
  }

  this->map_VC_block_class.clear();

  const size_t num_blocks = VCL_get_blocks_from_block_state_list_match(
      VCL_get_block_state_list(), this->current_selected_version(),
      this->current_selected_face(), nullptr, 0);

  std::vector<VCL_block *> buffer(num_blocks);
  for (auto &ptr_ref : buffer) {
    ptr_ref = nullptr;
  }
  const size_t block_nums = VCL_get_blocks_from_block_state_list_match(
      VCL_get_block_state_list(), this->current_selected_version(),
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

        connect(class_widget->chbox_enabled(), &QCheckBox::toggled, this,
                &VCWind::when_algo_dither_bottons_toggled);

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

    std::sort(blocks[bcl].begin(), blocks[bcl].end(), VCL_compare_block);

    class_widget->set_blocks(blocks[bcl].size(), blocks[bcl].data(), 4);

    // set images for radio buttons
    for (const auto &pair : class_widget->blocks_vector()) {
      connect(pair.second, &QCheckBox::toggled, this,
              &VCWind::when_algo_dither_bottons_toggled);

      VCL_model *model =
          VCL_get_block_model(pair.first, VCL_get_resource_pack());

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

  this->ui->gb_blocks->setTitle(VCWind::tr("全部方块") +
                                QStringLiteral(" (%1)").arg(block_nums));

  this->ui->ac_browse_block->setEnabled(true);
  this->ui->ac_browse_biome->setEnabled(true);
}

bool VCWind::is_basical_colorset_changed() const noexcept {
  // static QByteArray hash_prev{""};
  auto curr_opt = this->current_basic_option();
  QByteArray curr_hash = this->checksum_basic_colorset_option(curr_opt);
  const bool ret = this->basical_option_hash_prev != curr_hash;

  return ret;
}

void VCWind::update_hash_basic(const basic_colorset_option &opt) noexcept {
  this->basical_option_hash_prev = this->checksum_basic_colorset_option(opt);
}

void VCWind::setup_basical_colorset() noexcept {
  if (VCL_is_basic_colorset_ok() && !this->is_basical_colorset_changed()) {
    return;
  }

  emit signal_basic_colorset_changed();

  VCL_discard_resource();

  auto current_option = this->current_basic_option();

  VCL_resource_pack *rp = VCWind::create_resource_pack(current_option);

  if (rp == nullptr) {
    QMessageBox::critical(
        this, VCWind::tr("资源包解析失败"),
        VCWind::tr("在此窗口之前弹出的错误信息非常重要，请将它汇报给开发者。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close});
    exit(3);
    return;
  }

  VCL_block_state_list *bsl = VCWind::create_block_state_list(current_option);
  if (bsl == nullptr) {
    QMessageBox::critical(
        this, VCWind::tr("方块状态列表 json 解析失败"),
        VCWind::tr("在此窗口之前弹出的错误信息非常重要，请将它汇报给开发者。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close});
    VCL_destroy_resource_pack(rp);
    exit(4);
    return;
  }

  VCL_set_resource_option option;
  option.exposed_face = this->current_selected_face();
  option.version = this->current_selected_version();
  option.max_block_layers = this->ui->sb_max_layers->value();
  option.biome =
      (VCL_biome_t)this->ui->combobox_select_biome->currentData().toInt();
  option.is_render_quality_fast = !this->ui->cb_leaves_transparent->isChecked();

  const bool success = VCL_set_resource_move(&rp, &bsl, option);

  VCL_destroy_block_state_list(bsl);
  VCL_destroy_resource_pack(rp);

  if (!success) {
    const auto ret = QMessageBox::critical(
        this, VCWind::tr("资源包/方块状态列表 json 解析失败"),
        VCWind::tr("部分方块的投影图像计算失败，或者总颜色数量超过上限（65534）"
                   "。尝试移除解析失败的资源包/方块列表，或者减小最大层数。"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close});
    if (ret == QMessageBox::StandardButton::Close) {
      exit(5);
      return;
    }
  }

  this->update_hash_basic(current_option);
  this->setup_block_widgets();
  this->ui->ac_browse_basic_colors->setEnabled(true);
}

QByteArray VCWind::checksum_allowed_colorset_option(
    const allowed_colorset_option &opt) noexcept {
  QCryptographicHash hash(QCryptographicHash::Algorithm::Sha1);

  for (const VCL_block *blk : opt.blocks) {
    const char *const id = VCL_get_block_id(blk);
    hash.addData(QByteArray(id));
  }

  return hash.result();
}

bool VCWind::is_allowed_colorset_changed(
    allowed_colorset_option *opt) const noexcept {
  // static QByteArray prev_hash{""};
  this->selected_blocks(&opt->blocks);
  QByteArray cur_hash = VCWind::checksum_allowed_colorset_option(*opt);

  const bool ret = this->allowed_option_hash_prev != cur_hash;

  // prev_hash = cur_hash;

  return ret;
}

void VCWind::update_hash_allowed(const allowed_colorset_option &opt) noexcept {
  this->allowed_option_hash_prev = this->checksum_allowed_colorset_option(opt);
}

void VCWind::setup_allowed_colorset() noexcept {
  this->setup_basical_colorset();
  this->ui->ac_export_test_schem->setEnabled(true);

  allowed_colorset_option cur_option;
  if (VCL_is_allowed_colorset_ok() &&
      !this->is_allowed_colorset_changed(&cur_option)) {
    return;
  }

  emit signal_allowed_colorset_changed();

  VCL_discard_allowed_blocks();

  if (cur_option.blocks.empty()) {
    this->selected_blocks(&cur_option.blocks);
  }

  const bool success = VCL_set_allowed_blocks(cur_option.blocks.data(),
                                              cur_option.blocks.size());

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

  this->update_hash_allowed(cur_option);
  this->ui->gb_convert_algo->setTitle(
      VCWind::tr("调色算法 (共%1种颜色)")
          .arg(VCL_get_allowed_colors(nullptr, 0)));
  this->clear_convert_cache();
  this->ui->ac_browse_allowed_colors->setEnabled(true);
}

size_t VCWind::selected_blocks(
    std::vector<VCL_block *> *blocks_dest) const noexcept {
  size_t counter = 0;
  if (blocks_dest != nullptr) blocks_dest->clear();
  for (auto &pair : this->map_VC_block_class) {
    counter += pair.second->selected_blocks(blocks_dest, true);
  }

  // cout << "selected block count : " << counter << endl;

  return counter;
}

void VCWind::on_tb_add_images_clicked() noexcept {
  static QString prev_dir{""};
  QStringList files =
      QFileDialog::getOpenFileNames(this, VCWind::tr("选择图片（可多选）"),
                                    prev_dir, "*.bmp *.png *.jpg *.jpeg");
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

  this->flush_export_tabel();
}

void VCWind::on_tb_remove_images_clicked() noexcept {
  auto selected_items = this->ui->lw_image_files->selectedItems();

  for (auto item : selected_items) {
    auto it = this->image_cache.find(item->text());

    if (it != this->image_cache.end()) {
      this->image_cache.erase(it);
    }
    this->ui->lw_image_files->removeItemWidget(item);
    delete item;
  }

  this->flush_export_tabel();
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

  {
    allowed_colorset_option opt;
    const bool is_basical_changed = this->is_basical_colorset_changed();
    const bool is_allowed_changed = this->is_allowed_colorset_changed(&opt);

    if (is_allowed_changed) {
      VCL_discard_allowed_blocks();
    }
    if (is_basical_changed) {
      VCL_discard_resource();
    }
    if (is_basical_changed || is_allowed_changed) {
      this->clear_convert_cache();
      // this->setup_allowed_colorset();
    }
  }

  this->show_image(it);
}

void VCWind::clear_convert_cache() noexcept {
  this->ui->lable_converted->setPixmap(QPixmap(0, 0));
  for (auto &pair : this->image_cache) {
    pair.second.second = QImage();

    assert(pair.second.second.isNull());
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

bool VCWind::is_convert_algo_changed() const noexcept {
  static bool first_run{true};
  static convert_option prev;

  auto current = this->current_convert_option();
  if (first_run) {
    prev = current;
    return true;
  }

  return prev != current;
}

void VCWind::when_algo_dither_bottons_toggled() noexcept {
  allowed_colorset_option temp;

  if (this->is_convert_algo_changed() ||
      this->is_allowed_colorset_changed(&temp)) {
    this->clear_convert_cache();
  }
}

void VCWind::on_tabWidget_main_currentChanged(int) noexcept {}

void VCWind::on_ac_load_resource_triggered() noexcept {
  this->setup_basical_colorset();
}

void VCWind::on_ac_set_allowed_triggered() noexcept {
  this->setup_allowed_colorset();
}
