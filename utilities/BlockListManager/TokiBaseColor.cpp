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

#include "TokiBaseColor.h"

uchar TokiBaseColor::mcVer = 17;

TokiBaseColor::TokiBaseColor(uchar _baseColor, QGridLayout *_layout,
                             QObject *parent)
    : QObject(parent) {
  layout = _layout;
  baseColor = _baseColor;
  // 负责创建 QCheckBox 和对应的弹簧

  layout->addWidget(checkBox = new QCheckBox("启用"), 0, 1);
  checkBox->setChecked(false);
  checkBox->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Expanding,
                                      QSizePolicy::Policy::Preferred));
  checkBox->setEnabled(baseColor != 0);

  connect(checkBox, &QCheckBox::toggled, this, &TokiBaseColor::updateEnabled);
  connect(checkBox, &QCheckBox::toggled, this, &TokiBaseColor::userClicked);
  connect(this, &TokiBaseColor::translate, this,
          &TokiBaseColor::translateCheckBox);
  // 创建弹簧
  QSpacerItem *si = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding,
                                    QSizePolicy::Policy::Preferred);
  layout->addItem(si, 0, 2);

  isEnabled = true;
  tbs.clear();
  selected = 65535;
  versionCheck();
}

TokiBaseColor::~TokiBaseColor() {
  for (ushort i = 0; i < tbs.size(); i++) delete tbs[i];
}

void TokiBaseColor::addTokiBlock(SlopeCraft::AbstractBlock *blkp) noexcept {
  QRadioButton *qrb = new QRadioButton;
  int rows = 1 + tbs.size() / 2;
  int cols = 1 + tbs.size() % 2;
  layout->addWidget(qrb, rows, cols);
#warning this may be replaced with nullptr
  TokiBlock *tb = new TokiBlock(qrb, blkp, tbs.size(), this);
  tbs.push_back(tb);

  if (tb->getSimpleBlock()->getVersion() > mcVer) {
    tb->getNCTarget()->setEnabled(false);
    tb->getNCTarget()->setChecked(false);
  }

  connect(tb, &TokiBlock::radioBtnClicked, this,
          &TokiBaseColor::receiveClicked);
  connect(this, &TokiBaseColor::translate, tb, &TokiBlock::translate);
  connect(tb, &TokiBlock::radioBtnClicked, this, &TokiBaseColor::userClicked);
}

void TokiBaseColor::makeLabel(QRgb color) {
  QLabel *qL = new QLabel("");
  layout->addWidget(qL, 0, 0, ceil(1 + tbs.size() / 2.0), 1);
  QPalette pl;
  pl.setColor(QPalette::ColorRole::Text, Qt::black);
  pl.setColor(
      QPalette::ColorRole::Window,
      QColor(qRed(color), qGreen(color), qBlue(color), 255 * bool(baseColor)));

  qL->setPalette(pl);
  qL->setSizePolicy(QSizePolicy::Policy::Preferred,
                    QSizePolicy::Policy::Preferred);
  qL->setMinimumWidth(30);
  qL->setFrameShape(QFrame::Shape::StyledPanel);
  qL->setFrameShadow(QFrame::Shadow::Plain);
  qL->setLineWidth(1);
  qL->setAutoFillBackground(true);
}

void TokiBaseColor::receiveClicked(ushort _selected) {
  selected = _selected % tbs.size();
  versionCheck();
}

bool TokiBaseColor::isAllOverVersion() const {  // 判断是否所有方块都超版本了
  bool isAllOver = true;
  for (auto it = tbs.cbegin(); it != tbs.cend(); it++) {
    isAllOver &= ((*it)->block->getVersion() > mcVer);
  }
  return isAllOver;
}

void TokiBaseColor::versionCheck() {
  checkBox->setEnabled(baseColor != 0 && !isAllOverVersion());
  if (!checkBox->isEnabled()) {
    checkBox->setChecked(false || baseColor == 0);
  }
  isEnabled = checkBox->isChecked();

  if (tbs.size() <= 0) {
    selected = 65535;
    return;
  }
  if (tbs.size() == 1) {
    selected = 0;
    tbs[0]->getNCTarget()->setChecked(true);
    tbs[0]->getNCTarget()->setEnabled(false);
    return;
  }
  std::vector<short> scores(tbs.size());
  for (ushort idx = 0; idx < tbs.size(); idx++) {
    if (tbs[idx]->getSimpleBlock()->getVersion() <= mcVer) {
      scores[idx] = (tbs[idx]->getTarget()->isChecked()) ? 100 : 51;
      tbs[idx]->getNCTarget()->setEnabled(true);
    } else {
      scores[idx] = (idx <= 0);
      tbs[idx]->getNCTarget()->setEnabled(false);
    }
  }
  ushort maxIndex = 0;
  for (ushort idx = 0; idx < scores.size(); idx++) {
    if (scores[idx] > scores[maxIndex]) maxIndex = idx;
  }
  selected = maxIndex;
  if (!tbs[selected]->getTarget()->isChecked()) {
    tbs[selected]->getNCTarget()->setChecked(true);
  }
}

void TokiBaseColor::setSelected(ushort sel) {
  tbs[sel]->getNCTarget()->setChecked(true);
  versionCheck();
}

void TokiBaseColor::updateEnabled(bool isChecked) {
  isEnabled = isChecked;
  versionCheck();
}

void TokiBaseColor::translateCheckBox(Language lang) {
  switch (lang) {
    case Language::ZH:
      checkBox->setText("启用");
      break;
    case Language::EN:
      checkBox->setText("Enable");
      break;
  }
}

const TokiBlock *TokiBaseColor::getTokiBlock() const {
  // qDebug("getTokiBlock");
  // qDebug()<<selected;
  return tbs[selected];
}

bool TokiBaseColor::getEnabled() const { return isEnabled; }

ushort TokiBaseColor::getSelected() const { return selected; }

void TokiBaseColor::getTokiBlockList(
    std::vector<const TokiBlock *> &dest) const {
  dest.clear();
  dest.reserve(tbs.size());
  for (const auto it : tbs) {
    dest.emplace_back(it);
  }
}
