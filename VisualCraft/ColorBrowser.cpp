#include "ColorBrowser.h"
#include "ui_ColorBrowser.h"

ColorBrowser::ColorBrowser(QWidget *parent)
    : QWidget(parent), ui(new Ui::ColorBrowser) {
  this->ui->setupUi(this);
}

ColorBrowser::~ColorBrowser() { delete this->ui; }