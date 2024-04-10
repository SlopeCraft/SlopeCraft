//
// Created by Joseph on 2024/4/9.
//

#include "BlockListDialog.h"
#include "ui_BlockListDialog.h"

BlockListDialog::BlockListDialog(QWidget *parent)
    : QDialog{parent}, ui{new Ui::BlockListDialog} {
  this->ui->setupUi(this);
}

BlockListDialog::~BlockListDialog() {
  // delete this->ui;
}
