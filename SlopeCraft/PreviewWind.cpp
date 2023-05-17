#include "PreviewWind.h"
#include "ui_PreviewWind.h"
#include <QString>
#include <QModelIndex>

PreviewWind::PreviewWind(QWidget* parent)
    : QDialog(parent), ui(new Ui::PreviewWind) {
  this->ui->setupUi(this);

  this->mmp = new MaterialModel(this);
  this->mmp->set_mat_list_pointer(&this->mat_list);
  // this->ui->tv_mat->setModel(this->mmp);

  // connect(this->mmp,&QAbstractTableModel::dataChanged,this->ui->tv_mat,&QTableView::)
}

PreviewWind::~PreviewWind() { delete this->ui; }

void PreviewWind::set_size(std::span<int, 3> size) & noexcept {
  QString size_str =
      QStringLiteral("%1 × %2 × %3").arg(size[0]).arg(size[1]).arg(size[2]);

  this->ui->lb_show_size->setText(size_str);

  this->ui->lb_show_volume->setText(
      QString::number(size[0] * size[1] * size[2]));
}

void PreviewWind::set_total_count(int count) & noexcept {
  this->ui->lb_block_count->setText(QString::number(count));
}

void PreviewWind::setup_data(const SlopeCraft::Kernel* kernel) noexcept {
  std::array<int, 64> count_list;
  count_list.fill(0);

  {
    int total_blks{0};
    kernel->getBlockCounts(&total_blks, count_list.data());
    this->set_total_count(total_blks);
  }
  std::vector<const SlopeCraft::AbstractBlock*> blkp_arr;
  {
    int num = SlopeCraft::SCL_getBlockPalette(nullptr, 0);
    blkp_arr.resize(num);
    SlopeCraft::SCL_getBlockPalette(blkp_arr.data(), blkp_arr.size());
  }

  this->mat_list.resize(blkp_arr.size());
  for (size_t idx = 0; idx < blkp_arr.size(); idx++) {
    this->mat_list[idx] =
        material_item{.blk = blkp_arr[idx], .count = count_list[idx]};
  }

  {
    std::array<int, 3> sz;
    kernel->get3DSize(&sz[0], &sz[1], &sz[2]);
    this->set_size(sz);
  }

  this->ui->tv_mat->setModel(this->mmp);
  this->mmp->refresh();
  this->ui->tv_mat->doItemsLayout();
}

MaterialModel::MaterialModel(QObject* parent) : QAbstractTableModel(parent) {}

MaterialModel::~MaterialModel() {}

void MaterialModel::refresh() noexcept {
  emit this->dataChanged(
      this->index(0, 0),
      this->index(this->rowCount() - 1, this->columnCount() - 1));
}

QVariant MaterialModel::data(const QModelIndex& qmi, int role) const noexcept {
  if (!qmi.isValid()) {
    return {};
  }

  const int r = qmi.row();
  const int c = qmi.column();

  if (r >= (int)this->mat_list->size()) {
    return {};
  }
  if (c >= 2) {
    return {};
  }

  const auto& mat = this->mat_list->at(r);

  if (role == Qt::ItemDataRole::DisplayRole) {
    if (c == 0) {
      return QString::fromUtf8(mat.blk->getNameZH());
    }

    if (c == 1) {
      return QString::number(mat.count);
    }
  }

  if (role == Qt::ItemDataRole::DecorationRole) {
    if (c == 0) {
      QImage img{16, 16, QImage::Format_ARGB32};
      mat.blk->getImage((uint32_t*)img.scanLine(0), true);
      return QIcon{QPixmap::fromImage(img)};
    }
  }

  return {};
}

int MaterialModel::columnCount(const QModelIndex&) const noexcept { return 2; }

int MaterialModel::rowCount(const QModelIndex&) const noexcept {
  // const int rows = this->mat_list->size();
  return this->mat_list->size();
}