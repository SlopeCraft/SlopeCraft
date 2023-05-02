#ifndef SLOPECRAFT_SLOPECRAFT_POOLWIDGET_H
#define SLOPECRAFT_SLOPECRAFT_POOLWIDGET_H

#include <QWidget>
#include "cvt_task.h"

class PoolWidget;

namespace Ui {
class PoolWidget;
}

class PoolWidget : public QWidget {
  Q_OBJECT
 private:
  Ui::PoolWidget* ui;

 public:
  explicit PoolWidget(QWidget* parent = nullptr);
  ~PoolWidget();

  void set_task(const cvt_task& task) noexcept;
};

#endif  // SLOPECRAFT_SLOPECRAFT_POOLWIDGET_H