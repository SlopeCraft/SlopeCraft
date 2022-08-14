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



void matchColor(uint32_t taskCount, TokiColor **tk, ARGB *argb);

bool TokiSlopeCraft::convert(convertAlgo algo, bool dither) {
  if (kernelStep < convertionReady) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can call convert only after you imported the raw image");
    return false;
  }

  if (algo == convertAlgo::gaCvter) {

    convertAlgo algos[6] = {RGB, RGB_Better, HSV, Lab94, Lab00, XYZ};
    Eigen::ArrayXX<uint8_t> CvtedMap[6];
    std::vector<const Eigen::ArrayXX<uint8_t> *> seeds(6);
    for (int a = 0; a < 6; a++) {
      this->convert(algos[a]);
      CvtedMap[a].resize(getImageRows(), getImageCols());
      this->getConvertedMap(nullptr, nullptr, CvtedMap[a].data());
      seeds[a] = &CvtedMap[a];
    }

    {
      heu::GAOption opt;
      opt.crossoverProb = AiOpt.crossoverProb;
      opt.mutateProb = AiOpt.mutationProb;
      opt.maxGenerations = AiOpt.maxGeneration;
      opt.maxFailTimes = AiOpt.maxFailTimes;
      opt.populationSize = AiOpt.popSize;
      GAConverter->setOption(opt);
    }

    GAConverter->setSeeds(seeds);

    GAConverter->run();

    // replace raw image with ai result
    GAConverter->resultImage(&rawImage);

    algo = convertAlgo::RGB_Better;

  }

  ConvertAlgo = algo;
  colorHash.clear();

  progressRangeSet(wind, 0, 4 * sizePic(2), 0);

  /*
  //第一步，装入hash顺便转换颜色空间;（一次遍历
  //第二步，遍历hash并匹配颜色;（一次遍历
  //第三步，从hash中检索出对应的匹配结果;（一次遍历
  //第四步，抖动（一次遍历*/

  reportWorkingStatue(wind, workStatues::collectingColors);
  pushToHash();

  keepAwake(wind);
  progressRangeSet(wind, 0, 4 * sizePic(2), 1 * sizePic(2));

  reportWorkingStatue(wind, workStatues::converting);
  applyTokiColor();

  keepAwake(wind);
  progressRangeSet(wind, 0, 4 * sizePic(2), 2 * sizePic(2));

  fillMapMat();
  keepAwake(wind);
  progressRangeSet(wind, 0, 4 * sizePic(2), 3 * sizePic(2));

  ditheredImage = this->rawImage;

  if (dither) {
    reportWorkingStatue(wind, workStatues::dithering);
    Dither();
  }
  progressRangeSet(wind, 0, 4 * sizePic(2), 4 * sizePic(2));
  keepAwake(wind);

  reportWorkingStatue(wind, workStatues::none);

  kernelStep = converted;
  return true;
}

void TokiSlopeCraft::pushToHash() {
  auto R = &colorHash;
  R->clear();

  ::SlopeCraft::convertAlgo Mode = ConvertAlgo;
  TokiColor::convertAlgo = Mode;

  // R->reserve(sizePic(2)/4);

  for (uint32_t idx = 0; idx < rawImage.size(); idx++) {
    R->emplace(rawImage(idx), rawImage(idx));

    if ((idx / sizePic(1)) % reportRate == 0) {
      progressAdd(wind, reportRate * sizePic(1));
    }
  }

  cerr << "Total color count:" << R->size() << endl;
}

void TokiSlopeCraft::applyTokiColor() {
  static const uint64_t threadCount = std::thread::hardware_concurrency();
  const uint64_t taskCount = colorHash.size();

  // int step=threadCount*sizePic(2)/taskCount;
  // int step=threadCount*sizePic(2)/reportRate;

  std::vector<std::pair<const ARGB, TokiColor> *> tasks;

  tasks.clear();
  tasks.reserve(taskCount);

  for (auto it = colorHash.begin(); it != colorHash.end(); ++it) {
    tasks.emplace_back(&*it);
  }

  static const std::clock_t interval = 1 * CLOCKS_PER_SEC;
  std::clock_t prevClock = std::clock();
#pragma omp parallel for
  for (uint64_t begIdx = 0; begIdx < threadCount; begIdx++) {
    //
    for (uint64_t idx = begIdx; idx < taskCount; idx += threadCount) {
      tasks[idx]->second.apply(tasks[idx]->first);
      if (idx % reportRate == 0) {
        std::clock_t curClock = std::clock();
        if (curClock - prevClock > interval) {
          prevClock = curClock;
          progressRangeSet(wind, 0, 4 * sizePic(2), sizePic(2) + idx);
        }
      }
    }
  }

  cerr << "colors converted\n";
}

