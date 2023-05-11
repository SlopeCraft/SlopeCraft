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

bool TokiSlopeCraft::makeTests(const AbstractBlock **src,
                               const unsigned char *baseColor, const char *dst,
                               char *_unFileName) {
  if (kernelStep < step::wait4Image) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "Skipping is not allowed.\nYou can only make tests only after "
                "you finished the map type and gameversion.");

    if (_unFileName != nullptr) *_unFileName = '\0';

    return false;
  }

  std::string s = makeTests(src, baseColor, std::string(dst));
  if (_unFileName != nullptr) std::strcpy(_unFileName, s.data());

  return s.empty();
}

std::string TokiSlopeCraft::makeTests(const AbstractBlock **src,
                                      const uint8_t *src_baseColor,
                                      const std::string &fileName) {
  if (!fileName.ends_with(".nbt")) {
    return "File name should ends with \".nbt\"";
  }

  libSchem::Schem test;
  test.set_MC_major_version_number(::TokiSlopeCraft::mcVer);
  test.set_MC_version_number(
      MCDataVersion::suggested_version(::TokiSlopeCraft::mcVer));
  // const simpleBlock ** realSrc=(const simpleBlock **)src;
  std::vector<const simpleBlock *> realSrc;
  std::vector<uint8_t> realBaseColor;
  realSrc.clear();
  realBaseColor.clear();
  for (uint32_t idx = 0; src[idx] != nullptr; idx++) {
    if (src[idx]->getVersion() > (int)mcVer) {
      continue;
    }
    realSrc.emplace_back(static_cast<const simpleBlock *>(src[idx]));
    realBaseColor.emplace_back(src_baseColor[idx]);
  }

  std::vector<std::vector<int>> blocks;
  blocks.resize(64);

  for (auto &it : blocks) {
    while (!it.empty()) {
      it.clear();
      // it.reserve(16);
    }
  }

  for (uint32_t idx = 0; idx < realSrc.size(); idx++) {
    blocks[realBaseColor[idx]].push_back(idx);
  }

  {
    std::vector<const char *> ids;
    ids.reserve(realSrc.size() + 1);
    ids.emplace_back("minecraft:air");
    for (auto i : realSrc) {
      if (TokiSlopeCraft::mcVer == SCL_gameVersion::MC12) {
        ids.emplace_back(i->getIdOld());
      } else {
        ids.emplace_back(i->getId());
      }
    }

    test.set_block_id(ids.data(), ids.size());
  }

  int xSize = 0;
  constexpr int zSize = 64, ySize = 1;
  for (const auto &it : blocks) {
    xSize = std::max(size_t(xSize), it.size());
  }
  test.resize(xSize, ySize, zSize);
  test.set_zero();

  for (uint8_t base = 0; base < 64; base++) {
    for (uint32_t idx = 0; idx < blocks[base].size(); idx++) {
      int xPos = idx;
      int yPos = 0;
      int zPos = base;

      test(xPos, yPos, zPos) = blocks[base][idx] + 1;
    }
  }

  SCL_errorFlag err;
  std::string detail;
  const bool success = test.export_structure(fileName, true, &err, &detail);

  if (!success) {
    return std::string("Failed to export structure file ") + fileName +
           ", error code = " + std::to_string(int(err)) + ", detail: " + detail;
  } else {
    return {};
  }
}

void TokiSlopeCraft::exportAsLitematic(const char *TargetName,
                                       const char *LiteName,
                                       const char *RegionName,
                                       char *FileName) const {
  std::string temp = exportAsLitematic(TargetName, LiteName, RegionName);

  if (FileName != nullptr) std::strcpy(temp.data(), FileName);
}

std::string TokiSlopeCraft::exportAsLitematic(
    const std::string &TargetName, const std::string &LiteName,
    const std::string &RegionName) const {
  if (kernelStep < SCL_step::builded) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only export a map to litematic after you build the 3D "
                "structure.");
    return "Too hasty! export litematic after you built!";
  }
  reportWorkingStatue(wind, workStatues::writingMetaInfo);
  progressRangeSet(wind, 0, 100 + schem.size(), 0);

  libSchem::litematic_info info;
  info.litename_utf8 = LiteName;
  info.regionname_utf8 = RegionName;

  errorFlag flag = errorFlag::NO_ERROR_OCCUR;
  std::string error_string;
  const bool success =
      schem.export_litematic(TargetName, info, &flag, &error_string);

  if (!success) {
    // this->reportError(this->wind,errorFlag::)
    this->reportError(this->wind, flag, error_string.data());

    return "Failed to export as litematic.\n" + error_string;
  }

  progressRangeSet(wind, 0, 100, 100);
  reportWorkingStatue(wind, workStatues::none);

  return "";
}

