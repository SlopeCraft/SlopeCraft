#ifndef SLOPECRAFT_SLOPECRAFT_POOLMODEL_H
#define SLOPECRAFT_SLOPECRAFT_POOLMODEL_H

#include <QModelIndex>
#include <QAbstractItemModel>
#include <QListView>
#include <cvt_task.h>

class CvtPoolModel : public QAbstractListModel {
  Q_OBJECT
 public:
  explicit CvtPoolModel(QObject* parent = nullptr,
                        task_pool_t* poolptr = nullptr);
  ~CvtPoolModel();

  int rowCount(const QModelIndex&) const override { return this->pool->size(); }

  QModelIndex parent(const QModelIndex&) const override {
    return QModelIndex{};
  }

  QVariant data(const QModelIndex& idx, int role) const override;

  void refresh() noexcept {
    emit dataChanged(this->index(0, 0), this->index(this->pool->size(), 0));
  }

  Qt::DropActions supportedDropActions() const override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QStringList mimeTypes() const override;

  QMimeData* mimeData(const QModelIndexList& indexes) const override;

  bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row,
                       int column, const QModelIndex& parent) const override;

  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row,
                    int column, const QModelIndex& parent) override;

 private:
  task_pool_t* pool{nullptr};
  QListView* listview{nullptr};

 public:
  task_pool_t* attached_pool() noexcept { return this->pool; }
  const task_pool_t* attached_pool() const noexcept { return this->pool; }
  void set_pool(task_pool_t* _pool) noexcept { this->pool = _pool; }

  QListView* attached_listview() noexcept { return this->listview; }
  const QListView* attached_listview() const noexcept { return this->listview; }
  void set_listview(QListView* lv) noexcept { this->listview = lv; }
};

#endif  // SLOPECRAFT_SLOPECRAFT_POOLMODEL_H