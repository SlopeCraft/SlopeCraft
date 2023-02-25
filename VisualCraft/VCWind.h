#ifndef SLOPECRAFT_VISUALCRAFT_VCWIND_H
#define SLOPECRAFT_VISUALCRAFT_VCWIND_H

#include <QMainWindow>
#include <VisualCraftL.h>

class QListWidget;

namespace Ui {
class VCWind;
}

class VCWind : public QMainWindow {
  Q_OBJECT
private:
  Ui::VCWind *ui;
  VCL_Kernel *const kernel{nullptr};

  VCL_resource_pack *rp{nullptr};
  VCL_block_state_list *bsl{nullptr};

  // static bool have_special(QListWidget *qlw) noexcept;

public:
  explicit VCWind(QWidget *parent = nullptr);
  ~VCWind();

private:
  // create and set this->rp
  void create_resource_pack() noexcept;

  // create and set this->bsl
  void create_block_state_list() noexcept;

  // receive current selected version from ui
  SCL_gameVersion current_selected_version() const noexcept;
  VCL_face_t current_selected_face() const noexcept;

  static void append_default_to_rp_or_bsl(QListWidget *qlw,
                                          bool is_rp) noexcept;
private slots:
  void on_pb_add_rp_clicked() noexcept;
  void on_pb_remove_rp_clicked() noexcept;

  void on_pb_add_bsl_clicked() noexcept;
  void on_pb_remove_bsl_clicked() noexcept;

  void make_block_list_page() noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_VCWIND_H