std::vector<std::string_view> TokiSlopeCraft::schem_block_id_list()
    const noexcept {
  std::vector<std::string_view> temp;
  temp.reserve(64);
  temp.emplace_back("minecraft:air");

  for (const auto &block : TokiSlopeCraft::blockPalette) {
    std::string_view id_at_curversion =
        block.idForVersion(TokiSlopeCraft::mcVer);
    if ("minecraft:air" == id_at_curversion) {
      break;
    } else {
      temp.emplace_back(id_at_curversion);
    }
  }
  return temp;
}

bool TokiSlopeCraft::build(const build_options &option) noexcept {
  /*
  bool TokiSlopeCraft::build(compressSettings cS, uint16_t mAH,
                             glassBridgeSettings gBS, uint16_t bI, bool
  fireProof, bool endermanProof){
                             */

  if (kernelStep < SCL_step::converted) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can build 3D strcuture only after you converted the raw image.");
    cerr << "hasty!" << endl;
    return false;
  }
  if (option.maxAllowedHeight < 14) {
    cerr << "maxAllowedHeight is less than 14!" << endl;
    reportError(wind, errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14,
                "Your maximum allowed height is less than 14, which made lossy "
                "compressing almost impossible.");
    return false;
  }
  // cerr << "Setting block palette..." << endl;
  {
    auto temp = this->schem_block_id_list();
    schem.set_block_id(temp);
  }

  schem.set_MC_major_version_number(TokiSlopeCraft::mcVer);
  schem.set_MC_version_number(MCDataVersion::MCDataVersion_t(
      TokiSlopeCraft::mcVersion2VersionNumber(TokiSlopeCraft::mcVer)));

  // cerr << "ready to build" << endl;

  this->build_opt.compressMethod = option.compressMethod;
  this->build_opt.glassMethod = option.glassMethod;
  if (isFlat() || !isVanilla()) {
    this->build_opt.compressMethod = compressSettings::noCompress;
    this->build_opt.glassMethod = glassBridgeSettings::noBridge;
  }

  this->build_opt.maxAllowedHeight = option.maxAllowedHeight;
  this->build_opt.bridgeInterval = option.bridgeInterval;
  this->build_opt.fire_proof = option.fire_proof;
  this->build_opt.enderman_proof = option.enderman_proof;

  reportWorkingStatue(wind, workStatues::buidingHeighMap);

  progressRangeSet(wind, 0, 9 * sizePic(2), 0);
  // cerr << "start makeHeight" << endl;
  {
    Eigen::ArrayXXi Base, HighMap, LowMap;
    std::unordered_map<TokiPos, waterItem> WaterList;
    progressAdd(wind, sizePic(2));

    makeHeight_new(Base, HighMap, LowMap, WaterList);
    // cerr << "makeHeight finished" << endl;
    progressRangeSet(wind, 0, 9 * sizePic(2), 5 * sizePic(2));

    reportWorkingStatue(wind, workStatues::building3D);
    // cerr << "start buildHeight" << endl;
    buildHeight(this->build_opt.fire_proof, this->build_opt.enderman_proof,
                Base, HighMap, LowMap, WaterList);
    // cerr << "buildHeight finished" << endl;
    progressRangeSet(wind, 0, 9 * sizePic(2), 8 * sizePic(2));

    reportWorkingStatue(wind, workStatues::constructingBridges);
    // cerr << "start makeBridge" << endl;
    makeBridge();
  }
  // cerr << "makeBridge finished" << endl;
  progressRangeSet(wind, 0, 9 * sizePic(2), 9 * sizePic(2));

  reportWorkingStatue(wind, workStatues::none);

  kernelStep = SCL_step::builded;

  return true;
}

