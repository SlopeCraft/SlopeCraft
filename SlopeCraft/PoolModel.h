#ifndef SLOPECRAFT_SLOPECRAFT_POOLMODEL_H
#define SLOPECRAFT_SLOPECRAFT_POOLMODEL_H

#include <QModelIndex>
#include <QAbstractItemModel>
#include <QListView>
#include <cvt_task.h>
#include <QImage>

class PoolModel : public QAbstractListModel {
  Q_OBJECT
 protected:
  task_pool_t* pool{nullptr};
  QListView* _listview{nullptr};

  static const QPixmap& icon_empty() noexcept;
  static const QPixmap& icon_converted() noexcept;

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

  static void draw_icon(QPixmap& image, const QPixmap& icon, int index,
                        QWidget* ptr_to_report_error) noexcept;

  void draw_icon(QPixmap& image, const QPixmap& icon,
                 int index) const noexcept {
    draw_icon(image, icon, index, this->_listview);
  }
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
    return converted_task_count(*this->pool);
  }

  std::vector<int> iteration_map() const noexcept {
    return ::iteration_map(*this->pool);
  }

  int export_idx_to_full_idx(int eidx) const noexcept {
    return ::map_export_idx_to_full_idx(*this->pool, eidx);
  }

  cvt_task* export_idx_to_task_ptr(int eidx) const noexcept {
    const int pidx = this->export_idx_to_full_idx(eidx);
    if (pidx >= (int)this->pool->size()) {
      return nullptr;
    }
    return &this->pool->at(pidx);
  }

  QVariant data(const QModelIndex& idx, int role) const override;
};

#endif  // SLOPECRAFT_SLOPECRAFT_POOLMODEL_H