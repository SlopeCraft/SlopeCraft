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
  enum class export_type {
    litematica,
    vanilla_structure,
    WE_schem,
    flat_diagram,
    data_file
  };
 private slots:
  void on_pb_add_image_clicked() noexcept;
  void on_pb_remove_image_clicked() noexcept;
  void on_cb_lv_cvt_icon_mode_clicked() noexcept;
  void on_cb_compress_lossy_toggled(bool checked) noexcept;

  void on_pb_load_preset_clicked() noexcept;
  void on_pb_save_preset_clicked() noexcept;

  void on_pb_prefer_concrete_clicked() noexcept;
  void on_pb_prefer_wool_clicked() noexcept;
  void on_pb_prefer_glass_clicked() noexcept;
  void on_pb_prefer_planks_clicked() noexcept;
  void on_pb_prefer_logs_clicked() noexcept;

  void when_cvt_pool_selectionChanged() noexcept;
  void when_export_pool_selectionChanged() noexcept;
  void when_version_buttons_toggled() noexcept;
  void when_type_buttons_toggled() noexcept;
  void when_blocklist_changed() noexcept;
  void when_preset_clicked() noexcept;
  void when_export_type_toggled() noexcept;
  void when_algo_btn_clicked() noexcept;

  void on_pb_cvt_current_clicked() noexcept;
  void on_pb_cvt_all_clicked() noexcept;
  void on_pb_save_converted_clicked() noexcept;

  void on_pb_build3d_clicked() noexcept;
  void on_pb_preview_materials_clicked() noexcept;
  void on_pb_export_all_clicked() noexcept;

 private:
  Ui::SCWind* ui;
  SlopeCraft::Kernel* kernel;

  task_pool_t tasks;
  CvtPoolModel* cvt_pool_model{nullptr};
  ExportPoolModel* export_pool_model{nullptr};

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

  std::vector<cvt_task*> selected_export_task_list() const noexcept;
  std::optional<cvt_task*> selected_export_task() const noexcept;

  SCL_convertAlgo selected_algo() const noexcept;
  bool is_dither_selected() const noexcept;

  bool is_lossless_compression_selected() const noexcept;
  bool is_lossy_compression_selected() const noexcept;
  int current_max_height() const noexcept;
  SCL_compressSettings current_compress_method() const noexcept;

  bool is_glass_bridge_selected() const noexcept;
  int current_glass_brigde_interval() const noexcept;
  SCL_glassBridgeSettings current_glass_method() const noexcept;

  bool is_fire_proof_selected() const noexcept;
  bool is_enderman_proof_selected() const noexcept;
  bool is_connect_mushroom_selected() const noexcept;

  SlopeCraft::Kernel::build_options current_build_option() const noexcept;

  export_type selected_export_type() const noexcept;

  // kernel related functions
  void kernel_set_type() noexcept;

  void update_button_states() noexcept;

  void kernel_set_image(int idx) noexcept;
  void kernel_convert_image() noexcept;

  void kernel_make_cvt_cache() noexcept;

  // bool kernel_check_colorset_hash() noexcept;

  void kernel_build_3d() noexcept;
  void kernel_make_build_cache() noexcept;

  void refresh_current_cvt_display(std::optional<int> idx,
                                   bool is_image_coneverted_in_kernel) noexcept;
  void refresh_current_cvt_display(std::optional<int> idx) noexcept {
    this->refresh_current_cvt_display(idx, false);
  }
  void refresh_current_cvt_display() noexcept {
    this->refresh_current_cvt_display(this->selected_cvt_task_idx());
  }

  QImage get_converted_image_from_kernel() const noexcept;

  void refresh_current_build_display(std::optional<cvt_task*> taskp,
                                     bool is_image_built_in_kernel) noexcept;
  void refresh_current_build_display(std::optional<cvt_task*> taskp) noexcept {
    return this->refresh_current_build_display(taskp, false);
  }
  void refresh_current_build_display() noexcept {
    this->refresh_current_build_display(this->selected_export_task());
  }

  void mark_all_task_unconverted() noexcept;

  void export_current_cvted_image(int idx, QString filename) noexcept;

 signals:
  void image_changed();
};

QString extension_of_export_type(SCWind::export_type) noexcept;

#endif  // SLOPECRAFT_SLOPECRAFT_SCWIND_H