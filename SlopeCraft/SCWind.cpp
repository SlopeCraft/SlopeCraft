#include "SCWind.h"
#include "ui_SCWind.h"

SCWind::SCWind(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::SCWind),
      kernel(SlopeCraft::SCL_createKernel()) {
  this->ui->setupUi(this);
}

SCWind::~SCWind() {
  delete this->ui;
  SlopeCraft::SCL_destroyKernel(this->kernel);
}