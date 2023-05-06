#include "SCWind.h"
#include "ui_SCWind.h"
#include <QFileDialog>
#include <QMessageBox>
// #include "PoolWidget.h"
SCWind::SCWind(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::SCWind),
      kernel(SlopeCraft::SCL_createKernel()) {
  this->ui->setupUi(this);
  assert(this->ui->lview_pool_cvt != nullptr);
  assert(this->ui->lview_pool_export != nullptr);

  this->cvt_pool_model = new CvtPoolModel{this, &this->tasks};
  this->ui->lview_pool_cvt->setModel(this->cvt_pool_model);
  this->cvt_pool_model->set_listview(this->ui->lview_pool_cvt);
  connect(this->ui->lview_pool_cvt->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &SCWind::when_cvt_pool_selectionChanged);

  this->export_pool_model = new PoolModel{this, &this->tasks};
  this->ui->lview_pool_export->setModel(this->export_pool_model);
  this->export_pool_model->set_listview(this->ui->lview_pool_export);
}

SCWind::~SCWind() {
  delete this->ui;
  SlopeCraft::SCL_destroyKernel(this->kernel);
}

void SCWind::on_pb_add_image_clicked() noexcept {
  auto files =
      QFileDialog::getOpenFileNames(this, tr("选择图片"), "", "*.png;;*.jpg");

  if (files.empty()) {
    return;
  }

  QString err;
  for (const auto &filename : files) {
    auto task = cvt_task::load(filename, err);

    if (!err.isEmpty()) {
      auto ret = QMessageBox::critical(
          this, tr("打开图像失败"),
          tr("无法打开图像 %1 。\n详细信息： %2").arg(filename).arg(err),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                       QMessageBox::StandardButton::Ignore});

      if (ret == QMessageBox::Ignore) {
        return;
      } else {
        abort();
      }
    }

    this->tasks.emplace_back(task);
  }
  this->cvt_pool_model->refresh();
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