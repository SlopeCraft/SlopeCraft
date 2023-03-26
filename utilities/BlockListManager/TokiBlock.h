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

#ifndef TOKIBLOCK_H
#define TOKIBLOCK_H

#include <QFile>
#include <QJsonObject>
#include <QMessageBox>
#include <QObject>
#include <QRadioButton>
#include <QString>

#include "../../SlopeCraftL/SlopeCraftL.h"

class TokiBaseColor;

enum class Language { ZH, EN };

class TokiBlock : public QObject {
  Q_OBJECT
  friend class TokiBaseColor;

 public:
  explicit TokiBlock(QRadioButton *_target, SlopeCraft::AbstractBlock *blkp,
                     uint16_t _self, QObject *parent);

  ~TokiBlock();
  const QRadioButton *getTarget() const;
  const SlopeCraft::AbstractBlock *getSimpleBlock() const;

  QString nameZH() const noexcept {
    return QString::fromUtf8(this->block->getNameZH());
  }
  QString nameEN() const noexcept {
    return QString::fromUtf8(this->block->getNameEN());
  }

 signals:
  void radioBtnClicked(ushort);

 private:
  ushort self;  // 指明自己是所属基色的第 i 个方块
  QRadioButton *target;
  SlopeCraft::AbstractBlock *block;
  QRadioButton *getNCTarget() const;
 private slots:
  void translate(Language);
  void onTargetClicked(bool);
};

#endif  // TOKIBLOCK_H
