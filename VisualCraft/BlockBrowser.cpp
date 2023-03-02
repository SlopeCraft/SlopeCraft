#include "BlockBrowser.h"
#include "ui_BlockBrowser.h"

BlockBrowser::BlockBrowser(QWidget *parent)
    : QWidget(parent), ui(new Ui::BlockBrowser) {
  ui->setupUi(this);
}

BlockBrowser::~BlockBrowser() { delete this->ui; }