/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#include "TokiSlopeCraft.h"

void TokiSlopeCraft::makeTests(const AbstractBlock **src,
                               const unsigned char *baseColor, const char *dst,
                               char *_unFileName) {
  if (kernelStep < step::wait4Image) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "Skipping is not allowed.\nYou can only make tests only after "
                "you finished the map type and gameversion.");

    if (_unFileName != nullptr)
      std::strcpy(_unFileName, "");
    return;
  }

  std::string s = makeTests(src, baseColor, std::string(dst));
  if (_unFileName != nullptr)
    std::strcpy(_unFileName, s.data());
}

std::string TokiSlopeCraft::makeTests(const AbstractBlock **src,
                                      const uint8_t *src_baseColor,
                                      const std::string &fileName) {

  if (fileName.find_last_of(".nbt") == std::string::npos) {
    return "";
  }

  // const simpleBlock ** realSrc=(const simpleBlock **)src;
  std::vector<const simpleBlock *> realSrc;
  std::vector<uint8_t> realBaseColor;
  realSrc.clear();
  realBaseColor.clear();
  for (uint32_t idx = 0; src[idx] != nullptr; idx++) {
    if (src[idx]->getVersion() > mcVer) {
      continue;
    }
    realSrc.emplace_back((const simpleBlock *)src[idx]);
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

  int xSize = 0;
  static const int zSize = 64, ySize = 1;
  for (const auto &it : blocks) {
    xSize = std::max(size_t(xSize), it.size());
  }

  NBT::NBTWriter<false> file;
  std::string unCompress = fileName + ".TokiNoBug";
  file.open(unCompress.data());
  file.writeListHead("entities", NBT::Byte, 0);
  file.writeListHead("size", NBT::Int, 3);
  file.writeInt("This should never be shown", xSize);
  file.writeInt("This should never be shown", ySize);
  file.writeInt("This should never be shown", zSize);
  file.writeListHead("palette", NBT::Compound, realSrc.size() + 1);
  {
    std::vector<std::string> ProName, ProVal;
    std::string netBlockId;
    simpleBlock::dealBlockId("minecraft:air", netBlockId, &ProName, &ProVal);
    writeBlock(netBlockId, ProName, ProVal, file);
    for (const auto &it : blocks) {
      for (const auto jt : it) {
        simpleBlock::dealBlockId((mcVer > gameVersion::MC12)
                                     ? (realSrc[jt]->id)
                                     : (realSrc[jt]->idOld),
                                 netBlockId, &ProName, &ProVal);
        writeBlock(netBlockId, ProName, ProVal, file);
      }
    }
    file.writeListHead("blocks", NBT::Compound, realSrc.size());

    for (uint8_t base = 0; base < 64; base++) {
      for (uint32_t idx = 0; idx < blocks[base].size(); idx++) {
        int xPos = idx;
        int yPos = 0;
        int zPos = base;
        file.writeCompound("This should never be shown");
        file.writeListHead("pos", NBT::Int, 3);
        file.writeInt("This should never be shown", xPos);
        file.writeInt("This should never be shown", yPos);
        file.writeInt("This should never be shown", zPos);
        file.writeInt("state", blocks[base][idx] + 1);
        file.endCompound();
      }
    }
  }
  switch (mcVer) {
  case MC12:
  case MC13:
  case MC14:
  case MC15:
  case MC16:
  case MC17:
  case MC18:
  case MC19:
    file.writeInt("DataVersion", Kernel::mcVersion2VersionNumber(mcVer));
    break;
  default:
    cerr << "Wrong game version!\n";
    break;
  }
  file.close();

  if (!compressFile(unCompress.data(), fileName.data())) {
    std::string msg =
        "Failed to compress temporary file " + unCompress + " to " + fileName;
    reportError(wind, errorFlag::FAILED_TO_COMPRESS, msg.data());
    return unCompress;
  }

  if (std::remove(unCompress.data()) != 0) {
    std::string msg = "Failed to remove temporary file " + unCompress;
    reportError(wind, errorFlag::FAILED_TO_REMOVE, msg.data());
    return unCompress;
  }

  return fileName;
}

void TokiSlopeCraft::writeBlock(const std::string &netBlockId,
                                const std::vector<std::string> &Property,
                                const std::vector<std::string> &ProVal,
                                NBT::NBTWriter<false> &Lite) {
  Lite.writeCompound("ThisStringShouldNeverBeSeen");
  std::string BlockId = netBlockId;

  Lite.writeString("Name", BlockId.data());
  if (Property.empty() || ProVal.empty()) {
    Lite.endCompound();
    return;
  }

  if (Property.size() != ProVal.size()) {
    cerr << "Error: Property and ProVal have different sizes\n";
    return;
  }
  Lite.writeCompound("Properties");
  for (unsigned short i = 0; i < ProVal.size(); i++)
    Lite.writeString(Property.at(i).data(), ProVal.at(i).data());
  Lite.endCompound();
  // Property.clear();
  // ProVal.clear();
  Lite.endCompound();
}

void TokiSlopeCraft::writeTrash(int count, NBT::NBTWriter<false> &Lite) {
  std::vector<std::string> ProName(5), ProVal(5);
  // ProName:NEWSP
  //,,,,
  ProName.at(0) = "north";
  ProName.at(1) = "east";
  ProName.at(2) = "west";
  ProName.at(3) = "south";
  ProName.at(4) = "power";
  std::string dir[3] = {"none", "size", "up"};
  std::string power[16];
  for (short i = 0; i < 15; i++)
    power[i] = std::to_string(i);
  int written = 0;
  for (short North = 0; North < 3; North++)
    for (short East = 0; East < 3; East++)
      for (short West = 0; West < 3; West++)
        for (short South = 0; South < 3; South++)
          for (short Power = 0; Power < 16; Power++) {
            if (written >= count)
              return;
            if (!Lite.isInListOrArray())
              return;
            ProVal.at(0) = dir[North];
            ProVal.at(1) = dir[East];
            ProVal.at(2) = dir[West];
            ProVal.at(3) = dir[South];
            ProVal.at(4) = power[Power];
            writeBlock("minecraft:redstone_wire", ProName, ProVal, Lite);
            written++;
          }
}

void TokiSlopeCraft::exportAsLitematic(const char *TargetName,
                                       const char *LiteName,
                                       const char *RegionName,
                                       char *FileName) const {
  cerr << __FILE__ << " , " << __LINE__ << endl;
  std::string temp = exportAsLitematic(TargetName, LiteName, RegionName);
  cerr << __FILE__ << " , " << __LINE__ << endl;
  if (FileName != nullptr)
    std::strcpy(temp.data(), FileName);
}

std::string
TokiSlopeCraft::exportAsLitematic(const std::string &TargetName,
                                  const std::string &LiteName,
                                  const std::string &RegionName) const {
  if (kernelStep < builded) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only export a map to litematic after you build the 3D "
                "structure.");
    return "Too hasty! export litematic after you built!";
  }
  reportWorkingStatue(wind, workStatues::writingMetaInfo);
  progressRangeSet(wind, 0, 100 + Build.size(), 0);
  NBT::NBTWriter<false> Lite;

  std::string unCompressed = TargetName + ".TokiNoBug";
  Lite.open(unCompressed.data());

  Lite.writeCompound("Metadata");

  Lite.writeCompound("EnclosingSize");

  Lite.writeInt("x", size3D[0]);
  Lite.writeInt("y", size3D[1]);
  Lite.writeInt("z", size3D[2]);
  Lite.endCompound();
  const std::string author =
      std::string("SlopeCraft ") + Kernel::getSCLVersion();
  Lite.writeString("Author", author.data());

  static const std::string Description =
      "This litematic is generated by SlopeCraft " +
      std::string(Kernel::getSCLVersion()) + ", developed by TokiNoBug";
  Lite.writeString("Description", Description.data());
  Lite.writeString("Name", LiteName.data());

  Lite.writeInt("RegionCount", 1);

  Lite.writeLong("TimeCreated", 114514);
  Lite.writeLong("TimeModified", 1919810);

  Lite.writeInt("TotalBlocks", this->getBlockCounts());
  Lite.writeInt("TotalVolume", Build.size());

  Lite.endCompound();

  progressRangeSet(wind, 0, 100 + Build.size(), 50);
  Lite.writeCompound("Regions");
  Lite.writeCompound(RegionName.data());
  Lite.writeCompound("Position");
  Lite.writeInt("x", 0);
  Lite.writeInt("y", 0);
  Lite.writeInt("z", 0);
  Lite.endCompound();

  Lite.writeCompound("Size");
  Lite.writeInt("x", size3D[0]);
  Lite.writeInt("y", size3D[1]);
  Lite.writeInt("z", size3D[2]);
  Lite.endCompound();

  progressRangeSet(wind, 0, 100 + Build.size(), 100);

  reportWorkingStatue(wind, workStatues::writingBlockPalette);

  Lite.writeListHead("BlockStatePalette", NBT::Compound, 131);
  {
    short written = ((mcVer >= MC16) ? 59 : 52);
    if (mcVer >= 17)
      written = 62;
    std::vector<std::string> ProName, ProVal;
    // bool isNetBlockId;
    std::string netBlockId;

    simpleBlock::dealBlockId("minecraft:air", netBlockId, &ProName, &ProVal);
    writeBlock(netBlockId, ProName, ProVal, Lite);
    for (short r = 0; r < written; r++) {
      simpleBlock::dealBlockId((mcVer >= gameVersion::MC13)
                                   ? (blockPalette[r].id)
                                   : (blockPalette[r].idOld),
                               netBlockId, &ProName, &ProVal);
      writeBlock(netBlockId, ProName, ProVal, Lite);
    } //到此写入了written+1个方块，还需要写入130-written个

    writeTrash(130 - written, Lite);
  }
  Lite.writeListHead("Entities", NBT::Compound, 0);
  Lite.writeListHead("PendingBlockTicks", NBT::Compound, 0);
  Lite.writeListHead("PendingFluidTiccks", NBT::Compound, 0);
  Lite.writeListHead("TileEntities", NBT::Compound, 0);
  {
    int ArraySize;
    // Lite.writeLong("aLong",1145141919810);
    int Volume = size3D[0] * size3D[1] * size3D[2];
    ArraySize = ((Volume % 8) ? (Volume / 8 + 1) : Volume / 8);
    long long HackyVal = sizeof(long long);
    char *inverser = (char *)&HackyVal;
    short inverserIndex = 7;

    reportWorkingStatue(wind, workStatues::writing3D);

    Lite.writeLongArrayHead("BlockStates", ArraySize);
    for (int y = 0; y < size3D[1]; y++)
      for (int z = 0; z < size3D[2]; z++) {
        for (int x = 0; x < size3D[0]; x++) {
          inverser[inverserIndex--] = Build(x, y, z);

          if (inverserIndex < 0) {
            inverserIndex = 7;
            Lite.writeSingleTag<int64_t, false>(NBT::Long, "id", HackyVal);
            // Lite.writeLongDirectly("id", HackyVal);
          }
        }
        progressAdd(wind, size3D[0]);
      }

    if (!Lite.isListOrArrayFinished())
      Lite.writeSingleTag<int64_t, false>(NBT::Long, "id", HackyVal);
    // Lite.writeLongDirectly("id", HackyVal);
  }
  Lite.endCompound();

  Lite.endCompound();
  switch (mcVer) {
  case MC12:
    Lite.writeInt("MinecraftDataVersion", mcVersion2VersionNumber(mcVer));
    Lite.writeInt("Version", 4);
    break;
  case MC13:
  case MC14:
  case MC15:
  case MC16:
  case MC17:
  case MC18:
  case MC19:
    Lite.writeInt("MinecraftDataVersion", mcVersion2VersionNumber(mcVer));
    Lite.writeInt("Version", 5);
    break;
  default:
    cerr << "Wrong game version!\n";
    break;
  }

  Lite.close();

  reportWorkingStatue(wind, workStatues::none);

  if (!compressFile(unCompressed.data(), TargetName.data())) {
    std::string msg = "Failed to compress temporary file :" + unCompressed +
                      " to " + TargetName;
    reportError(wind, errorFlag::FAILED_TO_COMPRESS, msg.data());
    return unCompressed;
  }

  if (std::remove(unCompressed.data()) != 0) {
    std::string msg = "Failed to remove temporary file : " + unCompressed;
    reportError(wind, errorFlag::FAILED_TO_REMOVE, msg.data());
    return unCompressed;
  }

  return "";
}

