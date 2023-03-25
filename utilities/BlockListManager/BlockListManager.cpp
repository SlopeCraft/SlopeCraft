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

#include "BlockListManager.h"
#include "TokiBaseColor.h"
#include "TokiBlock.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <iostream>
#include <SlopeCraftL.h>

BlockListManager::BlockListManager(QHBoxLayout *_area, QObject *parent)
    : QObject(parent) {
  isApplyingPreset = false;
  area = _area;
  qDebug() << ((area == nullptr) ? " 错误！_area 为空指针" : "");
  QGroupBox *qgb = nullptr;
  QGridLayout *qgl = nullptr;
  TokiBaseColor *tbc = nullptr;
  tbcs.clear();
  // qDebug("方块列表管理者开始创建 QGroupBox");
  for (uchar baseColor = 0; baseColor < 64; baseColor++) {
    if (baseColorNames[baseColor].isEmpty()) break;
    qgb = new QGroupBox(baseColorNames[baseColor]);
    // qDebug("create QGroupBox");
    qgl = new QGridLayout;
    qgb->setLayout(qgl);
    // qDebug("create Layout");
    tbc = new TokiBaseColor(baseColor, qgl);
    // qDebug("create TokiBaseColor");
    area->addWidget(qgb);
    // qDebug("add QGroupBox to layout");
    tbcs.push_back(tbc);

    connect(this, &BlockListManager::translate, tbc, &TokiBaseColor::translate);
    connect(tbc, &TokiBaseColor::userClicked, this,
            &BlockListManager::receiveClicked);
  }
  // qDebug("Manager 构造函数完毕");
}

BlockListManager::~BlockListManager() {
  for (uchar i = 0; i < tbcs.size(); i++) delete tbcs[i];
}

void BlockListManager::setVersion(uchar _ver) {
  if (_ver < 12 || _ver > SlopeCraft::SCL_maxAvailableVersion()) return;
  TokiBaseColor::mcVer = _ver;
  for (uchar i = 0; i < tbcs.size(); i++) tbcs[i]->versionCheck();
}

void BlockListManager::addBlocks(const QJsonArray &jArray, QString imgDir) {
  std::vector<std::queue<QJsonObject>> tasks;
  tasks.resize(tbcs.size());
  for (ushort i = 0; i < tasks.size(); i++)
    while (!tasks[i].empty()) tasks[i].pop();
  imgDir.replace("\\", "/");
  QJsonObject temp;
  uchar baseColor;
  qDebug() << "jArray.size()=" << jArray.size();
  for (unsigned int i = 0; i < jArray.size(); i++) {
    // DispLine
    temp = jArray[i].toObject();

    if (!isValidBlockInfo(temp)) {
      qDebug() << "出现不合法的 json 信息：" << temp;
      continue;
    }
    if (!temp.contains("version")) temp["version"] = 0;
    if (!temp.contains("idOld")) temp["idOld"] = temp["id"];
    if (!temp.contains("needGlass")) temp["needGlass"] = false;
    if (!temp.contains("isGlowing")) temp["isGlowing"] = false;
    if (!temp.contains("icon")) temp["icon"] = "";
    if (!temp.contains("endermanPickable")) temp["endermanPickable"] = false;
    if (!temp.contains("burnable")) temp["burnable"] = false;
    if (!temp.contains("wallUseable")) temp["wallUseable"] = true;

    baseColor = temp.value("baseColor").toInt();
    tasks[baseColor].push(temp);
  }
  // qDebug("已经将全部的 QJsonObject 装入多个队列，开始创建控件");
  for (ushort i = 0; i < tasks.size(); i++) {
    // qDebug()<<"基色"<<i<<"有"<<tasks[i].size()<<"个方块";
    while (!tasks[i].empty()) {
      tbcs[i]->addTokiBlock(tasks[i].front(), imgDir);
      tasks[i].pop();
    }
  }
}

void BlockListManager::applyPreset(const ushort *preset) {
  qDebug("applyPreset");
  isApplyingPreset = true;
  for (ushort i = 0; i < tbcs.size(); i++) {
    tbcs[i]->setSelected(preset[i]);
    tbcs[i]->checkBox->setChecked(true);
  }
  isApplyingPreset = false;
  emit blockListChanged();
}