void TokiSlopeCraft::fillMapMat() {
  mapPic.setZero(getImageRows(), getImageCols());
  // auto R=&colorHash;
  for (short r = 0; r < sizePic(0); r++) {
    for (short c = 0; c < sizePic(1); c++) {
      mapPic(r, c) = colorHash[rawImage(r, c)].Result;
    }

    if (r % reportRate == 0)
      progressAdd(wind, reportRate * sizePic(1));
  }
}

void TokiSlopeCraft::Dither() {
  // colorHash.reserve(3*colorHash.size());
  auto R = &colorHash;
  Eigen::ArrayXXf Dither[3];
  /*
  cout<<"DitherMapLR="<<endl;
  cout<<DitherMapLR<<endl;
  cout<<"DitherMapRL="<<endl;
  cout<<DitherMapRL<<endl;*/
  bool isDirLR = true;
  Dither[0].setZero(sizePic(0) + 2, sizePic(1) + 2);
  Dither[1].setZero(sizePic(0) + 2, sizePic(1) + 2);
  Dither[2].setZero(sizePic(0) + 2, sizePic(1) + 2);

  ditheredImage.setZero(sizePic(0), sizePic(1));

  const Eigen::Array<float, 256, 3> *ColorMap = nullptr;
  ARGB Current;
  ARGB (*CvtFun)(float, float, float);
  switch (ConvertAlgo) {
  case ::SlopeCraft::convertAlgo::RGB:
  case ::SlopeCraft::convertAlgo::RGB_Better:
    ColorMap = &Basic._RGB;
    CvtFun = RGB2ARGB;
    break;
  case ::SlopeCraft::convertAlgo::HSV:
    ColorMap = &Basic.HSV;
    CvtFun = HSV2ARGB;
    break;
  case ::SlopeCraft::convertAlgo::Lab00:
  case ::SlopeCraft::convertAlgo::Lab94:
    ColorMap = &Basic.Lab;
    CvtFun = Lab2ARGB;
    break;
  default:
    ColorMap = &Basic.XYZ;
    CvtFun = XYZ2ARGB;
    break;
  }
  const Eigen::Array<float, 256, 3> &CM = *ColorMap;

  // int t=sizeof(Eigen::Array3f);

  int index = 0;
  for (short r = 0; r < sizePic(0); r++) {
    for (short c = 0; c < sizePic(1); c++) {
      TokiColor &&temp = std::move(R->operator[](rawImage(r, c)));
      Dither[0](r + 1, c + 1) = temp.c3[0];
      Dither[1](r + 1, c + 1) = temp.c3[1];
      Dither[2](r + 1, c + 1) = temp.c3[2];
    }
  }
  cerr << "Filled Dither matrix\n";
  int newCount = 0;
  // TokiColor* oldColor=nullptr;
  for (short r = 0; r < sizePic(0);
       r++) //底部一行、左右两侧不产生误差扩散，只接受误差
  {
    if (isDirLR) //从左至右遍历
    {
      for (short c = 0; c < sizePic(1); c++) {
        // float Error[3];
        Eigen::Array3f Error;
        if (getA(rawImage(r, c)) <= 0)
          continue;

        Current = CvtFun(Dither[0](r + 1, c + 1), Dither[1](r + 1, c + 1),
                         Dither[2](r + 1, c + 1));
        ditheredImage(r, c) = Current;

        auto find = R->find(Current);

        if (find == R->end()) {
          R->emplace(Current, TokiColor(Current));
          find = R->find(Current);
          find->second.apply(Current);
          //装入了一个新颜色并匹配为地图色
          newCount++;
        }
        TokiColor &oldColor = find->second;
        mapPic(r, c) = oldColor.Result;
        index = mapColor2Index(mapPic(r, c));

        Error = oldColor.c3 - CM.row(index).transpose();
        /*
        Error[0]=oldColor->c3[0]-CM(index,0);
        Error[1]=oldColor->c3[1]-CM(index,1);
        Error[2]=oldColor->c3[2]-CM(index,2);
        */

        Dither[0].block<2, 3>(r + 1, c + 1 - 1) += Error[0] * DitherMapLR;
        Dither[1].block<2, 3>(r + 1, c + 1 - 1) += Error[1] * DitherMapLR;
        Dither[2].block<2, 3>(r + 1, c + 1 - 1) += Error[2] * DitherMapLR;
      }
    } else {
      for (short c = sizePic(1) - 1; c >= 0; c--) {
        // float Error[3];
        Eigen::Array3f Error;
        if (getA(rawImage(r, c)) <= 0)
          continue;

        Current = CvtFun(Dither[0](r + 1, c + 1), Dither[1](r + 1, c + 1),
                         Dither[2](r + 1, c + 1));
        ditheredImage(r, c) = Current;

        auto find = R->find(Current);
        if (find == R->end()) {
          R->emplace(Current, Current);
          find = R->find(Current);
          find->second.apply(Current);
          //装入了一个新颜色并匹配为地图色
          newCount++;
        }
        TokiColor &oldColor = find->second;
        mapPic(r, c) = oldColor.Result;
        index = mapColor2Index(mapPic(r, c));

        Error = oldColor.c3 - CM.row(index).transpose();
        /*
        Error[0]=oldColor->c3[0]-CM(index,0);
        Error[1]=oldColor->c3[1]-CM(index,1);
        Error[2]=oldColor->c3[2]-CM(index,2);
        */

        Dither[0].block<2, 3>(r + 1, c + 1 - 1) += Error[0] * DitherMapRL;
        Dither[1].block<2, 3>(r + 1, c + 1 - 1) += Error[1] * DitherMapRL;
        Dither[2].block<2, 3>(r + 1, c + 1 - 1) += Error[2] * DitherMapRL;
      }
    }
    isDirLR = !isDirLR;
    if (r % reportRate == 0) {
      keepAwake(wind);
      progressAdd(wind, reportRate * sizePic(1));
    }
  }
  cerr << "Error diffuse finished\n";
  cerr << "Inserted " << newCount << " colors to hash\n";
}

