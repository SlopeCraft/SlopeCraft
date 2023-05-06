#ifndef SLOPECRAFT_SLOPECRAFT_SCWIND_H
#define SLOPECRAFT_SLOPECRAFT_SCWIND_H

#include <QMainWindow>
#include <SlopeCraftL.h>
#include <memory>
#include <QRadioButton>
#include <vector>

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

  void when_cvt_pool_selectionChanged() noexcept;

  void when_version_buttons_toggled() noexcept;

 private:
  Ui::SCWind* ui;
  SlopeCraft::Kernel* kernel;

  task_pool_t tasks;
  CvtPoolModel* cvt_pool_model{nullptr};
  PoolModel* export_pool_model{nullptr};

  std::array<const QRadioButton*, 20 - 12 + 1> version_buttons() const noexcept;

  SCL_gameVersion selected_version() const noexcept;
};

#endif  // SLOPECRAFT_SLOPECRAFT_SCWIND_H