bool TokiSlopeCraft::build(compressSettings cS, uint16_t mAH,
                           glassBridgeSettings gBS, uint16_t bI, bool fireProof,
                           bool endermanProof) {
  if (kernelStep < converted) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can build 3D strcuture only after you converted the raw image.");
    cerr << "hasty!" << endl;
    return false;
  }
  if (mAH < 14) {
    cerr << "maxAllowedHeight<14!" << endl;
    reportError(wind, errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14,
                "Your maximum allowed height is less than 14, which made lossy "
                "compressing almost impossible.");
    return false;
  }
  cerr << "ready to build" << endl;

  compressMethod = cS;
  glassMethod = gBS;
  if (isFlat() || !isVanilla()) {
    compressMethod = compressSettings::noCompress;
    glassMethod = glassBridgeSettings::noBridge;
  }

  maxAllowedHeight = mAH;
  bridgeInterval = bI;

  reportWorkingStatue(wind, workStatues::buidingHeighMap);

  progressRangeSet(wind, 0, 9 * sizePic(2), 0);
  cerr << "start makeHeight" << endl;

  mapPic.setZero(sizePic(0), sizePic(1));
  for (uint16_t r = 0; r < sizePic(0); r++) {
    for (uint16_t c = 0; c < sizePic(1); c++) {
      mapPic(r, c) = colorHash[ditheredImage(r, c)].Result;
    }
    progressAdd(wind, sizePic(1));
  }

  makeHeight_new();
  cerr << "makeHeight finished" << endl;
  progressRangeSet(wind, 0, 9 * sizePic(2), 5 * sizePic(2));

  reportWorkingStatue(wind, workStatues::building3D);
  cerr << "start buildHeight" << endl;
  buildHeight(fireProof, endermanProof);
  cerr << "buildHeight finished" << endl;
  progressRangeSet(wind, 0, 9 * sizePic(2), 8 * sizePic(2));

  reportWorkingStatue(wind, workStatues::constructingBridges);
  cerr << "start makeBridge" << endl;
  makeBridge();
  cerr << "makeBridge finished" << endl;
  progressRangeSet(wind, 0, 9 * sizePic(2), 9 * sizePic(2));

  if (mapType == mapTypes::Wall) {
    reportWorkingStatue(wind, workStatues::flippingToWall);
    Eigen::Tensor<uchar, 3> temp = Eigen::Tensor<uchar, 3>(Build);
    Eigen::array<int, 3> perm = {1, 2, 0};
    Build = temp.shuffle(perm);

    for (uint16_t x = 0; x < Build.dimension(0); x++)
      for (uint16_t z = 0; z < Build.dimension(2); z++)
        for (uint16_t y = 0; y < Build.dimension(1) / 2; y++) {
          std::swap(Build(x, y, z), Build(x, Build.dimension(1) - y - 1, z));
        }

    size3D[0] = Build.dimension(0);
    size3D[1] = Build.dimension(1);
    size3D[2] = Build.dimension(2);
  }

  reportWorkingStatue(wind, workStatues::none);

  kernelStep = builded;

  return true;
}

