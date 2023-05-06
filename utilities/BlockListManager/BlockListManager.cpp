#include "BlockListManager.h"
#include <string_view>
#include <QMessageBox>

extern std::string_view basecolor_names[64];

BlockListManager::BlockListManager(QWidget *parent) : QWidget(parent) {}

BlockListManager::~BlockListManager() {}

void BlockListManager::setup_basecolors(
    const SlopeCraft::Kernel *kernel) noexcept {
  for (auto bcp : this->basecolors) {
    delete bcp;
  }
  this->basecolors.clear();
  this->basecolors.reserve(64);
  const int max_basecolor = SlopeCraft::SCL_maxBaseColor();

  uint32_t bc_arr[64];

  kernel->getBaseColorInARGB32(bc_arr);

  for (int bc = 0; bc <= max_basecolor; bc++) {
    BaseColorWidget *bcw = new BaseColorWidget(this, bc);
    this->layout()->addWidget(bcw);
    this->basecolors.push_back(bcw);
    bcw->setTitle(QString::fromUtf8(basecolor_names[bc].data()));

    bcw->set_color(bc_arr[bc]);
  }
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

bool BlockListManager::impl_addblocklist(
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
    this->basecolors[basecolors[idx]]->add_block(blockps[idx]);
  }

  return true;
}

bool BlockListManager::add_blocklist(QString filename,
                                     QString dirname) noexcept {
  std::unique_ptr<SlopeCraft::BlockListInterface, BlockListDeleter> tmp;

  if (!this->impl_addblocklist(filename, dirname, tmp)) {
    return false;
  }

  this->blockslists.emplace_back(std::move(tmp));

  return true;
}

void BlockListManager::finish_blocklist() noexcept {
  for (auto bcw : this->basecolors) {
    bcw->finish_blocks();
  }
}

void BlockListManager::when_version_updated() noexcept {
  for (auto bcw : this->basecolors) {
    bcw->when_version_updated(this->callback_get_version());
  }
}

std::string_view basecolor_names[64] = {"00 None",
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
                                        "Unknown",
                                        "Unknown"};