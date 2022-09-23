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

#ifndef TOKISLOPECRAFT_H
#define TOKISLOPECRAFT_H
/////////////////////////////

/////////////////////////////

#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

#include "Colors.h"
#include "SCLDefines.h"
#include "WaterItem.h"
#include "simpleBlock.h"
#include <unsupported/Eigen/CXX11/Tensor>


#include "PrimGlassBuilder.h"
#include "lossyCompressor.h"

#include "newNBTWriter.hpp"

#include "AiCvterOpt.h"

#include "../ColorManip/newColorSet.hpp"

#include "../GAConverter/GAConverter.h"

/*
namespace SlopeCraft
{
    void *AllowedRGBList4AiCvters();
    void *AllowedMapList4AiCvters();
    void *BasicalRGBList4AiCvters();
}
*/

using namespace SlopeCraft;
#include <thread>

#define mapColor2Index(mapColor) (64 * (mapColor % 4) + (mapColor / 4))
#define index2mapColor(index) (4 * (index % 64) + (index / 64))
#define mapColor2baseColor(mapColor) (mapColor >> 2)
#define index2baseColor(index) (mapColor2baseColor(index2mapColor(index)))
#define mapColor2depth(mapColor) (mapColor % 4)
#define index2depth(index) (mapColor2depth(index2mapColor(index)))

class PrimGlassBuilder;
class LossyCompressor;

namespace NBT {
class NBTWriter;
};

class TokiSlopeCraft : public Kernel {
public:
  static const colorset_basic_t Basic;
  static colorset_allowed_t Allowed;

public:
  TokiSlopeCraft();
  virtual ~TokiSlopeCraft();

  // can do in nothing:
  void trySkipStep(step);
  /// function ptr to window object
  void setWindPtr(void *);
  /// a function ptr to show progress of converting and exporting
  void setProgressRangeSet(void (*)(void *, int, int, int));
  /// a function ptr to add progress value
  void setProgressAdd(void (*)(void *, int));
  /// a function ptr to prevent window from being syncoped
  void setKeepAwake(void (*)(void *));

  /// a function ptr to show progress of compressing and bridge-building
  void setAlgoProgressRangeSet(void (*)(void *, int, int, int));
  /// a function ptr to add progress value of compressing and bridge-building
  void setAlgoProgressAdd(void (*)(void *, int));

  /// a function ptr to report error when something wrong happens
  void setReportError(void (*)(void *, errorFlag, const char *));
  /// a function ptr to report working statue especially when busy
  void setReportWorkingStatue(void (*)(void *, workStatues));
  unsigned long long size() { return sizeof(TokiSlopeCraft); }
  void destroy() { delete this; }
  void decreaseStep(step);
  void makeTests(const AbstractBlock **, const unsigned char *, const char *,
                 char *);
  std::string makeTests(const AbstractBlock **, const uint8_t *,
                        const std::string &);
  void setAiCvterOpt(const AiCvterOpt *);
  const AiCvterOpt *aiCvterOpt() const;

  // can do in colorSetReady:
  step queryStep() const;

  bool setType(mapTypes, gameVersion, const bool[64],
               const AbstractBlock *[64]);
  bool setType(mapTypes, gameVersion, const bool[64], const simpleBlock[64]);

  void getBaseColorInARGB32(unsigned int *const) const;
  // can do in wait4Image:
  void setRawImage(const unsigned int *src, int rows, int cols);
  void setRawImage(const EImage &);
  uint16_t getColorCount() const;
  void getAvailableColors(ARGB *const, uint8_t *const,
                          int *const num = nullptr) const;
  // can do in convertionReady:
  bool convert(convertAlgo = RGB_Better, bool dither = false);
  int getImageRows() const;
  int getImageCols() const;
  bool isVanilla() const; //判断是可以生存实装的地图画
  bool isFlat() const;    //判断是平板的

  // can do in converted:
  bool build(compressSettings = noCompress, unsigned short = 256,
             glassBridgeSettings = noBridge, unsigned short = 3,
             bool fireProof = false, bool endermanProof = false); //构建三维结构
  void getConvertedImage(int *rows, int *cols, unsigned int *dest) const;
  EImage getConovertedImage() const;
  void getConvertedMap(int *rows, int *cols, unsigned char *) const;
  // void getConvertedMap(Eigen::Arra) const;
  void exportAsData(const char *, const int, int *fileCount, char **) const;
  std::vector<std::string> exportAsData(const std::string &, int) const;
  // can do in builded:
  void exportAsLitematic(const char *TargetName, const char *LiteName,
                         const char *RegionName, char *FileName) const;
  std::string exportAsLitematic(const std::string &TargetName, // Local
                                const std::string &LiteName,   // Utf8
                                const std::string &RegionName  // Utf8
  ) const;

