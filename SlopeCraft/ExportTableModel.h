#ifndef SLOPECRAFT_SLOPECRAFT_EXPORTTABLEMODEL_H
#define SLOPECRAFT_SLOPECRAFT_EXPORTTABLEMODEL_H

#include <QAbstractItemModel>
#include "cvt_task.h"

class SCWind;

class ExportTableModel : public QAbstractTableModel {
  Q_OBJECT
 private:
  task_pool& pool;

 public:
  explicit ExportTableModel(SCWind* parent);
  ~ExportTableModel();

  int rowCount(const QModelIndex&) const noexcept override;
  int columnCount(const QModelIndex&) const noexcept override;

  QVariant data(const QModelIndex&, int role) const noexcept override;

  SCWind* scwind() const noexcept;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const noexcept override;

 public slots:
  void refresh() noexcept;
};

QSize map_size_of_images(QSize image_size) noexcept;

QString map_data_filename(QString dir, int seq_number) noexcept;

#endif  //  SLOPECRAFT_SLOPECRAFT_EXPORTTABLEMODEL_H