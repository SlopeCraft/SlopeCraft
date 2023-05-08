#ifndef SLOPECRAFT_SLOPECRAFT_SCWIND_H
#define SLOPECRAFT_SLOPECRAFT_SCWIND_H

#include <QMainWindow>
#include <SlopeCraftL.h>
#include <memory>
#include <QRadioButton>
#include <vector>
#include <BlockListManager.h>
#include <QProgressBar>
#include "cvt_task.h"
#include "PoolModel.h"

class SCWind;

namespace Ui {
class SCWind;
}

class SCWind : public QMainWindow {
  Q_OBJECT
 public:
  explicit SCWind(QWidget* parent = nullptr);
  ~SCWind();
 private slots:
  void on_pb_add_image_clicked() noexcept;
  void on_pb_remove_image_clicked() noexcept;
  void on_cb_lv_cvt_icon_mode_clicked() noexcept;

  void on_pb_load_preset_clicked() noexcept;
  void on_pb_save_preset_clicked() noexcept;

  void on_pb_prefer_concrete_clicked() noexcept;
  void on_pb_prefer_wool_clicked() noexcept;
  void on_pb_prefer_glass_clicked() noexcept;
  void on_pb_prefer_planks_clicked() noexcept;
  void on_pb_prefer_logs_clicked() noexcept;

  void when_cvt_pool_selectionChanged() noexcept;
  void when_version_buttons_toggled() noexcept;
  void when_type_buttons_toggled() noexcept;
  void when_blocklist_changed() noexcept;
  void when_preset_clicked() noexcept;
  void when_export_type_toggled() noexcept;
  void when_algo_btn_clicked() noexcept;

  void on_pb_cvt_current_clicked() noexcept;
  void on_pb_cvt_all_clicked() noexcept;
  void on_pb_save_converted_clicked() noexcept;

 private:
  Ui::SCWind* ui;
  SlopeCraft::Kernel* kernel;

  task_pool_t tasks;
  CvtPoolModel* cvt_pool_model{nullptr};
  PoolModel* export_pool_model{nullptr};

  std::array<blockListPreset, 4> default_presets;

  std::array<QRadioButton*, 20 - 12 + 1> version_buttons() noexcept;
  std::array<const QRadioButton*, 20 - 12 + 1> version_buttons() const noexcept;

  std::array<QRadioButton*, 3> type_buttons() noexcept;
  std::array<const QRadioButton*, 3> type_buttons() const noexcept;

  std::array<QRadioButton*, 5> export_type_buttons() noexcept;
  std::array<const QRadioButton*, 5> export_type_buttons() const noexcept;

  std::array<QRadioButton*, 4> preset_buttons_no_custom() noexcept;

  std::array<const QRadioButton*, 6> algo_buttons() const noexcept;
  std::array<QRadioButton*, 6> algo_buttons() noexcept;

  QProgressBar* current_bar() noexcept;

  SCL_gameVersion selected_version() const noexcept;

  SCL_mapTypes selected_type() const noexcept;

  std::vector<int> selected_indices() const noexcept;
  std::optional<int> selected_cvt_task_idx() const noexcept;

  SCL_convertAlgo selected_algo() const noexcept;
  bool is_dither_selected() const noexcept;

  void kernel_set_type() noexcept;

  void update_button_states() noexcept;

  void kernel_set_image(int idx) noexcept;
  void kernel_convert_image() noexcept;

  void kernel_make_cache() noexcept;

  bool kernel_check_colorset_hash() noexcept;

  void refresh_current_cvt_display(std::optional<int> idx,
                                   bool is_image_coneverted_in_kernel) noexcept;
  QImage get_converted_image_from_kernel() const noexcept;

  void refresh_current_cvt_display(std::optional<int> idx) noexcept {
    this->refresh_current_cvt_display(idx, false);
  }

  void refresh_current_cvt_display() noexcept {
    this->refresh_current_cvt_display(this->selected_cvt_task_idx());
  }

  void mark_all_task_unconverted() noexcept;

  void export_current_cvted_image(int idx, QString filename) noexcept;

 signals:
  void image_changed();
};

#endif  // SLOPECRAFT_SLOPECRAFT_SCWIND_H