void TokiSlopeCraft::makeHeight_new() {
  Base.setZero(sizePic(0) + 1, sizePic(1));
  WaterList.clear();
  HighMap.setZero(sizePic(0) + 1, sizePic(1));
  LowMap.setZero(sizePic(0) + 1, sizePic(1));
  bool allowNaturalCompress = compressMethod == compressSettings::Both ||
                              compressMethod == compressSettings::NaturalOnly;
  // std::vector<const TokiColor*> src;
  cerr << "makeHeight_new\n";

  if ((mapPic - 4 * (mapPic / 4) >= 3).any()) {
    std::string msg =
        "Fatal error : SlopeCraftLib3 found map color with depth 3 in a "
        "vanilla map.\n Map contents (map color matrix in col-major) :\n[";

    for (int c = 0; c < mapPic.cols(); c++) {
      for (int r = 0; r < mapPic.rows(); r++) {
        msg += std::to_string(mapPic(r, c)) + ',';
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
    HL.make(mapPic.col(c), allowNaturalCompress);

    if (HL.maxHeight() > maxAllowedHeight &&
        (compressMethod == compressSettings::ForcedOnly ||
         compressMethod == compressSettings::Both)) {

      std::vector<const TokiColor *> ptr(getImageRows());

      getTokiColorPtr(c, &ptr[0]);

      Compressor->setSource(HL.getBase(), &ptr[0]);
      bool success =
          Compressor->compress(maxAllowedHeight, allowNaturalCompress);
      if (!success) {
        std::string msg = "Failed to compress the 3D structure at coloum " +
                          std::to_string(c);
        reportError(wind, LOSSYCOMPRESS_FAILED, msg.data());
        return;
      }
      Eigen::ArrayXi temp;
      HL.make(&ptr[0], Compressor->getResult(), allowNaturalCompress, &temp);
      mapPic.col(c) = temp;
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
  cerr << "makeHeight_new finished\n";
  size3D[2] = 2 + sizePic(0);         // z
  size3D[0] = 2 + sizePic(1);         // x
  size3D[1] = HighMap.maxCoeff() + 1; // y
}

void TokiSlopeCraft::buildHeight(bool fireProof, bool endermanProof) {
  {
    std::array<int64_t, 3> tempSize3D({size3D[0], size3D[1], size3D[2]});
    Build.resize(tempSize3D);
  }
  Build.setZero();
  // Base(r+1,c)<->High(r+1,c)<->Build(c+1,High(r+1,c),r+1)
  //为了区分玻璃与空气，张量中存储的是Base+1.所以元素为1对应着玻璃，0对应空气
  int x = 0, y = 0, z = 0;
  int yLow = 0;

  cerr << WaterList.size() << " water columns in map\n";
  for (auto it = WaterList.begin(); it != WaterList.end();
       it++) //水柱周围的玻璃
  {
    x = TokiCol(it->first) + 1;
    z = TokiRow(it->first);
    y = waterHigh(it->second);
    yLow = waterLow(it->second);
    Build(x, y + 1, z) = 0 + 1; //柱顶玻璃
    for (short yDynamic = yLow; yDynamic <= y; yDynamic++) {
      Build(x - 1, yDynamic, z - 0) = 1;
      Build(x + 1, yDynamic, z + 0) = 1;
      Build(x + 0, yDynamic, z - 1) = 1;
      Build(x + 0, yDynamic, z + 1) = 1;
    }
    if (yLow >= 1)
      Build(x, yLow - 1, z) = 1; //柱底玻璃
  }

  progressAdd(wind, sizePic(2));

  for (short r = -1; r < sizePic(0); r++) //普通方块
  {
    for (short c = 0; c < sizePic(1); c++) {
      if (Base(r + 1, c) == 12 || Base(r + 1, c) == 0)
        continue;
      x = c + 1;
      y = LowMap(r + 1, c);
      z = r + 1;
      if (y >= 1 && blockPalette[Base(r + 1, c)].needGlass)
        Build(x, y - 1, z) = 0 + 1;
      if ((fireProof && blockPalette[Base(r + 1, c)].burnable) ||
          (endermanProof && blockPalette[Base(r + 1, c)].endermanPickable)) {
        if (y >= 1 && Build(x, y - 1, z) == 0)
          Build(x, y - 1, z) = 0 + 1;
        if (x >= 1 && Build(x - 1, y, z) == 0)
          Build(x - 1, y, z) = 0 + 1;
        if (z >= 1 && Build(x, y, z - 1) == 0)
          Build(x, y, z - 1) = 0 + 1;
        if (y + 1 < size3D[1] && Build(x, y + 1, z) == 0)
          Build(x, y + 1, z) = 0 + 1;
        if (x + 1 < size3D[0] && Build(x + 1, y, z) == 0)
          Build(x + 1, y, z) = 0 + 1;
        if (z + 1 < size3D[2] && Build(x, y, z + 1) == 0)
          Build(x, y, z + 1) = 0 + 1;
      }

      Build(x, y, z) = Base(r + 1, c) + 1;
    }
    progressAdd(wind, sizePic(1));
  }

  progressAdd(wind, sizePic(2));

  for (auto it = WaterList.cbegin(); it != WaterList.cend(); it++) {
    x = TokiCol(it->first) + 1;
    z = TokiRow(it->first);
    y = waterHigh(it->second);
    yLow = waterLow(it->second);
    for (short yDynamic = yLow; yDynamic <= y; yDynamic++) {
      Build(x, yDynamic, z) = 13;
    }
  }
  /*
  for(short c=0;c<sizePic(1);c++)//北侧方块
      if(Base(0,c))   Build(c+1,HighMap(0,c),0)=11+1;*/
}

void TokiSlopeCraft::makeBridge() {
  if (mapType != mapTypes::Slope)
    return;
  if (glassMethod != glassBridgeSettings::withBridge)
    return;

  int step = sizePic(2) / Build.dimension(1);

  algoProgressRangeSet(wind, 0, 100, 0);

  for (uint32_t y = 0; y < Build.dimension(1); y++) {
    keepAwake(wind);
    progressAdd(wind, step);
    if (y % (bridgeInterval + 1) == 0) {
      std::array<int, 3> start, extension;
      start[0] = 0;
      start[1] = y;
      start[2] = 0;
      extension[0] = size3D[0];
      extension[1] = 1;
      extension[2] = size3D[2];
      TokiMap targetMap = ySlice2TokiMap(Build.slice(start, extension));
      glassMap glass;
      cerr << "Construct glass bridge at y=" << y << endl;
      glass = glassBuilder->makeBridge(targetMap);
      for (int r = 0; r < glass.rows(); r++)
        for (int c = 0; c < glass.cols(); c++)
          if (Build(r, y, c) == PrimGlassBuilder::air &&
              glass(r, c) == PrimGlassBuilder::glass)
            Build(r, y, c) = PrimGlassBuilder::glass;
    } else {
      continue;
      std::array<int, 3> start, extension;
      start[0] = 0;
      start[1] = y;
      start[2] = 0;
      extension[0] = size3D[0];
      extension[1] = 1;
      extension[2] = size3D[2];
      TokiMap yCur = ySlice2TokiMap(Build.slice(start, extension));
      start[1] = y - 1;
      TokiMap yBelow = ySlice2TokiMap(Build.slice(start, extension));
      cerr << "Construct glass bridge between y=" << y << " and y=" << y - 1
           << endl;
      glassMap glass = connectBetweenLayers(yCur, yBelow, nullptr);

      for (int r = 0; r < glass.rows(); r++)
        for (int c = 0; c < glass.cols(); c++)
          if (Build(r, y, c) == PrimGlassBuilder::air &&
              glass(r, c) == PrimGlassBuilder::glass)
            Build(r, y, c) = PrimGlassBuilder::glass;
    }
  }
  algoProgressRangeSet(wind, 0, 100, 100);
  cerr << "makeBridge finished\n";
}

void TokiSlopeCraft::exportAsStructure(const char *TargetName,
                                       char *FileName) const {

  std::string temp = exportAsStructure(TargetName);

  if (FileName != nullptr)
    std::strcpy(temp.data(), FileName);
}

std::string
TokiSlopeCraft::exportAsStructure(const std::string &TargetName) const {
  if (kernelStep < builded) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only export a map to structure after you build the 3D "
                "structure.");
    return "Too hasty! export structure after you built!";
  }

  reportWorkingStatue(wind, workStatues::writingMetaInfo);
  progressRangeSet(wind, 0, 100 + Build.size(), 0);
  NBT::NBTWriter<false> file;

  std::string unCompress = TargetName + ".TokiNoBug";
  file.open(unCompress.data());

  file.writeListHead("entities", NBT::Byte, 0);
  file.writeListHead("size", NBT::Int, 3);
  file.writeInt("This should never be shown", size3D[0]);
  file.writeInt("This should never be shown", size3D[1]);
  file.writeInt("This should never be shown", size3D[2]);

  reportWorkingStatue(wind, workStatues::writingBlockPalette);

  file.writeListHead("palette", NBT::Compound, 70);
  {
    short written = ((mcVer >= MC16) ? 59 : 52);
    if (mcVer >= MC17)
      written = 62;
    std::vector<std::string> ProName, ProVal;
    // bool isNetBlockId;
    std::string netBlockId;

    simpleBlock::dealBlockId("minecraft:air", netBlockId, &ProName, &ProVal);
    writeBlock(netBlockId, ProName, ProVal, file);
    for (short r = 0; r < written; r++) {
      simpleBlock::dealBlockId((mcVer > gameVersion::MC12)
                                   ? (blockPalette[r].id)
                                   : (blockPalette[r].idOld),
                               netBlockId, &ProName, &ProVal);
      writeBlock(netBlockId, ProName, ProVal, file);
    } //到此写入了written+1个方块，还需要写入69-written个

    writeTrash(69 - written, file);
  }

  int BlockCount = 0;
  for (int i = 0; i < Build.size(); i++)
    if (Build(i))
      BlockCount++;

  reportWorkingStatue(wind, workStatues::writing3D);

  file.writeListHead("blocks", NBT::Compound, BlockCount);
  for (int x = 0; x < size3D[0]; x++)
    for (int y = 0; y < size3D[1]; y++) {
      for (int z = 0; z < size3D[2]; z++) {
        if (!Build(x, y, z))
          continue;
        file.writeCompound("This should never be shown");
        file.writeListHead("pos", NBT::Int, 3);
        file.writeInt("This should never be shown", x);
        file.writeInt("This should never be shown", y);
        file.writeInt("This should never be shown", z);
        file.writeInt("state", Build(x, y, z));
        file.endCompound();
      }
      progressAdd(wind, size3D[2]);
    }
  switch (mcVer) {
  case MC12:
  case MC13:
  case MC14:
  case MC15:
  case MC16:
  case MC17:
  case MC18:
  case MC19:
    file.writeInt("MinecraftDataVersion", mcVersion2VersionNumber(mcVer));
    break;
  default:
    cerr << "Wrong game version!\n";
    break;
  }

  file.close();

  progressRangeSet(wind, 0, 100, 100);
  reportWorkingStatue(wind, workStatues::none);

  if (!compressFile(unCompress.data(), TargetName.data())) {
    std::string msg = "Failed to compress temporary file :" + unCompress +
                      " to " + TargetName;
    reportError(wind, errorFlag::FAILED_TO_COMPRESS, msg.data());
    return unCompress;
  }

  if (std::remove(unCompress.data()) != 0) {
    std::string msg = "Failed to remove temporary file : " + unCompress;
    reportError(wind, errorFlag::FAILED_TO_REMOVE, msg.data());
    return unCompress;
  }

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
      if (requiredMods[idx] == nullptr)
        continue;
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
    const std::array<int, 3> &weOffset, const char *Name,
    const std::vector<const char *> &requiredMods) const {
  if (kernelStep < builded) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only export a map to structure after you build the 3D "
                "structure.");
    return "Too hasty! export structure after you built!";
  }

  // progress bar value : 10(open+others) + 64(palette) + Build + 10
  // (others+compress+remove)

  const int progressMax = 10 + 64 + Build.size() + 10;

  progressRangeSet(wind, 0, progressMax, 0);

  NBT::NBTWriter<false> file;
  const std::string unCompressed = targetName + ".TokiNoBug";
  if (!file.open(unCompressed.data())) {
    cout << "Failed to open file " << unCompressed << endl;
    return "Failed to open file";
  }

  // write metadata
  file.writeCompound("Metadata");
  file.writeInt("WEOffsetX", weOffset[0]);
  file.writeInt("WEOffsetY", weOffset[1]);
  file.writeInt("WEOffsetZ", weOffset[2]);
  file.writeString("Name", Name);
  file.writeString(
      "Author", (std::string("SlopeCraft ") + Kernel::getSCLVersion()).data());
  file.writeLong("Date", std::time(nullptr) * 1000);

  file.writeListHead("RequiredMods", NBT::String, requiredMods.size());
  for (const char *str : requiredMods) {
    file.writeString("", str);
  }
  file.endCompound();

  int paletteMax = ((mcVer >= MC16) ? 59 : 52);
  if (mcVer >= 17)
    paletteMax = 62;

  progressAdd(wind, 10);

  file.writeCompound("Palette");
  {
    int blockIdx = 0;
    file.writeInt("minecraft:air", 0);
    for (const simpleBlock &block : blockPalette) {
      blockIdx++;
      if (blockIdx >= paletteMax)
        break;
      file.writeInt(block.getId(), blockIdx);
    }
  }
  file.endCompound();

  file.writeListHead("BlockEntities", NBT::Compound, 0);

  file.writeInt("DataVersion", Kernel::mcVersion2VersionNumber(this->mcVer));

  file.writeShort("Width", this->size3D[0]);
  file.writeShort("Height", this->size3D[1]);
  file.writeShort("Length", this->size3D[2]);

  file.writeInt("Version", 2);

  file.writeInt("PaletteMax", paletteMax);

  progressAdd(wind, 64);

  file.writeByteArrayHead("BlockData", this->Build.size());
  for (int y = 0; y < size3D[1]; y++) {
    for (int z = 0; z < size3D[2]; z++) {
      for (int x = 0; x < size3D[0]; x++) {
        file.writeByte("", this->Build(x, y, z));
      }
    }
    progressAdd(wind, size3D[2] * size3D[0]);
  }

  file.writeIntArrayHead("Offset", 3);
  {
    file.writeInt("x", offset[0]);
    file.writeInt("y", offset[1]);
    file.writeInt("z", offset[2]);
  }

  if (!file.close()) {
    cout << "Failed to close file" << endl;
    return "Failed to close file";
  }

  progressAdd(wind, 5);

  if (!compressFile(unCompressed.data(), targetName.data())) {
    const std::string msg = "Failed to gzip compress file : " + unCompressed;
    cout << "Failed to gzip compress file " << unCompressed << endl;
    this->reportError(this->wind, errorFlag::FAILED_TO_COMPRESS, msg.data());
    return unCompressed;
  }

  if (std::remove(unCompressed.data()) != 0) {
    std::string msg = "Failed to remove temporary file : " + unCompressed;
    reportError(wind, errorFlag::FAILED_TO_REMOVE, msg.data());
    return unCompressed;
  }

  progressRangeSet(wind, 0, progressMax, progressMax);

  return "";
}