void BlockListManager::setSelected(uchar baseColor, ushort blockSeq) {
  isApplyingPreset = true;
  tbcs[baseColor]->setSelected(blockSeq);
  isApplyingPreset = false;
}

void BlockListManager::setEnabled(uchar baseColor, bool isEnable) {
  isApplyingPreset = true;
  tbcs[baseColor]->checkBox->setChecked(isEnable);
  isApplyingPreset = false;
}

void BlockListManager::receiveClicked() const {
  if (isApplyingPreset) return;
  emit switchToCustom();
  emit blockListChanged();
}

void BlockListManager::setLabelColors(const QRgb *colors) {
  for (uchar i = 0; i < tbcs.size(); i++) tbcs[i]->makeLabel(colors[i]);
}

void BlockListManager::getEnableList(bool *dest) const {
  for (uchar i = 0; i < tbcs.size(); i++) dest[i] = tbcs[i]->getEnabled();
}

void BlockListManager::getSimpleBlockList(
    const SlopeCraft::AbstractBlock **SBL) const {
  // qDebug("void BlockListManager::getSimpleBlockList(simpleBlock * SBL)
  // const");

  for (uchar i = 0; i < 64; i++) {
    SBL[i] = nullptr;
  }
  for (uchar i = 0; i < tbcs.size(); i++) {
    SBL[i] = (tbcs[i]->getTokiBlock()->getSimpleBlock());
  }
}
std::vector<const SlopeCraft::AbstractBlock *>
BlockListManager::getSimpleBlockList() const {
  std::vector<const SlopeCraft::AbstractBlock *> SBL(64);
  const SlopeCraft::AbstractBlock *p;
  for (uchar i = 0; i < tbcs.size(); i++) {
    p = tbcs[i]->getTokiBlock()->getSimpleBlock();
    SBL[i] = p;
  }
  return SBL;
}
std::vector<const TokiBlock *> BlockListManager::getTokiBlockList() const {
  std::vector<const TokiBlock *> TBL(64);
  for (uchar i = 0; i < 64; i++) {
    if (i < tbcs.size())
      TBL[i] = tbcs[i]->getTokiBlock();
    else
      TBL[i] = nullptr;
  }
  return TBL;
}

std::vector<const QRadioButton *> BlockListManager::getQRadioButtonList()
    const {
  std::vector<const QRadioButton *> TBL(64);
  for (uchar i = 0; i < 64; i++) {
    if (i < tbcs.size())
      TBL[i] = tbcs[i]->getTokiBlock()->getTarget();
    else
      TBL[i] = nullptr;
  }
  return TBL;
}

std::vector<ushort> BlockListManager::toPreset() const {
  std::vector<ushort> TBL(64);
  for (uchar i = 0; i < 64; i++) {
    if (i < tbcs.size())
      TBL[i] = tbcs[i]->getSelected();
    else
      TBL[i] = 0;
  }
  return TBL;
}

bool isValidBlockInfo(const QJsonObject &json) {
  return (json.contains("id") && json.contains("nameZH") &&
          json.contains("nameEN") && json.contains("baseColor"));
}

void BlockListManager::getTokiBaseColors(
    std::vector<const TokiBaseColor *> *dest) const {
  dest->clear();
  dest->reserve(tbcs.size());
  for (const auto it : tbcs) {
    dest->emplace_back(it);
  }
}

int BlockListManager::getBlockNum() const {
  int result = 0;
  for (auto it : tbcs) {
    result += it->tbs.size();
  }
  return result;
}

void BlockListManager::getBlockPtrs(const SlopeCraft::AbstractBlock **dest,
                                    uint8_t *baseColor) const {
  int idx = 0;
  for (auto it : tbcs) {
    for (auto jt : it->tbs) {
      baseColor[idx] = it->baseColor;
      dest[idx] = jt->getSimpleBlock();
      idx++;
    }
  }
  dest[idx] = nullptr;
}

