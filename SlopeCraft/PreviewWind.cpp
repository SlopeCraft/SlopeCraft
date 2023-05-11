#include "PreviewWind.h"
#include "ui_PreviewWind.h"

PreviewWind::PreviewWind(const SlopeCraft::Kernel* kernel, QWidget* parent)
    : QDialog(parent), ui(new Ui::PreviewWind) {
  this->ui->setupUi(this);
}

PreviewWind::~PreviewWind() { delete this->ui; }