void TokiSlopeCraft::makeHeight_new(
    Eigen::ArrayXXi &Base, Eigen::ArrayXXi &HighMap, Eigen::ArrayXXi &LowMap,
    std::unordered_map<TokiPos, waterItem> &WaterList) {
  Base.setZero(sizePic(0) + 1, sizePic(1));
  HighMap.setZero(sizePic(0) + 1, sizePic(1));
  LowMap.setZero(sizePic(0) + 1, sizePic(1));
  WaterList.clear();
  bool allowNaturalCompress =
      this->build_opt.compressMethod == compressSettings::Both ||
      this->build_opt.compressMethod == compressSettings::NaturalOnly;
  // std::vector<const TokiColor*> src;
  // cerr << "makeHeight_new\n";

  if ((this->mapPic - 4 * (this->mapPic / 4) >= 3).any()) {
    std::string msg =
        "Fatal error : SlopeCraftL found map color with depth 3 in a "
        "vanilla map.\n Map contents (map color matrix in col-major) :\n[";

    for (int c = 0; c < this->mapPic.cols(); c++) {
      for (int r = 0; r < this->mapPic.rows(); r++) {
        msg += std::to_string(this->mapPic(r, c)) + ',';
      }
      msg += ";\n";
    }
    msg += "];\n";

    reportError(wind, errorFlag::DEPTH_3_IN_VANILLA_MAP, msg.data());
    return;
  }

  for (uint16_t c = 0; c < sizePic(1); c++) {
    // cerr << "Coloumn " << c << '\n';
    HeightLine HL;
    // getTokiColorPtr(c,&src[0]);
    HL.make(this->mapPic.col(c), allowNaturalCompress);

    if (HL.maxHeight() > this->build_opt.maxAllowedHeight &&
        (this->build_opt.compressMethod == compressSettings::ForcedOnly ||
         this->build_opt.compressMethod == compressSettings::Both)) {
      std::vector<const TokiColor *> ptr(getImageRows());

      this->image_cvter.col_TokiColor_ptrs(c, ptr.data());
      // getTokiColorPtr(c, &ptr[0]);

      Compressor->setSource(HL.getBase(), &ptr[0]);
      bool success = Compressor->compress(this->build_opt.maxAllowedHeight,
                                          allowNaturalCompress);
      if (!success) {
        std::string msg = "Failed to compress the 3D structure at coloum " +
                          std::to_string(c);
        reportError(wind, SCL_errorFlag::LOSSYCOMPRESS_FAILED, msg.data());
        return;
      }
      Eigen::ArrayXi temp;
      HL.make(&ptr[0], Compressor->getResult(), allowNaturalCompress, &temp);
      this->mapPic.col(c) = temp;
    }

    Base.col(c) = HL.getBase();
    HighMap.col(c) = HL.getHighLine();
    LowMap.col(c) = HL.getLowLine();
    auto HLM = &HL.getWaterMap();

    for (auto it = HLM->cbegin(); it != HLM->cend(); it++) {
      WaterList[TokiRC(it->first, c)] = it->second;
    }

    progressAdd(wind, 4 * sizePic(0));
  }
  // cerr << "makeHeight_new finished\n";
  schem.resize(2 + sizePic(1), HighMap.maxCoeff() + 1, 2 + sizePic(0));
  schem.set_zero();
  // schem.z_range() = 2 + sizePic(0);         // z
  // schem.x_range() = 2 + sizePic(1);         // x
  // schem.y_range() = HighMap.maxCoeff() + 1; // y
}

