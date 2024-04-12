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

#include "TokiSlopeCraft.h"

#ifdef RGB
#undef RGB
#endif

// std::unordered_set<TokiSlopeCraft *> TokiSlopeCraft::kernel_hash_set;
//
// std::mutex SCL_internal_lock;

TokiSlopeCraft::TokiSlopeCraft()
    : colorset{},
      image_cvter{*color_set::basic, colorset.allowed_colorset},
      glassBuilder{new PrimGlassBuilder},
      Compressor{new LossyCompressor} {
  kernelStep = step::nothing;
  this->image_cvter.clear_images();
  this->image_cvter.clear_color_hash();
  // rawImage.setZero(0, 0);
  // GAConverter = new GACvter::GAConverter;
  this->progressRangeSet = [](void *, int, int, int) {};
  this->progressAdd = [](void *, int) {};
  this->keepAwake = [](void *) {};
  this->reportError = [](void *, errorFlag, const char *) {};
  this->reportWorkingStatue = [](void *, workStatues) {};
  this->algoProgressAdd = [](void *, int) {};
  this->algoProgressRangeSet = [](void *, int, int, int) {};

  glassBuilder->windPtr = &wind;
  glassBuilder->progressAddPtr = &this->algoProgressAdd;
  glassBuilder->progressRangeSetPtr = &this->algoProgressRangeSet;
  glassBuilder->keepAwakePtr = &this->keepAwake;

  Compressor->windPtr = &wind;
  Compressor->progressAddPtr = &this->algoProgressAdd;
  Compressor->progressRangeSetPtr = &this->algoProgressRangeSet;
  Compressor->keepAwakePtr = &this->keepAwake;
}

TokiSlopeCraft::~TokiSlopeCraft() {}

/// function ptr to window object
void TokiSlopeCraft::setWindPtr(void *_w) {
  wind = _w;
  this->image_cvter.ui._uiPtr = _w;
  // GAConverter->setUiPtr(_w);
}
/// a function ptr to show progress of converting and exporting
void TokiSlopeCraft::setProgressRangeSet(void (*prs)(void *, int, int, int)) {
  progressRangeSet = prs;
  this->image_cvter.ui.progressRangeSet = prs;
  // GAConverter->setProgressRangeFun(prs);
}
/// a function ptr to add progress value
void TokiSlopeCraft::setProgressAdd(void (*pa)(void *, int)) {
  progressAdd = pa;
  this->image_cvter.ui.progressAdd = pa;
  // GAConverter->setProgressAddFun(pa);
}
/// a function ptr to prevent window from being syncoped
void TokiSlopeCraft::setKeepAwake(void (*ka)(void *)) { keepAwake = ka; }

/// a function ptr to show progress of compressing and bridge-building
void TokiSlopeCraft::setAlgoProgressRangeSet(void (*aprs)(void *, int, int,
                                                          int)) {
  algoProgressRangeSet = aprs;
}
/// a function ptr to add progress value of compressing and bridge-building
void TokiSlopeCraft::setAlgoProgressAdd(void (*apa)(void *, int)) {
  algoProgressAdd = apa;
}

/// a function ptr to report error when something wrong happens
void TokiSlopeCraft::setReportError(void (*re)(void *, errorFlag,
                                               const char *)) {
  reportError = re;
}
/// a function ptr to report working statue especially when busy
void TokiSlopeCraft::setReportWorkingStatue(void (*rws)(void *, workStatues)) {
  reportWorkingStatue = rws;
}

void TokiSlopeCraft::decreaseStep(step _step) {
  if (kernelStep <= _step) return;
  kernelStep = _step;
}

ColorMapPtrs TokiSlopeCraft::getAllowedColorMapPtrs() const noexcept {
  const auto &allowed = this->colorset.allowed_colorset;
  return {allowed.rgb_data(0), allowed.rgb_data(1), allowed.rgb_data(2),
          allowed.map_data(), allowed.color_count()};
}

void TokiSlopeCraft::trySkipStep(step s) {
  if (this->kernelStep >= s) {
    return;
  }

  if (this->colorset.allowed_colorset.color_count() != 0 &&
      this->colorset.palette.size() != 0) {
    this->kernelStep = step::wait4Image;
  }
}
/*
bool compressFile(const char *inputPath, const char *outputPath) {
  const size_t BUFFER_SIZE = 4096;
  std::ifstream fin;
  fin.open(inputPath, std::ifstream::binary | std::ifstream::in);
  if (!fin)
    return false;
  std::vector<char> buffer(BUFFER_SIZE, 0);
  gzFile fout = gzopen(outputPath, "wb");
  while (!fin.eof()) {
    fin.read(buffer.data(), buffer.size());
    std::streamsize s = fin.gcount();
    gzwrite(fout, buffer.data(), s);
  }
  gzclose(fout);
  return true;
}

*/

