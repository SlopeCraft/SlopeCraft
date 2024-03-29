#include "ExportTableModel.h"
#include "SCWind.h"

ExportTableModel::ExportTableModel(SCWind* parent, const task_pool_t* poolptr)
    : QAbstractTableModel(parent), pool{poolptr} {}

ExportTableModel::~ExportTableModel() {}

const SCWind* ExportTableModel::scwind() const noexcept {
  return dynamic_cast<const SCWind*>(this->parent());
}

QSize map_size_of_images(QSize image_size) noexcept {
  const int width = std::ceil(image_size.width() / 128.0);
  const int height = std::ceil(image_size.height() / 128.0);
  return QSize{width, height};
}

map_range map_range_at_index(const task_pool_t& pool, int first_map_seq_num,
                             int asked_idx) noexcept {
  assert(asked_idx >= 0 && asked_idx < (int)pool.size());

  int current_start_seq = first_map_seq_num;

  for (int idx = 0; idx < asked_idx; idx++) {
    const auto current_map_size =
        map_size_of_images(pool[idx].original_image.size());
    const int current_map_num =
        current_map_size.height() * current_map_size.width();

    current_start_seq += current_map_num;
  }

  const auto cms = map_size_of_images(pool[asked_idx].original_image.size());
  const int cmn = cms.height() * cms.width();

  return map_range{current_start_seq, current_start_seq + cmn - 1};
}

QString map_data_filename(QString dir, int seq_number) noexcept {
  return QStringLiteral("%1/map_%2.dat").arg(dir).arg(seq_number);
}

void ExportTableModel::refresh() noexcept {
  // emit
  emit this->layoutChanged();
  emit this->dataChanged(this->index(0, 0), this->index(this->rowCount({}),
                                                        this->columnCount({})));
}

int ExportTableModel::rowCount(const QModelIndex&) const noexcept {
  return ::converted_task_count(*this->pool);
}

int ExportTableModel::columnCount(const QModelIndex&) const noexcept {
  return 6;
}

QVariant ExportTableModel::data(const QModelIndex& qmi,
                                int role) const noexcept {
  if (!qmi.isValid()) {
    return {};
  }

  const int r = qmi.row();
  const int c = qmi.column();

  if (r < 0 || r >= (int)this->rowCount({})) {
    return {};
  }

  const auto& task =
      this->pool->at(::map_export_idx_to_full_idx(*this->pool, r));

  if (role == Qt::ItemDataRole::DisplayRole) {
    const QSize map_size = map_size_of_images(task.original_image.size());

    const int map_count = map_size.height() * map_size.width();
    const int beg_idx = this->scwind()->current_map_begin_seq_number();
    const auto range = map_range_at_index(*this->pool, beg_idx, r);

    switch (c) {
      case 0:
        return task.filename;
      case 1:
        return QStringLiteral("%1 × %2")
            .arg(task.original_image.height())
            .arg(task.original_image.width());
      case 2:
        return QStringLiteral("%1 × %2 = %3")
            .arg(map_size.height())
            .arg(map_size.width())
            .arg(map_count);
      case 3:
        return QStringLiteral("%1 ~ %2").arg(range.first).arg(range.last);
      case 4:
        return QStringLiteral("map_%1.dat").arg(range.first);
      case 5:
        return QStringLiteral("map_%1.dat").arg(range.last);
    }
  }

  return {};
}

QVariant ExportTableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const noexcept {
  if (orientation != Qt::Orientation::Horizontal ||
      role != Qt::ItemDataRole::DisplayRole ||
      section > this->columnCount({})) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section) {
    case 0:
      return tr("原图文件名");
    case 1:
      return tr("图像大小");
    case 2:
      return tr("地图大小");
    case 3:
      return tr("地图序号范围");
    case 4:
      return tr("第一个地图文件名");
    case 5:
      return tr("最后一个地图文件名");
    default:
      return QAbstractTableModel::headerData(section, orientation, role);
  }
}