void TokiSlopeCraft::buildHeight(
    bool fireProof, bool endermanProof, const Eigen::ArrayXXi &Base,
    const Eigen::ArrayXXi &, const Eigen::ArrayXXi &LowMap,
    const std::unordered_map<TokiPos, waterItem> &WaterList) {
  /*
{
  std::array<int64_t, 3> tempSize3D({schem.x_range(), schem.y_range(),
schem.z_range()}); Build.resize(tempSize3D);
}
*/
  schem.set_zero();
  // Base(r+1,c)<->High(r+1,c)<->Build(c+1,High(r+1,c),r+1)
  // 为了区分玻璃与空气，张量中存储的是 Base+1.所以元素为 1 对应着玻璃，0
  // 对应空气
  int x = 0, y = 0, z = 0;
  int yLow = 0;

  // cerr << WaterList.size() << " water columns in map\n";
  for (auto it = WaterList.begin(); it != WaterList.end();
       it++)  // 水柱周围的玻璃
  {
    x = TokiCol(it->first) + 1;
    z = TokiRow(it->first);
    y = waterHigh(it->second);
    yLow = waterLow(it->second);
    schem(x, y + 1, z) = 0 + 1;  // 柱顶玻璃
    for (short yDynamic = yLow; yDynamic <= y; yDynamic++) {
      schem(x - 1, yDynamic, z - 0) = 1;
      schem(x + 1, yDynamic, z + 0) = 1;
      schem(x + 0, yDynamic, z - 1) = 1;
      schem(x + 0, yDynamic, z + 1) = 1;
    }
    if (yLow >= 1) schem(x, yLow - 1, z) = 1;  // 柱底玻璃
  }

  progressAdd(wind, sizePic(2));

  for (short r = -1; r < sizePic(0); r++)  // 普通方块
  {
    for (short c = 0; c < sizePic(1); c++) {
      if (Base(r + 1, c) == 12 || Base(r + 1, c) == 0) continue;
      x = c + 1;
      y = LowMap(r + 1, c);
      z = r + 1;
      if (y >= 1 && blockPalette[Base(r + 1, c)].needGlass)
        schem(x, y - 1, z) = 0 + 1;
      if ((fireProof && blockPalette[Base(r + 1, c)].burnable) ||
          (endermanProof && blockPalette[Base(r + 1, c)].endermanPickable)) {
        if (y >= 1 && schem(x, y - 1, z) == 0) schem(x, y - 1, z) = 0 + 1;
        if (x >= 1 && schem(x - 1, y, z) == 0) schem(x - 1, y, z) = 0 + 1;
        if (z >= 1 && schem(x, y, z - 1) == 0) schem(x, y, z - 1) = 0 + 1;
        if (y + 1 < schem.y_range() && schem(x, y + 1, z) == 0)
          schem(x, y + 1, z) = 0 + 1;
        if (x + 1 < schem.x_range() && schem(x + 1, y, z) == 0)
          schem(x + 1, y, z) = 0 + 1;
        if (z + 1 < schem.z_range() && schem(x, y, z + 1) == 0)
          schem(x, y, z + 1) = 0 + 1;
      }

      schem(x, y, z) = Base(r + 1, c) + 1;
    }
    progressAdd(wind, sizePic(1));
  }

  progressAdd(wind, sizePic(2));

  for (auto it = WaterList.cbegin(); it != WaterList.cend(); ++it) {
    x = TokiCol(it->first) + 1;
    z = TokiRow(it->first);
    y = waterHigh(it->second);
    yLow = waterLow(it->second);
    for (short yDynamic = yLow; yDynamic <= y; yDynamic++) {
      schem(x, yDynamic, z) = 13;
    }
  }
  /*
  for(short c=0;c<sizePic(1);c++)//北侧方块
      if(Base(0,c))   Build(c+1,HighMap(0,c),0)=11+1;*/
}

void TokiSlopeCraft::makeBridge() {
  if (mapType != mapTypes::Slope) return;
  if (this->build_opt.glassMethod != glassBridgeSettings::withBridge) return;

  int step = sizePic(2) / schem.y_range();

  algoProgressRangeSet(wind, 0, 100, 0);

  for (uint32_t y = 0; y < schem.y_range(); y++) {
    keepAwake(wind);
    progressAdd(wind, step);
    if (y % (this->build_opt.bridgeInterval + 1) == 0) {
      std::array<int, 3> start, extension;  // x,z,y
      start[0] = 0;
      start[1] = 0;
      start[2] = y;
      extension[0] = schem.x_range();
      extension[1] = schem.z_range();
      extension[2] = 1;
      TokiMap targetMap = ySlice2TokiMap(
          schem.tensor().slice(start, extension).cast<uint8_t>());
      glassMap glass;
      cerr << "Construct glass bridge at y=" << y << endl;
      glass = glassBuilder->makeBridge(targetMap);
      for (int r = 0; r < glass.rows(); r++)
        for (int c = 0; c < glass.cols(); c++)
          if (schem(r, y, c) == PrimGlassBuilder::air &&
              glass(r, c) == PrimGlassBuilder::glass)
            schem(r, y, c) = PrimGlassBuilder::glass;
    } else {
      continue;
      std::array<int, 3> start, extension;
      start[0] = 0;
      start[1] = y;
      start[2] = 0;
      extension[0] = schem.x_range();
      extension[1] = 1;
      extension[2] = schem.z_range();
      TokiMap yCur = ySlice2TokiMap(
          schem.tensor().slice(start, extension).cast<uint8_t>());
      start[1] = y - 1;
      TokiMap yBelow = ySlice2TokiMap(
          schem.tensor().slice(start, extension).cast<uint8_t>());
      cerr << "Construct glass bridge between y=" << y << " and y=" << y - 1
           << endl;
      glassMap glass = connectBetweenLayers(yCur, yBelow, nullptr);

      for (int r = 0; r < glass.rows(); r++)
        for (int c = 0; c < glass.cols(); c++)
          if (schem(r, y, c) == PrimGlassBuilder::air &&
              glass(r, c) == PrimGlassBuilder::glass)
            schem(r, y, c) = PrimGlassBuilder::glass;
    }
  }
  algoProgressRangeSet(wind, 0, 100, 100);
  cerr << "makeBridge finished\n";
}

