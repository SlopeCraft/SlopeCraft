#include "VCWind.h"
#include "ui_VCWind.h"

#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>

#include <advanced_qlist_widget_item.h>

VCWind::VCWind(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::VCWind), kernel(VCL_create_kernel()) {
  ui->setupUi(this);

  // this->ui->lv_rp;

  this->append_default_to_rp_or_bsl(this->ui->lw_rp, true);
  this->append_default_to_rp_or_bsl(this->ui->lw_bsl, false);
}

VCWind::~VCWind() { delete ui; }

void VCWind::append_default_to_rp_or_bsl(QListWidget *qlw,
                                         bool is_rp) noexcept {

  const QString txt = is_rp ? VCWind::tr("原版资源包") : VCWind::tr("原版json");
  advanced_qlwi *aqlwi = new advanced_qlwi(txt, true);
  qlw->addItem(aqlwi);
}

/*
bool VCWind::have_special(QListWidget *qlw) noexcept {
  for (int i = 0; i < qlw->count(); i++) {
    auto qlwi = qlw->item(i);
    if (dynamic_cast<advanced_qlwi *>(qlwi)->is_special()) {
      return true;
    }
  }

  return false;
}*/

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
