#ifndef SLOPECRAFT_SLOPECRAFT_SCWIND_H
#define SLOPECRAFT_SLOPECRAFT_SCWIND_H

#include <tuple>
#include <vector>
#include <memory>
#include <QMainWindow>
#include <QRadioButton>
#include <QProgressBar>
#include <QTranslator>
#include <QNetworkAccessManager>
#include <SlopeCraftL.h>
#include <BlockListManager.h>
#include "cvt_task.h"
#include "PoolModel.h"
#include "ExportTableModel.h"
#include "MemoryPolicyDialog.h"

class SCWind;

namespace Ui {
class SCWind;
}

struct colortable_settings {
  colortable_settings() = delete;
  colortable_settings(const selection& s, SCL_mapTypes t)
      : selection_{s}, map_type{t} {}

  colortable_settings(selection&& s, SCL_mapTypes t)
      : selection_{std::move(s)}, map_type{t} {}

  selection selection_;
  SCL_mapTypes map_type;

  [[nodiscard]] bool operator==(const colortable_settings& b) const noexcept {
    if (this->selection_ not_eq b.selection_) {
      return false;
    }
    if (this->map_type not_eq b.map_type) {
      return false;
    }
    return true;
  }
};

template <>
struct std::hash<colortable_settings> {
  uint64_t operator()(const colortable_settings& s) const noexcept {
    return std::hash<selection>{}(s.selection_) xor
           static_cast<int>(s.map_type);
  }
};

SlopeCraft::progress_callbacks progress_callback(QProgressBar* bar) noexcept;

SlopeCraft::const_image_reference wrap_image(const QImage& img) noexcept;

[[nodiscard]] QImage get_converted_image(
    const SlopeCraft::converted_image&) noexcept;

class SCWind : public QMainWindow {
  Q_OBJECT
 private:
  void connect_slots() noexcept;

 public:
  explicit SCWind(QWidget* parent = nullptr);
  ~SCWind();

  inline static QNetworkAccessManager& network_manager() noexcept {
    static QNetworkAccessManager manager;
    return manager;
  }

  static const QString& default_wind_title() noexcept;

  static QString workStatus_to_string(::SCL_workStatus) noexcept;

  const static QString update_url;

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
  void on_pb_replace_image_clicked() noexcept;

  void on_cb_lv_cvt_icon_mode_clicked() noexcept;
  void on_cb_compress_lossy_toggled(bool checked) noexcept;

  void on_pb_load_preset_clicked() noexcept;
  void on_pb_save_preset_clicked() noexcept;

  void on_pb_prefer_concrete_clicked() noexcept;
  void on_pb_prefer_wool_clicked() noexcept;
  void on_pb_prefer_glass_clicked() noexcept;
  void on_pb_prefer_planks_clicked() noexcept;
  void on_pb_prefer_logs_clicked() noexcept;
  void on_pb_prefer_slabs_clicked() noexcept;

  void on_pb_select_all_clicked() noexcept;
  void on_pb_deselect_all_clicked() noexcept;
  void on_pb_invselect_clicked() noexcept;

  void when_cvt_pool_selectionChanged() noexcept;
  void when_export_pool_selectionChanged() noexcept;
  void when_version_buttons_toggled(bool) noexcept;
  void when_type_buttons_toggled(bool) noexcept;
  void when_blocklist_changed() noexcept;
  void when_preset_clicked() noexcept;
  void when_export_type_toggled() noexcept;
  void when_algo_btn_clicked() noexcept;

  void on_pb_cvt_current_clicked() noexcept;
  void on_pb_cvt_all_clicked() noexcept;
  void on_pb_save_converted_clicked() noexcept;

  void on_pb_build3d_clicked() noexcept;
  void on_pb_preview_materials_clicked() noexcept;
  void on_pb_preview_compress_effect_clicked() noexcept;
  void on_pb_export_all_clicked() noexcept;
  // exports for data file
  void on_pb_export_file_clicked() noexcept;
  void on_pb_export_data_command_clicked() noexcept;
  void on_pb_export_data_vanilla_structure_clicked() noexcept;
  void when_data_file_command_changed() noexcept;

 private slots:
  void on_ac_GAcvter_options_triggered() noexcept;

  void on_ac_cache_dir_open_triggered() noexcept;
  void on_ac_clear_cache_triggered() noexcept;

  void on_ac_about_triggered() noexcept;

  void on_ac_get_current_colorlist_triggered() noexcept;
  void on_ac_test_blocklist_triggered() noexcept;

  void on_ac_cache_all_3d_triggered() noexcept;

  void on_ac_memory_policy_triggered() noexcept;

  void on_ac_blocklist_triggered() noexcept;

 private:
  Ui::SCWind* ui;

  std::unordered_map<
      colortable_settings,
      std::unique_ptr<SlopeCraft::color_table, SlopeCraft::deleter>>
      color_tables;
  //  SlopeCraft::Kernel* kernel;

  CvtPoolModel* cvt_pool_model{nullptr};
  ExportPoolModel* export_pool_model{nullptr};
  ExportTableModel* export_table_model{nullptr};

  std::array<blockListPreset, 4> default_presets;

