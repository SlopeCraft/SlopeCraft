#include "PoolModel.h"

CvtPoolModel::CvtPoolModel(QObject* parent, task_pool_t* poolptr)
    : QAbstractListModel(parent), pool(poolptr) {}

CvtPoolModel::~CvtPoolModel() {}

QVariant CvtPoolModel::data(const QModelIndex& idx, int role) const {
  if (role == Qt::ItemDataRole::DisplayRole) {
    return this->pool->at(idx.row()).filename;
  }

  if (role == Qt::ItemDataRole::DecorationRole) {
    auto raw_image =
        QPixmap::fromImage(this->pool->at(idx.row()).original_image);
    auto img = raw_image.scaledToWidth(this->listview->size().width());
    return QIcon{raw_image};
  }

  return QVariant{};

  // return QVariant{};

  return QVariant{};
}
