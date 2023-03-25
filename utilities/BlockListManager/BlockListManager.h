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

#ifndef BLOCKLISTMANAGER_H
#define BLOCKLISTMANAGER_H

#include <queue>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QObject>
#include <QRadioButton>

#include "../../SlopeCraftL/SlopeCraftL.h"

#include "TokiBaseColor.h"
#include "TokiBlock.h"

#define DispLine qDebug() << "File = " << __FILE__ << " , Line = " << __LINE__;

struct basecolorOption {
  uint8_t baseColor{0xFF};
  bool enabled{false};
  QString blockId{""};
};

struct blockListPreset {
  std::vector<std::pair<bool, QString>> values;
};

blockListPreset load_preset(QString filename, QString &err) noexcept;

QString serialize_preset(const blockListPreset &preset) noexcept;

class BlockListManager : public QObject {
  Q_OBJECT
 public:
  explicit BlockListManager(QHBoxLayout *_area, QObject *parent = nullptr);

  ~BlockListManager();

  void addBlocks(const QJsonArray &, QString imgDir);

  void applyPreset(const ushort *);

  void setSelected(uchar baseColor, ushort blockSeq);

  void setEnabled(uchar baseColor, bool isEnable);

  void setLabelColors(const QRgb *);

  void setVersion(uchar);

  void getEnableList(bool *) const;
  void getSimpleBlockList(const SlopeCraft::AbstractBlock **) const;
  std::vector<const SlopeCraft::AbstractBlock *> getSimpleBlockList() const;
  std::vector<const TokiBlock *> getTokiBlockList() const;
  std::vector<const QRadioButton *> getQRadioButtonList() const;
  std::vector<ushort> toPreset() const;

  void getTokiBaseColors(std::vector<const TokiBaseColor *> *) const;

  int getBlockNum() const;
  void getBlockPtrs(const SlopeCraft::AbstractBlock **, uint8_t *) const;

  bool loadPreset(const blockListPreset &preset);

  blockListPreset currentPreset() const noexcept;

 public slots:

 signals:
  void translate(Language);
  void switchToCustom() const;
  void blockListChanged() const;

 private:
  bool isApplyingPreset;
  QHBoxLayout *area;
  std::vector<TokiBaseColor *> tbcs;
  static const QString baseColorNames[64];

 private slots:
  void receiveClicked() const;
};

bool isValidBlockInfo(const QJsonObject &);

#endif  // BLOCKLISTMANAGER_H
