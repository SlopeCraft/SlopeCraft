#ifndef SLOPECRAFT_SLOPECRAFT_POOLMODEL_H
#define SLOPECRAFT_SLOPECRAFT_POOLMODEL_H

#include <QModelIndex>
#include <QAbstractItemModel>
#include <QListView>
#include <cvt_task.h>

class PoolModel : public QAbstractListModel {
  Q_OBJECT
 protected:
  task_pool_t* pool{nullptr};
  QListView* _listview{nullptr};

 public:
  explicit PoolModel(QObject* parent = nullptr, task_pool_t* poolptr = nullptr);
  ~PoolModel();

  int rowCount(const QModelIndex& midx) const override {
    if (midx.isValid()) {
      return 0;
    }
    return this->pool->size();
  }

  QModelIndex parent(const QModelIndex&) const override {
    return QModelIndex{};
  }

  QVariant data(const QModelIndex& idx, int role) const override;

 public slots:
  void refresh() noexcept {
    emit dataChanged(this->index(0, 0), this->index(this->rowCount({}), 0));
  }

 public:
  task_pool_t* attached_pool() noexcept { return this->pool; }
  const task_pool_t* attached_pool() const noexcept { return this->pool; }
  void set_pool(task_pool_t* _pool) noexcept { this->pool = _pool; }

  QListView* attached_listview() noexcept { return this->_listview; }
  const QListView* attached_listview() const noexcept {
    return this->_listview;
  }
  void set_listview(QListView* lv) noexcept { this->_listview = lv; }
};

class CvtPoolModel : public PoolModel {
  Q_OBJECT
 public:
  explicit CvtPoolModel(QObject* parent = nullptr,
                        task_pool_t* poolptr = nullptr);
  ~CvtPoolModel();

  Qt::DropActions supportedDropActions() const override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QStringList mimeTypes() const override;

  QMimeData* mimeData(const QModelIndexList& indexes) const override;

  bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row,
                       int column, const QModelIndex& parent) const override;

  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row,
                    int column, const QModelIndex& parent) override;
};

class ExportPoolModel : public PoolModel {
  Q_OBJECT
 public:
  explicit ExportPoolModel(QObject* parent = nullptr,
                           task_pool_t* poolptr = nullptr);
  ~ExportPoolModel();

  int rowCount(const QModelIndex& midx) const override {
    if (midx.isValid()) {
      return 0;
    }
    int num = 0;
    for (const auto& i : *this->pool) {
      if (i.is_converted) {
        num++;
      }
    }
    return num;
  }

  std::vector<int> iteration_map() const noexcept {
    std::vector<int> ret;
    ret.reserve(this->pool->size());
    for (int i = 0; i < (int)this->pool->size(); i++) {
      if (this->pool->at(i).is_converted) {
        ret.emplace_back(i);
      }
    }
    return ret;
  }

  int export_idx_to_full_idx(int eidx) const noexcept {
    assert(eidx >= 0);
    for (int fidx = 0; fidx < (int)this->pool->size(); fidx++) {
      if (pool->at(fidx).is_converted) {
        eidx--;
      }
      if (eidx < 0) {
        return fidx;
      }
    }

    assert(false);
    return INT_MAX;
  }

  QVariant data(const QModelIndex& idx, int role) const override;
};

#endif  // SLOPECRAFT_SLOPECRAFT_POOLMODEL_H