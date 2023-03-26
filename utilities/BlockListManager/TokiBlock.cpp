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

#include "TokiBlock.h"

using namespace SlopeCraft;

TokiBlock::TokiBlock(QRadioButton *_target, SlopeCraft::AbstractBlock *blkp,
                     uint16_t _self, QObject *parent)
    : QObject(parent) {
  this->self = _self;
  this->target = _target;
  this->block = blkp;

  target->setText(nameZH());
  target->setChecked(true);

  connect(target, &QRadioButton::clicked, this, &TokiBlock::onTargetClicked);

  QImage img(16, 16, QImage::Format_ARGB32);

  this->block->getImage((uint32_t *)img.scanLine(0), true);

  this->target->setIcon(QIcon(QPixmap::fromImage(img)));
}

void TokiBlock::translate(Language lang) {
  switch (lang) {
    case Language::ZH:
      target->setText(nameZH());
      break;
    case Language::EN:
      target->setText(nameEN());
      break;
  }
  return;
}

TokiBlock::~TokiBlock() {}

void TokiBlock::onTargetClicked(bool isChecked) {
  if (isChecked) emit radioBtnClicked(self);
}

const QRadioButton *TokiBlock::getTarget() const { return target; }

const AbstractBlock *TokiBlock::getSimpleBlock() const { return block; }

QRadioButton *TokiBlock::getNCTarget() const { return target; }
