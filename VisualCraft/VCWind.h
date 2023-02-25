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
  VCL_block *bsl{nullptr};

  void create_resource_pack() noexcept;

  static void append_default_to_rp_or_bsl(QListWidget *qlw,
                                          bool is_rp) noexcept;
  // static bool have_special(QListWidget *qlw) noexcept;

public:
  explicit VCWind(QWidget *parent = nullptr);
  ~VCWind();

public slots:
  void on_pb_add_rp_clicked() noexcept;
  void on_pb_remove_rp_clicked() noexcept;

  void on_pb_add_bsl_clicked() noexcept;
  void on_pb_remove_bsl_clicked() noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_VCWIND_H
