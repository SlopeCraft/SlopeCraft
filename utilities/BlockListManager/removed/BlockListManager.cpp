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

bool callback_load_image(const char *filename, uint32_t *dst_row_major) {
  QImage img(QString::fromLocal8Bit(filename));

  if (img.isNull()) {
    return false;
  }

  QImage another = img.convertedTo(QImage::Format_ARGB32).scaled(16, 16);

  memcpy(dst_row_major, another.scanLine(0), 16 * 16 * sizeof(uint32_t));
  return true;
}

bool BlockListManager::setupFixedBlockList(const QString &filename,
                                           const QString &imgdir) noexcept {
  return this->impl_setupBlockList(filename, imgdir, this->BL_fixed);
}
bool BlockListManager::setupCustomBlockList(const QString &filename,
                                            const QString &imgdir) noexcept {
  return this->impl_setupBlockList(filename, imgdir, this->BL_custom);
}

bool BlockListManager::impl_setupBlockList(
    const QString &filename, const QString &dirname,
    std::unique_ptr<SlopeCraft::BlockListInterface, BlockListDeleter>
        &dst) noexcept {
  std::string errmsg;
  errmsg.resize(4096);
  SlopeCraft::blockListOption opt;
  opt.errmsg = errmsg.data();
  opt.errmsg_capacity = errmsg.size();
  size_t msg_len{0};
  opt.errmsg_len_dest = &msg_len;

  auto img_dir_local8bit = dirname.toLocal8Bit();
  opt.image_dir = img_dir_local8bit.data();

  opt.callback_load_image = callback_load_image;

  SlopeCraft::BlockListInterface *bli =
      SlopeCraft::SCL_createBlockList(filename.toLocal8Bit().data(), opt);

  errmsg.resize(msg_len);

  if (!errmsg.empty()) {
    if (bli == nullptr) {
      QMessageBox::critical(dynamic_cast<QWidget *>(this->parent()),
                            tr("解析方块列表失败"),
                            QString::fromUtf8(errmsg.data()));
      return false;
    } else {
      QMessageBox::warning(dynamic_cast<QWidget *>(this->parent()),
                           tr("解析方块列表失败"),
                           QString::fromUtf8(errmsg.data()));
    }
  }
  dst.reset(bli);

  std::vector<SlopeCraft::AbstractBlock *> blockps;
  std::vector<uint8_t> basecolors;
  basecolors.resize(dst->size());
  blockps.resize(dst->size());

  dst->get_blocks(blockps.data(), basecolors.data(), blockps.size());

  for (size_t idx = 0; idx < dst->size(); idx++) {
    this->tbcs[basecolors[idx]]->addTokiBlock(blockps[idx]);
  }
  return true;
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

  emit blockListChanged();

  return true;
}

bool BlockListManager::loadInternalPreset(
    const blockListPreset &preset) noexcept {
  return this->loadPreset(preset);
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
