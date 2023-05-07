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

  int rowCount(const QModelIndex&) const override { return this->pool->size(); }

  QModelIndex parent(const QModelIndex&) const override {
    return QModelIndex{};
  }

  QVariant data(const QModelIndex& idx, int role) const override;

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

#endif  // SLOPECRAFT_SLOPECRAFT_POOLMODEL_H