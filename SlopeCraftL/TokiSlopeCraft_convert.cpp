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
#include <filesystem>

bool TokiSlopeCraft::convert(convertAlgo algo, bool dither) {
  if (kernelStep < SCL_step::convertionReady) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can call convert only after you imported the raw image");
    return false;
  }

  if (algo == convertAlgo::gaCvter) {
    algo = convertAlgo::RGB_Better;
  }

  // this->ConvertAlgo = algo;

  // TokiColor::convertAlgo() = algo;
  progressRangeSet(wind, 0, 100, 100);
  reportWorkingStatue(wind, workStatues::converting);

  {
    heu::GAOption opt;
    opt.crossoverProb = AiOpt.crossoverProb;
    opt.mutateProb = AiOpt.mutationProb;
    opt.maxGenerations = AiOpt.maxGeneration;
    opt.maxFailTimes = AiOpt.maxFailTimes;
    opt.populationSize = AiOpt.popSize;
    // here opt is passed as a const ptr, it won't be changed in this function
    // call
    this->image_cvter.convert_image(algo, dither, &opt);
  }

  /*
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
    */
  this->mapPic = this->image_cvter.mapcolor_matrix().cast<int>();
  progressRangeSet(wind, 0, 4 * sizePic(2), 4 * sizePic(2));
  keepAwake(wind);

  reportWorkingStatue(wind, workStatues::none);

  kernelStep = SCL_step::converted;
  return true;
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

std::vector<std::string> TokiSlopeCraft::exportAsData(std::string FolderPath,
                                                      int indexStart) const {

  std::vector<std::string> failed_file_list;

  if (kernelStep < SCL_step::converted) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can export the map as map data files only after the image "
                "isSCL_step::converted.");
    failed_file_list.push_back(
        "Too hasty! export after youSCL_step::converted the map!");
    return failed_file_list;
  }

  if (FolderPath.back() == '/') {
    FolderPath.pop_back();
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
      std::string current_filename =
          FolderPath + "/map_" + std::to_string(currentIndex) + ".dat";
      // string
      // currentFile=FolderPath+"/map_"+std::to_string(currentIndex)+".dat";

      // cerr << "Export map of (" << r << "," << c << ")" << current_filename
      // << endl;

      NBT::NBTWriter<true> MapFile;

      if (!MapFile.open(current_filename.data())) {
        cerr << "Failed to create nbt file " << current_filename << endl;
        failed_file_list.emplace_back(current_filename);
        continue;
      }

      switch (mcVer) {
      case SCL_gameVersion::MC12:
      case SCL_gameVersion::MC13:
        break;
      case SCL_gameVersion::MC14:
      case SCL_gameVersion::MC15:
      case SCL_gameVersion::MC16:
      case SCL_gameVersion::MC17:
      case SCL_gameVersion::MC18:
      case SCL_gameVersion::MC19:
        MapFile.writeInt("DataVersion", mcVersion2VersionNumber(mcVer));
        break;
      default:
        cerr << "Wrong game version!\n";
        break;
      }
      static const std::string ExportedBy =
          "Exported by SlopeCraft " +
          std::string(TokiSlopeCraft::getSCLVersion()) +
          ", developed by TokiNoBug";
      MapFile.writeString("ExportedBy", ExportedBy.data());
      MapFile.writeCompound("data");
      {
        MapFile.writeByte("scale", 0);
        MapFile.writeByte("trackingPosition", 0);
        MapFile.writeByte("unlimitedTracking", 0);
        MapFile.writeInt("xCenter", 0);
        MapFile.writeInt("zCenter", 0);
        switch (mcVer) {
        case SCL_gameVersion::MC12:
          MapFile.writeByte("dimension", 114);
          MapFile.writeShort("height", 128);
          MapFile.writeShort("width", 128);
          break;
        case SCL_gameVersion::MC13:
          MapFile.writeListHead("banners", NBT::Compound, 0);
          MapFile.writeListHead("frames", NBT::Compound, 0);
          MapFile.writeInt("dimension", 889464);
          break;
        case SCL_gameVersion::MC14:
          MapFile.writeListHead("banners", NBT::Compound, 0);
          MapFile.writeListHead("frames", NBT::Compound, 0);
          MapFile.writeInt("dimension", 0);
          MapFile.writeByte("locked", 1);
          break;
        case SCL_gameVersion::MC15:
          MapFile.writeListHead("banners", NBT::Compound, 0);
          MapFile.writeListHead("frames", NBT::Compound, 0);
          MapFile.writeInt("dimension", 0);
          MapFile.writeByte("locked", 1);
          break;
        case SCL_gameVersion::MC16:
        case SCL_gameVersion::MC17:
        case SCL_gameVersion::MC18:
        case SCL_gameVersion::MC19:
          MapFile.writeListHead("banners", NBT::Compound, 0);
          MapFile.writeListHead("frames", NBT::Compound, 0);
          MapFile.writeString("dimension", "minecraft:overworld");
          MapFile.writeByte("locked", 1);
          break;
        default:
          cerr << "Wrong game version!\n";
          this->reportError(this->wind, errorFlag::UNKNOWN_MAJOR_GAME_VERSION,
                            "Unknown major game version!");
          failed_file_list.emplace_back(current_filename);
          continue;
        }

        MapFile.writeByteArrayHead("colors", 16384);
        {
          uchar ColorCur = 0;
          for (short rr = 0; rr < 128; rr++) {
            for (short cc = 0; cc < 128; cc++) {
              if (rr + offset[0] < mapPic.rows() &&
                  cc + offset[1] < mapPic.cols())
                ColorCur = mapPic(rr + offset[0], cc + offset[1]);
              else
                ColorCur = 0;
              MapFile.writeByte("this should never be seen", ColorCur);
            }
            progressAdd(wind, 1);
          }
        }
      }
      MapFile.endCompound();
      MapFile.close();

      currentIndex++;
    }
  }

  reportWorkingStatue(wind, workStatues::none);

  return failed_file_list;
}