void matchColor(uint32_t taskCount, TokiColor **tk, ARGB *argb) {
  for (uint32_t i = 0; i < taskCount; i++) {
    tk[i]->apply(argb[i]);
  }
}


void TokiSlopeCraft::exportAsData(const char *FolderPath, const int indexStart,
                                  int *fileCount, char **dest) const {
  std::vector<std::string> uFL = exportAsData(FolderPath, indexStart);
  if (fileCount != nullptr)
    *fileCount = uFL.size();
  if (dest != nullptr)
    for (uint16_t i = 0; i < uFL.size(); i++) {
      if (dest[i] != nullptr)
        std::strcpy(dest[i], uFL[i].data());
    }
}

std::vector<std::string> TokiSlopeCraft::exportAsData(const std::string &FolderPath,
                             int indexStart) const {
  std::vector<std::string> unCompressedFileList;
  unCompressedFileList.clear();
  std::vector<std::string> compressedFileList;
  compressedFileList.clear();

  if (kernelStep < converted) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can export the map as map data files only after the image "
                "is converted.");
    unCompressedFileList.push_back(
        "Too hasty! export after you converted the map!");
    return unCompressedFileList;
  }

  const int rows = ceil(mapPic.rows() / 128.0f);
  const int cols = ceil(mapPic.cols() / 128.0f);
  // const int maxrr=rows*128;
  // const int maxcc=cols*128;
  progressRangeSet(wind, 0, 128 * rows * cols, 0);

  int offset[2] = {0, 0}; // r,c
  int currentIndex = indexStart;

  reportWorkingStatue(wind, workStatues::writingMapDataFiles);

  for (int c = 0; c < cols; c++) {
    for (int r = 0; r < rows; r++) {
      offset[0] = r * 128;
      offset[1] = c * 128;
      std::string currentCn =
          FolderPath + "/map_" + std::to_string(currentIndex) + ".dat";
      std::string currentUn = currentCn + ".TokiNoBug";
      // string
      // currentFile=FolderPath+"/map_"+std::to_string(currentIndex)+".dat";

      cerr << "Export map of (" << r << "," << c << ")" << currentUn << endl;

      NBT::NBTWriter MapFile(currentUn.data());

      switch (mcVer) {
      case MC12:
      case MC13:
        break;
      case MC14:
      case MC15:
      case MC16:
      case MC17:
      case MC18:
      case MC19:
        MapFile.writeInt("DataVersion", mcVersion2VersionNumber(mcVer));
        break;
      default:
        cerr << "Wrong game version!\n";
        break;
      }
      static const std::string ExportedBy =
          "Exported by SlopeCraft " + std::string(Kernel::getSCLVersion()) +
          ", developed by TokiNoBug";
      MapFile.writeString("ExportedBy", ExportedBy.data());
      MapFile.writeCompound("data");
      MapFile.writeByte("scale", 0);
      MapFile.writeByte("trackingPosition", 0);
      MapFile.writeByte("unlimitedTracking", 0);
      MapFile.writeInt("xCenter", 0);
      MapFile.writeInt("zCenter", 0);
      switch (mcVer) {
      case MC12:
        MapFile.writeByte("dimension", 114);
        MapFile.writeShort("height", 128);
        MapFile.writeShort("width", 128);
        break;
      case MC13:
        MapFile.writeListHead("banners", NBT::Compound, 0);
        MapFile.writeListHead("frames", NBT::Compound, 0);
        MapFile.writeInt("dimension", 889464);
        break;
      case MC14:
        MapFile.writeListHead("banners", NBT::Compound, 0);
        MapFile.writeListHead("frames", NBT::Compound, 0);
        MapFile.writeInt("dimension", 0);
        MapFile.writeByte("locked", 1);
        break;
      case MC15:
        MapFile.writeListHead("banners", NBT::Compound, 0);
        MapFile.writeListHead("frames", NBT::Compound, 0);
        MapFile.writeInt("dimension", 0);
        MapFile.writeByte("locked", 1);
        break;
      case MC16:
      case MC17:
      case MC18:
      case MC19:
        MapFile.writeListHead("banners", NBT::Compound, 0);
        MapFile.writeListHead("frames", NBT::Compound, 0);
        MapFile.writeString("dimension", "minecraft:overworld");
        MapFile.writeByte("locked", 1);
        break;
      default:
        cerr << "Wrong game version!\n";
        break;
      }

      MapFile.writeByteArrayHead("colors", 16384);
      uchar ColorCur = 0;
      for (short rr = 0; rr < 128; rr++) {
        for (short cc = 0; cc < 128; cc++) {
          if (rr + offset[0] < mapPic.rows() && cc + offset[1] < mapPic.cols())
            ColorCur = mapPic(rr + offset[0], cc + offset[1]);
          else
            ColorCur = 0;
          MapFile.writeByte("this should never be seen", ColorCur);
        }
        progressAdd(wind, 1);
      }
      MapFile.endCompound();
      MapFile.close();
      unCompressedFileList.emplace_back(currentUn);
      compressedFileList.emplace_back(currentCn);
      /*
      if(compressFile(currentUn.data(),currentFile.data()))
      {
          qDebug("压缩成功");
          QFile umComFile(QString::fromStdString(currentUn));
          umComFile.remove();
      }*/
      currentIndex++;
    }
  }

  reportWorkingStatue(wind, workStatues::none);

  for (uint32_t i = 0; i < compressedFileList.size(); i++) {
    bool success = true;
    success = success && compressFile(unCompressedFileList[i].data(),
                                      compressedFileList[i].data());
    std::string msg =
        "Failed to compress temporary file : " + unCompressedFileList[i] +
        " to " + compressedFileList[i];
    if (!success) {
      reportError(wind, errorFlag::FAILED_TO_COMPRESS, msg.data());
      continue;
    }
    success = success && (std::remove(unCompressedFileList[i].data()) == 0);
    if (!success) {
      std::string msg =
          "Failed to remove temporary file : " + unCompressedFileList[i];
      reportError(wind, errorFlag::FAILED_TO_REMOVE, msg.data());
      continue;
    }
  }

  return compressedFileList;
}
