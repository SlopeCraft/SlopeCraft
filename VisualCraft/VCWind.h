#ifndef SLOPECRAFT_VISUALCRAFT_VCWIND_H
#define SLOPECRAFT_VISUALCRAFT_VCWIND_H

#include <QMainWindow>
#include <VisualCraftL.h>
#include <map>

class QListWidget;
class QListWidgetItem;
class VC_block_class;

namespace Ui {
class VCWind;
}

class VCWind : public QMainWindow {
  Q_OBJECT
private:
  // for all pages
  Ui::VCWind *ui;
  VCL_Kernel *const kernel{nullptr};
  // for page 0
  VCL_resource_pack *rp{nullptr};
  VCL_block_state_list *bsl{nullptr};

  // for page 1
  std::map<VCL_block_class_t, VC_block_class *> map_VC_block_class{};
  // for page 2
  std::map<QString, std::pair<QImage, QImage>> image_cache;
  // for page 3
  // for page 4
  QPoint prev_compute_device{-3, -3};

  // static bool have_special(QListWidget *qlw) noexcept;

public:
  explicit VCWind(QWidget *parent = nullptr);
  ~VCWind();

private:
  // for all pages
  static void callback_progress_range_set(void *, int, int, int) noexcept;
  static void callback_progress_range_add(void *, int) noexcept;

  // for page 0 ------------------------------------------
  //  create and set this->rp
  void create_resource_pack() noexcept;
  // create and set this->bsl
  void create_block_state_list() noexcept;

  // receive current selected version from ui
  SCL_gameVersion current_selected_version() const noexcept;
  VCL_face_t current_selected_face() const noexcept;

  static void append_default_to_rp_or_bsl(QListWidget *qlw,
                                          bool is_rp) noexcept;
  void setup_basical_colorset() noexcept;
  void setup_allowed_colorset() noexcept;
  // for page 1 ------------------------------------------
  size_t selected_blocks(std::vector<VCL_block *> *blocks_dest) const noexcept;
  void setup_image(const QImage &img) noexcept;
  // for page 2 ------------------------------------------
  SCL_convertAlgo current_selected_algo() const noexcept;
  // for page 3 ------------------------------------------
  // for page 4 ------------------------------------------
  void refresh_gpu_info() noexcept;
  void select_default_device() noexcept;
private slots:

  // for page 0

  // auto connected
  void on_pb_add_rp_clicked() noexcept;
  void on_pb_remove_rp_clicked() noexcept;
  // auto connected
  void on_pb_add_bsl_clicked() noexcept;
  void on_pb_remove_bsl_clicked() noexcept;

  // for page 1

  // manually connected
  void make_block_list_page() noexcept;
  // for page 2

  // auto connected
  void on_tb_add_images_clicked() noexcept;
  void on_tb_remove_images_clicked() noexcept;

  // auto connected
  void on_cb_show_raw_size_stateChanged(int state) noexcept;
  void on_cb_show_raw_stateChanged(int state) noexcept;
  void on_cb_show_converted_stateChanged(int state) noexcept;

  // for page 3
  void on_lw_image_files_itemClicked(QListWidgetItem *item) noexcept;
  // for page 4

  // auto connected
  void on_sb_threads_valueChanged(int val) noexcept;

  // auto connected
  void on_combobox_select_device_currentIndexChanged(int idx) noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_VCWIND_H