step TokiSlopeCraft::queryStep() const { return kernelStep; }

void TokiSlopeCraft::setAiCvterOpt(const AiCvterOpt *_a) { AiOpt = *_a; }

const AiCvterOpt *TokiSlopeCraft::aiCvterOpt() const { return &AiOpt; }

bool TokiSlopeCraft::setType(mapTypes type, gameVersion ver,
                             const bool *allowedBaseColor,
                             const AbstractBlock *const *const palettes) {
  /*
  if (kernelStep < colorSetReady)
  {
      reportError(wind, errorFlag::HASTY_MANIPULATION,
                  "You must load colorset before you set the map type.");
      return false;
  }
  */

  this->colorset.map_type = type;
  this->colorset.mc_version = ver;

  this->colorset.allowed_colorset.need_find_side =
      (this->colorset.map_type == mapTypes::Slope);

  this->colorset.palette.resize(64);

  // cerr<<__FILE__<<__LINE__<<endl;
  for (short i = 0; i < 64; i++) {
    // cerr<<"Block_"<<i<<endl;
    if (palettes[i] == nullptr) {
      this->colorset.palette[i].clear();
      continue;
    }
    palettes[i]->copyTo(&this->colorset.palette[i]);

    if (this->colorset.palette[i].id.find(':') ==
        this->colorset.palette[i].id.npos) {
      this->colorset.palette[i].id =
          "minecraft:" + this->colorset.palette[i].id;
    }

    if (this->colorset.palette[i].idOld.empty()) {
      this->colorset.palette[i].idOld = this->colorset.palette[i].id;
    }

    if (this->colorset.palette[i].idOld.size() > 0 &&
        (this->colorset.palette[i].idOld.find(':') ==
         this->colorset.palette[i].idOld.npos)) {
      this->colorset.palette[i].idOld =
          "minecraft:" + this->colorset.palette[i].idOld;
    }
  }

  // cerr<<__FILE__<<__LINE__<<endl;

  this->reportWorkingStatue(this->wind, workStatues::collectingColors);

  Eigen::ArrayXi baseColorVer(64);  // 基色对应的版本
  baseColorVer.setConstant((int)SCL_gameVersion::FUTURE);
  baseColorVer.segment(0, 52).setConstant((int)SCL_gameVersion::ANCIENT);
  baseColorVer.segment(52, 7).setConstant((int)SCL_gameVersion::MC16);
  baseColorVer.segment(59, 3).setConstant((int)SCL_gameVersion::MC17);

  bool MIndex[256];

  for (short index = 0; index < 256; index++) {
    MIndex[index] = true;  // 默认可以使用这种颜色，逐次判断各个不可以使用的条件

    if (!allowedBaseColor[index2baseColor(
            index)]) {  // 在 allowedBaseColor 中被禁止
      MIndex[index] = false;
      continue;
    }
    if (index2baseColor(index) == 0) {  // 空气禁用
      MIndex[index] = false;
      continue;
    }
    if ((int)this->colorset.mc_version <
        baseColorVer(index2baseColor(index))) {  // 版本低于基色版本
      MIndex[index] = false;
      continue;
    }
    if (this->colorset.palette[index2baseColor(index)].id.empty()) {  // 空 id
      MIndex[index] = false;
      continue;
    }
    /*
    if ((mapType == mapTypes::Wall) &&
        !blockPalette[index2baseColor(index)]
             .wallUseable) { //墙面像素画且当前方块不适合墙面

      MIndex[index] = false;
      continue;
    }*/
    if (this->isVanilla() &&
        (index2depth(index) >= 3)) {  // 可实装的地图画不允许第四种阴影
      MIndex[index] = false;
      continue;
    }
    if (index2baseColor(index) == 12) {  // 如果是水且非墙面
      if (this->isFlat() && index2depth(index) != 2) {  // 平板且水深不是 1 格
        MIndex[index] = false;
        continue;
      }
    } else {
      if (this->isFlat() && index2depth(index) != 1) {  // 平板且阴影不为 1
        MIndex[index] = false;
        continue;
      }
    }
  }

  if (!this->colorset.allowed_colorset.apply_allowed(*color_set::basic,
                                                     MIndex)) {
    std::string msg =
        "Too few usable color(s) : only " +
        std::to_string(this->colorset.allowed_colorset.color_count()) +
        " colors\n";
    msg += "Avaliable base color(s) : ";

    for (int idx = 0; idx < this->colorset.allowed_colorset.color_count();
         idx++) {
      msg += std::to_string(this->colorset.allowed_colorset.Map(idx)) + ", ";
    }

    this->reportError(this->wind, errorFlag::USEABLE_COLOR_TOO_FEW, msg.data());
    return false;
  }

  // GACvter::updateMapColor2GrayLUT();

  this->reportWorkingStatue(this->wind, workStatues::none);
  this->image_cvter.on_color_set_changed();

  this->kernelStep = SCL_step::wait4Image;

  return true;
}

