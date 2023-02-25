#include "VCWind.h"
#include "VC_block_class.h"
#include "advanced_qlist_widget_item.h"
#include "ui_VCWind.h"

#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <magic_enum.hpp>
#include <thread>

VCWind::VCWind(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::VCWind), kernel(VCL_create_kernel()) {
  ui->setupUi(this);

  // this->ui->lv_rp;

  this->append_default_to_rp_or_bsl(this->ui->lw_rp, true);
  this->append_default_to_rp_or_bsl(this->ui->lw_bsl, false);

  this->ui->sb_threads->setValue(std::thread::hardware_concurrency());

  // for test
  connect(this->ui->action_test01, &QAction::triggered, this,
          &VCWind::make_block_list_page);
}

VCWind::~VCWind() { delete this->ui; }

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
    VC_block_class *class_widget = new VC_block_class(this);
    const size_t idx = size_t(bcl);

    this->ui->gl_sa_blocks->addWidget(class_widget, idx, 0);
    class_widget->show();

    class_widget->set_blocks(blocks[bcl].size(), blocks[bcl].data(), 4);
    class_widget->setTitle(QString::fromUtf8(magic_enum::enum_name(bcl)) +
                           " (" + QString::number(int(bcl)) + ") ");
  }
}