bool BlockListManager::loadPreset(const blockListPreset &preset) {
  if (preset.values.size() != this->tbcs.size()) {
    QMessageBox::warning(dynamic_cast<QWidget *>(this->parent()),
                         tr("加载预设错误"),
                         tr("预设文件包含的基色数量 (%1) 与实际情况 (%2) 不符")
                             .arg(preset.values.size())
                             .arg(this->tbcs.size()));
    return false;
  }

  std::vector<const TokiBlock *> blocks_arr;

  for (size_t basecolor = 0; basecolor < this->tbcs.size(); basecolor++) {
    auto tbc = this->tbcs[basecolor];
    const auto &pre = preset.values[basecolor];

    this->setEnabled(basecolor, pre.first);

    // find block
    int matched_block_idx = -1;
    blocks_arr.clear();
    tbc->getTokiBlockList(blocks_arr);
    for (int bidx = 0; bidx < (int)blocks_arr.size(); bidx++) {
      const char *const blkid = blocks_arr[bidx]->getSimpleBlock()->getId();
      if (QString::fromUtf8(blkid) == pre.second) {
        matched_block_idx = bidx;
        break;
      }
    }

    if (matched_block_idx < 0) {
      QMessageBox::warning(
          dynamic_cast<QWidget *>(this->parent()), tr("加载预设错误"),
          tr("预设中为基色%1指定的方块 id 是\"%2\"，没有找到这个方块 id")
              .arg(basecolor)
              .arg(pre.second));
      return false;
    }

    this->setSelected(basecolor, matched_block_idx);
  }

  return true;
}

blockListPreset BlockListManager::currentPreset() const noexcept {
  blockListPreset ret;
  ret.values.resize(this->tbcs.size());
  for (size_t basecolor = 0; basecolor < this->tbcs.size(); basecolor++) {
    ret.values[basecolor].first = this->tbcs[basecolor]->getEnabled();
    ret.values[basecolor].second = QString::fromUtf8(
        this->tbcs[basecolor]->getTokiBlock()->getSimpleBlock()->getId());
  }
  return ret;
}

const QString BlockListManager::baseColorNames[64] = {"00 None",
                                                      "01 Grass",
                                                      "02 Sand",
                                                      "03 Wool",
                                                      "04 Fire",
                                                      "05 Ice",
                                                      "06 Metal",
                                                      "07 Plant",
                                                      "08 Snow",
                                                      "09 Clay",
                                                      "10 Dirt",
                                                      "11 Stone",
                                                      "12 Water",
                                                      "13 Wood",
                                                      "14 Quartz",
                                                      "15 ColorOrange",
                                                      "16 ColorMagenta",
                                                      "17 ColorLightBlue",
                                                      "18 ColorYellow",
                                                      "19 ColorLime",
                                                      "20 ColorPink",
                                                      "21 ColorGray",
                                                      "22 ColorLightGray",
                                                      "23 ColorCyan",
                                                      "24 ColorPurple",
                                                      "25 ColorBlue",
                                                      "26 ColorBrown",
                                                      "27 ColorGreen",
                                                      "28 ColorRed",
                                                      "29 ColorBlack",
                                                      "30 Gold",
                                                      "31 Diamond",
                                                      "32 Lapis",
                                                      "33 Emerald",
                                                      "34 Podzol",
                                                      "35 Nether",
                                                      "36 TerracottaWhite",
                                                      "37 TerracottaOrange",
                                                      "38 TerracottaMagenta",
                                                      "39 TerracottaLightBlue",
                                                      "40 TerracottaYellow",
                                                      "41 TerracottaLime",
                                                      "42 TerracottaPink",
                                                      "43 TerracottaGray",
                                                      "44 TerracottaLightGray",
                                                      "45 TerracottaCyan",
                                                      "46 TerracottaPurple",
                                                      "47 TerracottaBlue",
                                                      "48 TerracottaBrown",
                                                      "49 TerracottaGreen",
                                                      "50 TerracottaRed",
                                                      "51 TerracottaBlack",
                                                      "52 CrimsonNylium",
                                                      "53 CrimsonStem",
                                                      "54 CrimsonHyphae",
                                                      "55 WarpedNylium",
                                                      "56 WarpedStem",
                                                      "57 WarpedHyphae",
                                                      "58 WarpedWartBlock",
                                                      "59 Deepslate",
                                                      "60 RawIron",
                                                      "61 GlowLichen",
                                                      "",
                                                      ""};