uint16_t TokiSlopeCraft::getColorCount() const {
  if (kernelStep < SCL_step::wait4Image) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only query for avaliable color count after you set "
                "the map type and gameversion");
    return 0;
  }
  return this->colorset.allowed_colorset.color_count();
}

void TokiSlopeCraft::getAvailableColors(ARGB *const ARGBDest,
                                        uint8_t *const mapColorDest,
                                        int *const num) const {
  if (num != nullptr) {
    *num = getColorCount();
  }

  for (int idx = 0; idx < this->colorset.allowed_colorset.color_count();
       idx++) {
    if (mapColorDest != nullptr) {
      mapColorDest[idx] = this->colorset.allowed_colorset.Map(idx);
    }

    if (ARGBDest != nullptr) {
      ARGB r, g, b, a;
      if (mapColor2baseColor(this->colorset.allowed_colorset.Map(idx)) != 0)
        a = 255;
      else
        a = 0;

      r = ARGB(this->colorset.allowed_colorset.RGB(idx, 0) * 255);
      g = ARGB(this->colorset.allowed_colorset.RGB(idx, 1) * 255);
      b = ARGB(this->colorset.allowed_colorset.RGB(idx, 2) * 255);

      ARGBDest[idx] = (a << 24) | (r << 16) | (g << 8) | (b);
    }
  }
}

void TokiSlopeCraft::setRawImage(const ARGB *src, int rows, int cols,
                                 bool is_col_major) {
  if (kernelStep < SCL_step::wait4Image) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only import the raw image count after you set the map "
                "type and gameversion");
    return;
  }
  if (rows <= 0 || cols <= 0) {
    reportError(wind, errorFlag::EMPTY_RAW_IMAGE,
                "The size of your raw image is 0. You loaded an empty image.");
    return;
  }

  this->image_cvter.set_raw_image(src, rows, cols, is_col_major);

  kernelStep = SCL_step::convertionReady;

  return;
}

void TokiSlopeCraft::getBaseColorInARGB32(ARGB *const dest) const {
  if (dest == nullptr) return;

  for (uint8_t base = 0; base < 64; base++)
    dest[base] = ARGB32(255 * color_set::basic->RGB(128 + base, 0),
                        255 * color_set::basic->RGB(128 + base, 1),
                        255 * color_set::basic->RGB(128 + base, 2), 255);
}

int64_t TokiSlopeCraft::sizePic(short dim) const {
  if (dim == 0) return this->image_cvter.rows();
  if (dim == 1) return this->image_cvter.cols();
  return this->image_cvter.size();
}

TokiSlopeCraft::ColorSpace TokiSlopeCraft::getColorSpace() const {
  switch (this->image_cvter.convert_algo()) {
    case SCL_convertAlgo::RGB:
      return R;
    case SCL_convertAlgo::RGB_Better:
      return R;
    case SCL_convertAlgo::HSV:
      return H;
    case SCL_convertAlgo::Lab94:
      return L;
    case convertAlgo::Lab00:
      return L;
    case SCL_convertAlgo::XYZ:
      return X;
    case convertAlgo::gaCvter:
      return R;
  }
  return R;
}

void TokiSlopeCraft::getConvertedImage(int *rows, int *cols, ARGB *dest,
                                       bool expected_col_major) const {
  EImage result = this->getConovertedImage();
  if (rows != nullptr) *rows = result.rows();
  if (cols != nullptr) *cols = result.cols();
  if (!expected_col_major) {
    result.transposeInPlace();
  }
  if (dest != nullptr) {
    memcpy(dest, result.data(), sizeof(ARGB) * sizePic(2));
  }
}

EImage TokiSlopeCraft::getConovertedImage() const {
  EImage cvtedImg(sizePic(0), sizePic(1));
  cvtedImg.setZero();
  if (kernelStep < SCL_step::converted) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can get the converted image only after you converted a map.");
    return cvtedImg;
  }

  const auto argbLUT = this->LUT_mapcolor_to_argb();

  for (short r = 0; r < sizePic(0); r++) {
    for (short c = 0; c < sizePic(1); c++) {
      const auto map_color = this->image_cvter.color_id(r, c);
      if (mapColor2baseColor(map_color) == 0) {  //  if base ==0
        cvtedImg(r, c) = ARGB32(0, 0, 0, 0);
        continue;
      }
      const int Index = mapColor2Index(map_color);

      cvtedImg(r, c) = argbLUT[Index];
    }
  }
  return cvtedImg;
}

