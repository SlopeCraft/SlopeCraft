#include "BlockListManager.h"
#include <string_view>

std::string_view basecolor_names[] = {"00 None",
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
                                      "61 GlowLichen"};

BlockListManager::BlockListManager(QWidget* parent) : QWidget(parent) {}

BlockListManager::~BlockListManager() {}

void BlockListManager::setup_basecolors(SlopeCraft::Kernel* kernel) noexcept {
  for (auto bcp : this->basecolors) {
    delete bcp;
  }
  this->basecolors.clear();
  this->basecolors.reserve(64);
  constexpr int num_basecolor =
      sizeof(basecolor_names) / sizeof(basecolor_names[0]);

  uint32_t bc_arr[64];

  kernel->getBaseColorInARGB32(bc_arr);

  for (int bc = 0; bc < num_basecolor; bc++) {
    BaseColorWidget* bcw = new BaseColorWidget(this, bc);
    this->layout()->addWidget(bcw);
    this->basecolors.push_back(bcw);
    bcw->setTitle(QString::fromUtf8(basecolor_names[bc].data()));

    bcw->set_color(bc_arr[bc]);
  }
}