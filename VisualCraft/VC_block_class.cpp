/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "VC_block_class.h"
#include "VCWind.h"
#include "ui_VC_block_class.h"
#include <QCheckBox>
#include <QLabel>
#include <iostream>

VC_block_class::VC_block_class(QWidget *parent)
    : QGroupBox(parent), ui(new Ui::VC_block_class) {
  this->ui->setupUi(this);
}

VC_block_class::~VC_block_class() { delete this->ui; }

void VC_block_class::erase_blocks() noexcept {
  for (auto &pair : this->blocks) {
    this->ui->grid_layout->removeWidget(pair.second);
    delete pair.second;
  }

  this->blocks.clear();
}

QCheckBox *VC_block_class::chbox_enabled() noexcept {
  return this->ui->cb_enable;
}

void VC_block_class::set_blocks(size_t num_blocks,
                                VCL_block *const *const blocks,
                                size_t cols) noexcept {
  this->erase_blocks();

  this->blocks.reserve(num_blocks);

  for (size_t idx = 0; idx < num_blocks; idx++) {
    VCL_block *const blkp = blocks[idx];
    QCheckBox *const cb = new QCheckBox;

    cb->setChecked(VCL_is_block_enabled(blkp));
    // #warning set name here
    cb->setText(QString::fromUtf8(VCL_get_block_name(blkp, ::is_language_ZH)));

    const size_t colidx = idx % cols;
    const size_t rowidx = idx / cols + 1;

    this->ui->grid_layout->addWidget(cb);

    this->ui->grid_layout->addWidget(cb, rowidx, colidx);
    this->blocks.emplace_back(std::pair<VCL_block *, QCheckBox *>{blkp, cb});

    cb->show();
  }

  if (num_blocks <= 1) {
    return;
  }

  // add place holders so that there will always be `cols` cols
  for (size_t c = num_blocks; c < cols; c++) {
    QLabel *lb = new QLabel;
    lb->setText("");
    this->ui->grid_layout->addWidget(lb, 1, c);
    lb->show();
  }
}

void VC_block_class::set_state_for_all(bool checked) noexcept {
  for (auto &pair : this->blocks) {
    pair.second->setChecked(checked);
  }
}

size_t VC_block_class::selected_blocks(std::vector<VCL_block *> *select_blks,
                                       bool append) const noexcept {
  if (!append && select_blks != nullptr) {
    select_blks->clear();
  }

  if (!this->ui->cb_enable->isChecked()) {
    return 0;
  }

  size_t counter = 0;

  for (const auto &pair : this->blocks) {
    if (!pair.second->isChecked()) {
      continue;
    }
    counter++;
    if (select_blks != nullptr) {
      select_blks->emplace_back(pair.first);
    }
  }

  return counter;
}