  SCL_language language{SCL_language::Chinese};
  std::vector<QTranslator*> translators;

  QString prev_load_image_dir{""};

  memory_policy mem_policy{};
  // QString fileonly_export_dir{""};

 public:
  SlopeCraft::GA_converter_option GA_option{};
  task_pool tasks;

  QString cache_root_dir() const noexcept;
  SlopeCraft::color_table* current_color_table() noexcept;
  SlopeCraft::convert_option current_convert_option() noexcept;

  SlopeCraft::ui_callbacks ui_callbacks() const noexcept;

  std::array<QRadioButton*, 21 - 12 + 1> version_buttons() noexcept;
  std::array<const QRadioButton*, 21 - 12 + 1> version_buttons() const noexcept;

  std::array<QRadioButton*, 3> type_buttons() noexcept;
  std::array<const QRadioButton*, 3> type_buttons() const noexcept;

  std::array<QRadioButton*, 5> export_type_buttons() noexcept;
  std::array<const QRadioButton*, 5> export_type_buttons() const noexcept;
  std::vector<QRadioButton*> valid_export_type_buttons(
      SCL_mapTypes type) const noexcept;

  std::array<QRadioButton*, 4> preset_buttons_no_custom() noexcept;

  std::array<const QRadioButton*, 6> algo_buttons() const noexcept;
  std::array<QRadioButton*, 6> algo_buttons() noexcept;

  QProgressBar* current_bar() noexcept;

  SCL_gameVersion selected_version() const noexcept;

  SCL_mapTypes selected_type() const noexcept;

  std::vector<int> selected_indices() const noexcept;
  std::optional<int> selected_cvt_task_idx() const noexcept;

  std::vector<cvt_task*> selected_export_task_list() const noexcept;
  cvt_task* selected_export_task() const noexcept;

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

  SlopeCraft::build_options current_build_option() const noexcept;

  export_type selected_export_type() const noexcept;

  std::optional<SlopeCraft::litematic_options> current_litematic_option(
      QString& err) const noexcept;
  std::optional<SlopeCraft::vanilla_structure_options> current_nbt_option(
      QString& err) const noexcept;
  std::optional<SlopeCraft::WE_schem_options> current_schem_option(
      QString& err) const noexcept;
  std::optional<SlopeCraft::flag_diagram_options> current_flatdiagram_option(
      QString& err) const noexcept;

  int current_map_begin_seq_number() const noexcept;

  SlopeCraft::assembled_maps_options current_assembled_maps_option()
      const noexcept;

  inline auto lang() const noexcept { return this->language; }
  void set_lang(::SCL_language lang) noexcept;

 private:
  // kernel related functions
  void set_colorset() noexcept;

  void update_button_states() noexcept;

  [[nodiscard]] std::unique_ptr<SlopeCraft::converted_image,
                                SlopeCraft::deleter>
  convert_image(int idx) noexcept;

  [[nodiscard]] std::unique_ptr<SlopeCraft::converted_image,
                                SlopeCraft::deleter>
  convert_image(const cvt_task&) noexcept;

  [[nodiscard]] const SlopeCraft::converted_image& convert_if_need(
      cvt_task&) noexcept;

  [[nodiscard]] std::tuple<const SlopeCraft::converted_image&,
                           const SlopeCraft::structure_3D&>
  convert_and_build_if_need(cvt_task&) noexcept;

  [[nodiscard]] std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter>
  build_3D(const SlopeCraft::converted_image&) noexcept;

  std::tuple<const SlopeCraft::converted_image*,
             const SlopeCraft::structure_3D*>
  load_selected_3D() noexcept;

  //  [[deprecated]] void kernel_set_image(int idx) noexcept;
  //  [[deprecated]] void kernel_convert_image() noexcept;
  //
  //  [[deprecated]] void kernel_make_cvt_cache() noexcept;

  //  [[deprecated]] void kernel_build_3d() noexcept;
  //  [[deprecated]] void kernel_make_build_cache() noexcept;

  void refresh_current_cvt_display(std::optional<int> idx) noexcept;
  void refresh_current_cvt_display() noexcept {
    this->refresh_current_cvt_display(this->selected_cvt_task_idx());
  }

  //  [[deprecated]] QImage get_converted_image_from_kernel() const noexcept;

  void refresh_current_build_display(cvt_task* taskp) noexcept;
  void refresh_current_build_display() noexcept {
    this->refresh_current_build_display(this->selected_export_task());
  }

  //  void mark_all_task_unconverted() noexcept;

  void export_current_cvted_image(int idx, QString filename) noexcept;

  void report_error(::SCL_errorFlag flag, const char* msg) noexcept;

  [[nodiscard]] bool should_auto_cache(bool suppress_warnings) noexcept;

  struct auto_cache_report {
    size_t structures_cached;
    int64_t memory_saved;
  };
  auto_cache_report auto_cache_3D(bool cache_all = false) noexcept;

  [[nodiscard]] tl::expected<QString, QString> get_command(
      const SlopeCraft::converted_image&, int begin_idx) const noexcept;

 signals:
  void image_changed();
};

QString extension_of_export_type(SCWind::export_type) noexcept;

#endif  // SLOPECRAFT_SLOPECRAFT_SCWIND_H