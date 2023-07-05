/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SLOPECRAFT_VISUALCRAFT_VCWIND_H
#define SLOPECRAFT_VISUALCRAFT_VCWIND_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <VCLConfigLoader.h>
#include <VisualCraftL.h>
#include <functional>
#include <map>

extern uint8_t is_language_ZH;
extern QNetworkAccessManager *global_manager;

extern QString url_for_update;

class QListWidget;
class QListWidgetItem;
class VC_block_class;
class QCheckBox;

namespace Ui {
class VCWind;
}

class VCWind : public QMainWindow {
  Q_OBJECT
 public:
  struct convert_option {
    SCL_convertAlgo algo;
    bool dither;

    inline bool operator==(convert_option another) const noexcept {
      return this->algo == another.algo && this->dither == another.dither;
    }

    inline bool operator!=(convert_option another) const noexcept {
      return !((*this) == another);
    }
  };

  struct basic_colorset_option {
    std::vector<QString> zips;
    std::vector<QString> jsons;
    SCL_gameVersion version;
    int layers;
    VCL_face_t face;
    VCL_biome_t biome;
    bool is_leaves_transparent;
  };

  struct allowed_colorset_option {
    std::vector<VCL_block *> blocks;
  };

 private:
  // for all pages
  Ui::VCWind *ui;
  VCL_Kernel *const kernel{nullptr};

  // for page 0
  // VCL_resource_pack *rp{nullptr};
  // VCL_block_state_list *bsl{nullptr};
  // bool is_basical_colorset_changed{true};
  QByteArray basical_option_hash_prev{""};
  QByteArray allowed_option_hash_prev{""};

  // for page 1
  std::map<VCL_block_class_t, VC_block_class *> map_VC_block_class{};
  // QByteArray hash_basical{""};
  //  bool is_allowed_colorset_changed{true};
  //   for page 2
  std::map<QString, std::pair<QImage, QImage>> image_cache;
  // QByteArray hash_allowed{""};
  //  for page 3
  //  for page 4
  QPoint prev_compute_device{-3, -3};

  // static bool have_special(QListWidget *qlw) noexcept;

 public:
  explicit VCWind(QWidget *parent = nullptr);
  ~VCWind();

  static VCL_config config;

  inline const auto &block_class_widgets() const noexcept {
    return this->map_VC_block_class;
  }

  void retrieve_latest_version(QString url_api, QNetworkAccessManager &qnam,
                               bool is_manually) noexcept;
 signals:
  void signal_basic_colorset_changed();
  void signal_allowed_colorset_changed();

 private:
  // for all pages
  static void callback_progress_range_set(void *, int, int, int) noexcept;
  static void callback_progress_range_add(void *, int) noexcept;

  // void when_network_finished(QNetworkReply *reply, bool is_manually)
  // noexcept;

  // for page 0 ------------------------------------------
  void setup_ui_select_biome() noexcept;
  //  create and set this->rp
  [[nodiscard]] static VCL_resource_pack *create_resource_pack(
      const basic_colorset_option &opt) noexcept;
  // create and set this->bsl
  [[nodiscard]] static VCL_block_state_list *create_block_state_list(
      const basic_colorset_option &opt) noexcept;

  void update_hash_basic(const basic_colorset_option &opt) noexcept;
  void update_hash_allowed(const allowed_colorset_option &opt) noexcept;

  // receive current selected version from ui
  SCL_gameVersion current_selected_version() const noexcept;
  VCL_face_t current_selected_face() const noexcept;

  basic_colorset_option current_basic_option() const noexcept;

  QByteArray checksum_basic_colorset_option(
      const basic_colorset_option &opt) const noexcept;

  static QByteArray checksum_allowed_colorset_option(
      const allowed_colorset_option &opt) noexcept;

  bool is_basical_colorset_changed() const noexcept;

  static void append_default_to_rp_or_bsl(QListWidget *qlw,
                                          bool is_rp) noexcept;
  void setup_basical_colorset() noexcept;

  // void connect_when_basical_colorset_changed() noexcept;
  //  for page 1 ------------------------------------------
  size_t selected_blocks(std::vector<VCL_block *> *blocks_dest) const noexcept;
  bool is_allowed_colorset_changed(allowed_colorset_option *opt) const noexcept;

  void setup_allowed_colorset() noexcept;

  bool is_convert_algo_changed() const noexcept;

  void apply_selection(
      std::function<void(const VCL_block *, QCheckBox *)>) noexcept;

 public:
  void select_blocks(
      std::function<bool(const VCL_block *)> return_true_for_select) noexcept;

  void deselect_blocks(
      std::function<bool(const VCL_block *)> return_true_for_deselect) noexcept;

  int count_block_matched(std::function<bool(const VCL_block *)>
                              return_true_when_match) const noexcept;

