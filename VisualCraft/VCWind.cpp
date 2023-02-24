#include "VCWind.h"
#include "ui_VCWind.h"

VCWind::VCWind(QWidget *parent) : QMainWindow(parent), ui(new Ui::VCWind) {
  ui->setupUi(this);
}

VCWind::~VCWind() { delete ui; }