std::array<uint32_t, 256> TokiSlopeCraft::LUT_mapcolor_to_argb()
    const noexcept {
  std::array<uint32_t, 256> argbLUT;
  for (int idx = 0; idx < 256; idx++) {
    argbLUT[idx] =
        RGB2ARGB(color_set::basic->RGB(idx, 0), color_set::basic->RGB(idx, 1),
                 color_set::basic->RGB(idx, 2));
  }
  return argbLUT;
}

void TokiSlopeCraft::getConvertedMap(int *rows, int *cols,
                                     unsigned char *dst) const {
  if (rows != nullptr) {
    *rows = this->image_cvter.rows();
  }
  if (cols != nullptr) {
    *cols = this->image_cvter.cols();
  }

  Eigen::Map<Eigen::Array<uint8_t, Eigen::Dynamic, Eigen::Dynamic>> dest(
      dst, getImageRows(), getImageCols());

  dest = this->image_cvter.color_id();
}

int TokiSlopeCraft::getImageRows() const {
  if (kernelStep < SCL_step::convertionReady) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can call getImageRows only after you imported the raw image.");
    return -1;
  }
  return this->image_cvter.rows();
}

int TokiSlopeCraft::getImageCols() const {
  if (kernelStep < SCL_step::convertionReady) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can call getImageRows only after you imported the raw image.");
    return -1;
  }
  return this->image_cvter.cols();
}

void TokiSlopeCraft::get3DSize(int *x, int *y, int *z) const {
  if (kernelStep < SCL_step::builded) return;
  if (x != nullptr) *x = schem.x_range();
  if (y != nullptr) *y = schem.y_range();
  if (z != nullptr) *z = schem.z_range();
  return;
}

int TokiSlopeCraft::getHeight() const {
  if (kernelStep < SCL_step::builded) return -1;
  return schem.y_range();
}

void TokiSlopeCraft::getBlockCounts(int *total, int detail[64]) const {
  std::vector<int64_t> temp;
  if (total != nullptr) *total = getBlockCounts(&temp);
  if (detail != nullptr)
    for (uint16_t idx = 0; idx < temp.size(); idx++) {
      detail[idx] = temp[idx];
    }
}

int64_t TokiSlopeCraft::getBlockCounts(std::vector<int64_t> *dest) const {
  if (kernelStep < SCL_step::builded) return -1;

  const bool is_dest_nonnull = (dest != nullptr);
  if (is_dest_nonnull) {
    dest->resize(64);
    for (int i = 0; i < 64; i++) dest->at(i) = 0;
  }

  int64_t total = 0;

  // map ele_t in schem to index in blockPalette(material index)
  std::vector<int> map_ele_to_material;
  {
    map_ele_to_material.resize(this->colorset.palette.size());
    std::fill(map_ele_to_material.begin(), map_ele_to_material.end(), -1);

    for (int ele = 1; ele < this->schem.palette_size(); ele++) {
      std::string_view blkid = this->schem.palette()[ele];

      const simpleBlock *sbp =
          TokiSlopeCraft::find_block_for_idx(ele - 1, blkid);
      assert(sbp != nullptr);

      int64_t idx = sbp - this->colorset.palette.data();
      assert(idx >= 0);
      assert(idx < (int64_t)this->colorset.palette.size());

      map_ele_to_material[ele] = idx;
    }
  }

  for (int i = 0; i < schem.size(); i++) {
    if (schem(i) == 0) {
      continue;
    }

    total++;
    if (is_dest_nonnull) {
      const int mat_idx = map_ele_to_material[schem(i)];
      assert(mat_idx >= 0);
      dest->at(mat_idx)++;
    }
  }
  return total;
}

int64_t TokiSlopeCraft::getBlockCounts() const {
  if (kernelStep < SCL_step::builded) return -1;
  int totalCount = 0;
  for (int i = 0; i < schem.size(); i++) {
    if (schem(i)) totalCount++;
  }
  return totalCount;
}

const uint16_t *TokiSlopeCraft::getBuild(int *xSize, int *ySize,
                                         int *zSize) const {
  if (xSize != nullptr) *xSize = getXRange();
  if (ySize != nullptr) *ySize = getHeight();
  if (zSize != nullptr) *zSize = getZRange();
  return schem.data();
}

int TokiSlopeCraft::getXRange() const {
  if (kernelStep < SCL_step::builded) return -1;
  return schem.x_range();
}
int TokiSlopeCraft::getZRange() const {
  if (kernelStep < SCL_step::builded) return -1;
  return schem.x_range();
}