 private:
  // void connect_when_allowed_colorset_changed() noexcept;

  // for page 2 ------------------------------------------
  void setup_image(const QImage &img) noexcept;
  SCL_convertAlgo current_selected_algo() const noexcept;
  void show_image(decltype(image_cache)::iterator) noexcept;

  void clear_convert_cache() noexcept;

  convert_option current_convert_option() const noexcept;

  // for page 3 ------------------------------------------
  QString get_dir_for_export() noexcept;

  void flush_export_tabel() noexcept;

  [[nodiscard]] std::unordered_map<std::string_view, QCheckBox *>
  id_blockclass_map() noexcept;

  static constexpr int export_col_filename = 0;
  static constexpr int export_col_imagesize = 1;
  static constexpr int export_col_lite = 2;
  static constexpr int export_col_structure = 3;
  static constexpr int export_col_schem = 4;
  static constexpr int export_col_converted = 5;
  static constexpr int export_col_flagdiagram = 6;
  static constexpr int export_col_progress = 7;

  // returns false means to skip current task.
  bool export_lite(const QString &, const QString &image_filename) noexcept;
  bool export_structure(const QString &,
                        const QString &image_filename) noexcept;
  bool export_schem(const QString &, const QString &image_filename) noexcept;
  bool export_converted(const QString &, const QImage &) noexcept;
  bool export_flatdiagram(const QString &) noexcept;

  // for page 4 ------------------------------------------
  void refresh_gpu_info() noexcept;
  void select_default_device() noexcept;

  QString update_gpu_device(QPoint current_choice) noexcept;
 private slots:
  // for all pages ------------------------------------------

  void on_ac_flush_warnings_triggered() noexcept;
  void on_ac_export_test_schem_triggered() noexcept;

  // auto connected
  void on_tabWidget_main_currentChanged(int page) noexcept;

  // auto connected
  void on_ac_tutorial_triggered() noexcept;
  void on_ac_about_VisualCraft_triggered() noexcept;
  void on_ac_contact_bilibili_triggered() noexcept;
  void on_ac_contact_github_repo_triggered() noexcept;
  void on_ac_report_bugs_triggered() noexcept;

  // auto connected
  void on_ac_load_resource_triggered() noexcept;
  void on_ac_set_allowed_triggered() noexcept;

  // auto connected
  void on_ac_browse_block_triggered() noexcept;
  void on_ac_browse_biome_triggered() noexcept;
  void on_ac_browse_basic_colors_triggered() noexcept;
  void on_ac_browse_allowed_colors_triggered() noexcept;

  // auto connected
  void on_ac_check_update_triggered() noexcept;

  // for page 0 ------------------------------------------

  // auto connected
  void on_pb_add_rp_clicked() noexcept;
  void on_pb_remove_rp_clicked() noexcept;
  // auto connected
  void on_pb_add_bsl_clicked() noexcept;
  void on_pb_remove_bsl_clicked() noexcept;

  // manually connected
  // void when_basical_colorset_changed() noexcept;

  // for page 1 ------------------------------------------

  // auto connected
  void on_pb_select_all_clicked() noexcept;
  void on_pb_deselect_all_clicked() noexcept;
  void on_pb_deselect_non_reporducible_clicked() noexcept;
  void on_pb_deselect_rare_clicked() noexcept;
  void on_pb_invselect_classwise_clicked() noexcept;
  void on_pb_invselect_blockwise_clicked() noexcept;

  // auto connected
  void on_pb_custom_select_clicked() noexcept;

  void on_pb_load_preset_clicked() noexcept;
  void on_pb_save_preset_clicked() noexcept;

  // manually connected
  void setup_block_widgets() noexcept;

  // void when_allowed_colorset_changed() noexcept;
  //  for page 2 ------------------------------------------

  // auto connected
  void on_tb_add_images_clicked() noexcept;
  void on_tb_remove_images_clicked() noexcept;

  // auto connected
  void on_cb_show_raw_size_stateChanged(int state) noexcept;
  void on_cb_show_raw_stateChanged(int state) noexcept;
  void on_cb_show_converted_stateChanged(int state) noexcept;
  void on_lw_image_files_itemClicked(QListWidgetItem *item) noexcept;

  // manually connected
  void when_algo_dither_bottons_toggled() noexcept;

  // for page 3 ------------------------------------------
  // auto connected
  void on_pb_select_export_dir_clicked() noexcept;
  void on_pb_execute_clicked() noexcept;
  // for page 4 ------------------------------------------

  // auto connected
  void on_sb_threads_valueChanged(int val) noexcept;

  // auto connected
  void on_combobox_select_device_currentIndexChanged(int idx) noexcept;
};

#endif  // SLOPECRAFT_VISUALCRAFT_VCWIND_H