  void exportAsStructure(const char *TargetName, char *FileName) const;
  std::string exportAsStructure(const std::string &) const;

  void exportAsWESchem(const char *fileName, const int (&offset)[3],
                       const int (&weOffset)[3], const char *Name,
                       const char *const *const requiredMods,
                       const int requiredModsCount, char *returnVal) const;

  std::string
  exportAsWESchem(const std::string &, const std::array<int, 3> &offset,
                  const std::array<int, 3> &weOffset, const char *Name,
                  const std::vector<const char *> &requiredMods) const;

  void get3DSize(int *x, int *y, int *z) const;
  int getHeight() const;
  int getXRange() const;
  int getZRange() const;

  void getBlockCounts(int *total, int detail[64]) const;
  int64_t getBlockCounts(std::vector<int64_t> *) const;
  int64_t getBlockCounts() const;

  const unsigned char *getBuild(int *xSize, int *ySize, int *zSize) const;
  const Eigen::Tensor<uchar, 3> &getBuild() const;

private:
#ifdef SCL_CAPI
  friend struct Kernel;
#else
  friend class Kernel;
#endif //  #ifdef SLOPECRAFTL_CAPI
  // friend class TokiColor;
  //  friend void * allowedRGB();
  //  friend void * allowedMap();
  enum ColorSpace { R = 'R', H = 'H', L = 'L', X = 'X' };
  static const Eigen::Array<float, 2, 3> DitherMapLR, DitherMapRL;
  static const uint32_t reportRate = 100;

  void *wind;
  void (*progressRangeSet)(void *, int, int, int);
  void (*progressAdd)(void *, int);
  void (*keepAwake)(void *);
  void (*algoProgressRangeSet)(void *, int, int, int);
  void (*algoProgressAdd)(void *, int);
  void (*reportError)(void *, errorFlag, const char *);
  void (*reportWorkingStatue)(void *, workStatues);

  static gameVersion mcVer; // 12,13,14,15,16,17
  static mapTypes mapType;
  static std::vector<simpleBlock> blockPalette;

  step kernelStep;
  convertAlgo ConvertAlgo;
  compressSettings compressMethod;
  glassBridgeSettings glassMethod;

  std::array<int, 3> size3D; // x,y,z

  EImage rawImage;
  EImage ditheredImage;

  std::unordered_map<ARGB, TokiColor> colorHash;

  uint16_t maxAllowedHeight;
  uint16_t bridgeInterval;
  PrimGlassBuilder *glassBuilder;
  LossyCompressor *Compressor;

  GACvter::GAConverter *GAConverter;

  AiCvterOpt AiOpt;
  Eigen::ArrayXXi mapPic; // stores mapColor
  Eigen::ArrayXXi Base;
  Eigen::ArrayXXi HighMap;
  Eigen::ArrayXXi LowMap;
  std::unordered_map<TokiPos, waterItem> WaterList;
  Eigen::Tensor<uchar, 3> Build; // x,y,z

  // for setType:

  void configGAConverter();

  // for setImage:

  // for convert:
  ColorSpace getColorSpace() const;
  void pushToHash();
  void applyTokiColor();
  void fillMapMat();
  void Dither();
  int64_t sizePic(short dim) const;

  // for build
  void getTokiColorPtr(uint16_t, const TokiColor *[]) const;
  // void makeHeight_old();//构建HighMap和LowMap
  void makeHeight_new();
  // void makeHeightInLine(const uint16_t c);
  void buildHeight(bool = false, bool = false); //构建Build
  void makeBridge();
  // for Litematic
  static void writeBlock(const std::string &netBlockId,
                         const std::vector<std::string> &Property,
                         const std::vector<std::string> &ProVal,
                         NBT::NBTWriter &);
  static void writeTrash(int count, NBT::NBTWriter &);
  std::string Noder(const short *src, int size) const;

  Kernel *toBaseClassPtr() { return this; }
};

bool compressFile(const char *sourcePath, const char *destPath);
#endif // TOKISLOPECRAFT_H
