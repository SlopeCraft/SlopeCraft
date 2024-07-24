#ifndef SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H
#define SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H

#include <QDialog>
#include <SlopeCraftL.h>
#include <vector>
#include <span>
#include <memory>
#include <QAbstractItemModel>

class PreviewWind;
class MaterialModel;

namespace Ui {
class PreviewWind;
}

struct material_item {
  const SlopeCraft::mc_block_interface* blk{nullptr};
  size_t count{0};
};

class PreviewWind : public QDialog {
  Q_OBJECT
 private:
  std::unique_ptr<Ui::PreviewWind> ui;
  std::vector<material_item> mat_list;
  MaterialModel* mmp{nullptr};

  QString export_mat_list_prev_dir;

  void set_size(std::span<size_t, 3> size) & noexcept;
  void set_total_count(size_t count) & noexcept;

 public:
  explicit PreviewWind(QWidget* parent = nullptr);
  ~PreviewWind();

  const auto& material_list() const noexcept { return this->mat_list; }
  void setup_data(const SlopeCraft::color_table&,
                  const SlopeCraft::structure_3D&) noexcept;

  enum class sort_option { no_sort, ascending, descending };

  bool is_unit_stack() const noexcept;
  sort_option current_sort_option() const noexcept;

 private slots:
  void on_pb_export_file_clicked() noexcept;
};

class MaterialModel : public QAbstractTableModel {
  Q_OBJECT
 private:
  const std::vector<material_item>* mat_list{nullptr};
  const PreviewWind* const pwind;

 public:
  explicit MaterialModel(PreviewWind* parent = nullptr);
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