void TokiSlopeCraft::exportAsStructure(const char *TargetName,
                                       char *FileName) const {
  std::string temp = exportAsStructure(TargetName);

  if (FileName != nullptr) std::strcpy(temp.data(), FileName);
}

std::string TokiSlopeCraft::exportAsStructure(
    const std::string &TargetName) const {
  if (kernelStep < SCL_step::builded) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only export a map to structure after you build the 3D "
                "structure.");
    return "Too hasty! export structure after you built!";
  }

  reportWorkingStatue(wind, workStatues::writingMetaInfo);
  progressRangeSet(wind, 0, 100 + schem.size(), 0);

  errorFlag flag = errorFlag::NO_ERROR_OCCUR;
  std::string error_string;
  const bool success =
      schem.export_structure(TargetName, true, &flag, &error_string);

  if (!success) {
    this->reportError(this->wind, flag, error_string.data());
    return "Failed to export structure.\n" + error_string;
  }

  progressRangeSet(wind, 0, 100, 100);
  reportWorkingStatue(wind, workStatues::none);

  return "";
}

void TokiSlopeCraft::exportAsWESchem(const char *fileName,
                                     const int (&offset)[3],
                                     const int (&weOffset)[3], const char *Name,
                                     const char *const *const requiredMods,
                                     const int requiredModsCount,
                                     char *returnVal) const {
  if (fileName == nullptr || strlen(fileName) == 0) {
    return;
  }

  const std::array<int, 3> _offset({offset[0], offset[1], offset[2]});
  const std::array<int, 3> _weOffset({weOffset[0], weOffset[1], weOffset[2]});

  std::vector<const char *> _reqMods;
  _reqMods.reserve(requiredModsCount);
  if (requiredMods != nullptr)
    for (int idx = 0; idx < requiredModsCount; idx++) {
      if (requiredMods[idx] == nullptr) continue;
      _reqMods.emplace_back(requiredMods[idx]);
    }

  const std::string retVal = this->exportAsWESchem(
      std::string(fileName), _offset, _weOffset, Name, _reqMods);
  if (returnVal != nullptr) {
    strcpy(returnVal, retVal.data());
  }
  return;
}

std::string TokiSlopeCraft::exportAsWESchem(
    const std::string &targetName, const std::array<int, 3> &offset,
    const std::array<int, 3> &, const char *Name,
    const std::vector<const char *> &requiredMods) const {
  if (kernelStep < SCL_step::builded) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only export a map to structure after you build the 3D "
                "structure.");
    return "Too hasty! export structure after you built!";
  }

  // progress bar value : 10(open+others) + 64(palette) + Build + 10
  // (others+compress+remove)

  progressRangeSet(wind, 0, 100, 0);

  libSchem::WorldEditSchem_info info;

  info.schem_name_utf8 = Name;
  info.offset = offset;

  info.required_mods_utf8.resize(requiredMods.size());

  for (int idx = 0; idx < int(requiredMods.size()); idx++) {
    info.required_mods_utf8[idx] = requiredMods[idx];
  }

  progressRangeSet(wind, 0, 100, 5);

  errorFlag flag = errorFlag::NO_ERROR_OCCUR;
  std::string error_string;
  const bool success =
      schem.export_WESchem(targetName, info, &flag, &error_string);

  if (!success) {
    this->reportError(this->wind, flag, error_string.data());
    return "Failed to export as WE schem files.\n" + error_string;
  }

  progressRangeSet(wind, 0, 100, 100);

  return "";
}

int TokiSlopeCraft::getSchemPalette(const char **dest_id,
                                    size_t dest_capacity) const noexcept {
  if (kernelStep < SCL_step::builded) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only export a map to structure after you build the 3D "
                "structure.");
    return -1;
  }

  const int ret = this->schem.palette_size();

  if (dest_id == nullptr || dest_capacity <= 0) {
    return ret;
  }

  size_t write_counter = 0;
  for (const auto &id : this->schem.palette()) {
    if (write_counter < dest_capacity) {
      dest_id[write_counter] = id.c_str();
      write_counter++;
    }
  }

  return ret;
}