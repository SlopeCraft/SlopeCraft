#ifndef SLOPECRAFT_SLOPECRAFT_SCWIND_H
#define SLOPECRAFT_SLOPECRAFT_SCWIND_H

#include <QMainWindow>
#include <SlopeCraftL.h>
#include <memory>

class SCWind;

namespace Ui {
class SCWind;
}

class SCWind : public QMainWindow {
  Q_OBJECT
 public:
  explicit SCWind(QWidget* parent = nullptr);
  ~SCWind();

 private:
  Ui::SCWind* ui;
  SlopeCraft::Kernel* kernel;
};

#endif  // SLOPECRAFT_SLOPECRAFT_SCWIND_H