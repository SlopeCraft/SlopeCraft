#ifndef SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H
#define SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H

#include <QDialog>
#include <SlopeCraftL.h>
#include <vector>
#include <span>
#include <QAbstractItemModel>

class PreviewWind;
class MaterialModel;

namespace Ui {
class PreviewWind;
}

struct material_item {
  const SlopeCraft::AbstractBlock* blk{nullptr};
  int count{0};
};

class PreviewWind : public QDialog {
  Q_OBJECT
 private:
  Ui::PreviewWind* ui;
  std::vector<material_item> mat_list;
  MaterialModel* mmp{nullptr};

  void set_size(std::span<int, 3> size) & noexcept;
  void set_total_count(int count) & noexcept;

 public:
  explicit PreviewWind(QWidget* parent = nullptr);
  ~PreviewWind();

  const auto& material_list() const noexcept { return this->mat_list; }
  void setup_data(const SlopeCraft::Kernel* kernel) noexcept;
};

class MaterialModel : public QAbstractTableModel {
  Q_OBJECT
 private:
  const std::vector<material_item>* mat_list{nullptr};

 public:
  explicit MaterialModel(QObject* parent = nullptr);
  ~MaterialModel();

  auto mat_list_pointer() const noexcept { return this->mat_list; }

  void set_mat_list_pointer(const std::vector<material_item>* mlp) noexcept {
    this->mat_list = mlp;
  }

 public slots:
  void refresh() noexcept;

 public:
  QVariant data(const QModelIndex& qmi,
                int role = Qt::DisplayRole) const noexcept override;
  int columnCount(
      const QModelIndex& parent = QModelIndex{}) const noexcept override;
  int rowCount(
      const QModelIndex& parent = QModelIndex{}) const noexcept override;
};

#endif  // SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H