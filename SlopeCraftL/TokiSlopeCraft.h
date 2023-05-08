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

#ifndef TOKISLOPECRAFT_H
#define TOKISLOPECRAFT_H
/////////////////////////////

/////////////////////////////

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
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

#include <NBTWriter/NBTWriter.h>

#include "AiCvterOpt.h"

#include <ColorManip/imageConvert.hpp>
#include <ColorManip/newColorSet.hpp>
#include <ExternalConverters/GAConverter/GAConverter.h>
#include <MapImageCvter/MapImageCvter.h>
#include <Schem/Schem.h>
#include "WriteStringDeliver.h"

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

#define mapColor2Index(mapColor) (64 * ((mapColor) % 4) + ((mapColor) / 4))
#define index2mapColor(index) (4 * ((index) % 64) + ((index) / 64))
#define mapColor2baseColor(mapColor) ((mapColor) >> 2)
#define index2baseColor(index) (mapColor2baseColor(index2mapColor(index)))
#define mapColor2depth(mapColor) ((mapColor) % 4)
#define index2depth(index) (mapColor2depth(index2mapColor(index)))

class PrimGlassBuilder;
class LossyCompressor;

namespace NBT {
template <bool>
class NBTWriter;
};

class TokiSlopeCraft : public ::SlopeCraft::Kernel {
 public:
  static const colorset_basic_t Basic;
  static colorset_allowed_t Allowed;

  static void getColorMapPtrs(const float **const rdata,
                              const float **const gdata,
                              const float **const bdata, const uint8_t **,
                              int *);
  // full palette
  static const float *getBasicColorMapPtrs();

  static const char *getSCLVersion();

  static uint64_t mcVersion2VersionNumber(::SCL_gameVersion);

 public:
  TokiSlopeCraft();
  virtual ~TokiSlopeCraft();

  // can do in nothing:
  void trySkipStep(step);
  /// function ptr to window object
  void setWindPtr(void *) override;
  /// a function ptr to show progress of converting and exporting
  void setProgressRangeSet(void (*)(void *, int, int, int)) override;
  /// a function ptr to add progress value
  void setProgressAdd(void (*)(void *, int)) override;
  /// a function ptr to prevent window from being syncoped
  void setKeepAwake(void (*)(void *)) override;

  /// a function ptr to show progress of compressing and bridge-building
  void setAlgoProgressRangeSet(void (*)(void *, int, int, int)) override;
  /// a function ptr to add progress value of compressing and bridge-building
  void setAlgoProgressAdd(void (*)(void *, int)) override;

  /// a function ptr to report error when something wrong happens
  void setReportError(void (*)(void *, errorFlag, const char *)) override;
  /// a function ptr to report working statue especially when busy
  void setReportWorkingStatue(void (*)(void *, workStatues)) override;
  unsigned long long size() override { return sizeof(TokiSlopeCraft); }
  // void destroy() override { delete this; }
  void decreaseStep(step) override;
  bool makeTests(const AbstractBlock **, const unsigned char *, const char *,
                 char *) override;
  std::string makeTests(const AbstractBlock **, const uint8_t *,
                        const std::string &);
  void setAiCvterOpt(const AiCvterOpt *) override;
  const AiCvterOpt *aiCvterOpt() const override;

  // can do in colorSetReady:
  step queryStep() const override;

  bool setType(mapTypes, gameVersion, const bool[64],
               const AbstractBlock *const *const) override;

 private:
  static bool __impl_setType(mapTypes, gameVersion, const bool[64],
                             const AbstractBlock *const *const,
                             const TokiSlopeCraft *reporter) noexcept;

 public:
  void getBaseColorInARGB32(unsigned int *const) const override;
  // can do in wait4Image:
  void setRawImage(const unsigned int *src, int rows, int cols) override {
    this->setRawImage(src, rows, cols, true);
  }

  void setRawImage(const uint32_t *src, int rows, int cols,
                   bool is_col_major) override;

  uint16_t getColorCount() const override;
  void getAvailableColors(ARGB *const, uint8_t *const,
                          int *const num = nullptr) const override;
  // can do in convertionReady:
  bool convert(convertAlgo = SCL_convertAlgo::RGB_Better,
               bool dither = false) override;
  const uint32_t *getRawImage() const override {
    return this->image_cvter.raw_image().data();
  }
  int getImageRows() const override;
  int getImageCols() const override;

  bool isVanilla() const override { return is_vanilla_static(); }
  static inline bool is_vanilla_static() noexcept {
    return mapType != SCL_mapTypes::FileOnly;
  }

  bool isFlat() const override { return is_flat_static(); }
  static inline bool is_flat_static() noexcept {
    return mapType == SCL_mapTypes::Flat;
  }

  // can do in converted:
  bool build(compressSettings = SCL_compressSettings::noCompress,
             unsigned short = 256,
             glassBridgeSettings = SCL_glassBridgeSettings::noBridge,
             unsigned short = 3, bool fireProof = false,
             bool endermanProof = false) override;  // 构建三维结构
  void getConvertedImage(int *rows, int *cols,
                         unsigned int *dest) const override {
    this->getConvertedImage(rows, cols, dest, true);
  }
  void getConvertedImage(int *rows, int *cols, uint32_t *dest,
                         bool expected_col_major) const override;
  EImage getConovertedImage() const;
  void getConvertedMap(int *rows, int *cols, unsigned char *) const override;
  // void getConvertedMap(Eigen::Arra) const;
  void exportAsData(const char *, const int, int *fileCount,
                    char **) const override;
  std::vector<std::string> exportAsData(std::string, int) const;
  // can do in builded:
  void exportAsLitematic(const char *TargetName, const char *LiteName,
                         const char *RegionName, char *FileName) const override;
  std::string exportAsLitematic(const std::string &TargetName,  // Local
                                const std::string &LiteName,    // Utf8
                                const std::string &RegionName   // Utf8
  ) const;

  void exportAsStructure(const char *TargetName, char *FileName) const override;
  std::string exportAsStructure(const std::string &) const;

  void exportAsWESchem(const char *fileName, const int (&offset)[3],
                       const int (&weOffset)[3], const char *Name,
                       const char *const *const requiredMods,
                       const int requiredModsCount,
                       char *returnVal) const override;

  std::string exportAsWESchem(
      const std::string &, const std::array<int, 3> &offset,
      const std::array<int, 3> &weOffset, const char *Name,
      const std::vector<const char *> &requiredMods) const;

  void get3DSize(int *x, int *y, int *z) const override;
  int getHeight() const override;
  int getXRange() const override;
  int getZRange() const override;

  void getBlockCounts(int *total, int detail[64]) const override;
  int64_t getBlockCounts(std::vector<int64_t> *) const;
  int64_t getBlockCounts() const override;

  const unsigned short *getBuild(int *xSize, int *ySize,
                                 int *zSize) const override;

  // const Eigen::Tensor<uchar, 3> &getBuild() const;

 private:
#ifdef SCL_CAPI
  friend struct Kernel;
#else
  friend class Kernel;
#endif  //  #ifdef SLOPECRAFTL_CAPI
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

  static gameVersion mcVer;  // 12,13,14,15,16,17
  static mapTypes mapType;
  static std::vector<simpleBlock> blockPalette;

 private:
  static std::unordered_set<TokiSlopeCraft *> kernel_hash_set;

 public:
  step kernelStep;
  // convertAlgo ConvertAlgo;

  libMapImageCvt::MapImageCvter image_cvter;

  // std::array<int, 3> size3D; // x,y,z
  PrimGlassBuilder *glassBuilder;
  LossyCompressor *Compressor;

  // std::shared_ptr<GACvter::GAConverter> GAConverter{nullptr};

  AiCvterOpt AiOpt;
  Eigen::ArrayXXi mapPic;  // stores mapColor
  Eigen::ArrayXXi Base;
  Eigen::ArrayXXi HighMap;
  Eigen::ArrayXXi LowMap;
  std::unordered_map<TokiPos, waterItem> WaterList;
  uint16_t maxAllowedHeight;
  uint16_t bridgeInterval;
  compressSettings compressMethod;
  glassBridgeSettings glassMethod;
  libSchem::Schem schem;

  std::optional<std::string> cache_dir{""};
  // Eigen::Tensor<uchar, 3> Build; // x,y,z

  // for setType:

  // for setImage:

  // for convert:
  ColorSpace getColorSpace() const;
  int64_t sizePic(short dim) const;

  // for build
  // void makeHeight_old();//构建 HighMap 和 LowMap
  void makeHeight_new();
  // void makeHeightInLine(const uint16_t c);
  void buildHeight(bool = false, bool = false);  // 构建 Build
  void makeBridge();
  // for Litematic
  /*
  static void writeBlock(const std::string &netBlockId,
                         const std::vector<std::string> &Property,
                         const std::vector<std::string> &ProVal,
                         NBT::NBTWriter<false> &);
  static void writeTrash(int count, NBT::NBTWriter<false> &);
  */
  std::string Noder(const short *src, int size) const;

  Kernel *toBaseClassPtr() { return this; }

  void setCacheDir(const char *) noexcept override;
  const char *cacheDir() const noexcept override;

  bool saveCache(StringDeliver &_err) const noexcept override {
    std::string err;
    this->saveCache(err);
    write(_err, err);
    return err.empty();
  }

  void saveCache(std::string &err) const noexcept;

 private:
  std::string task_dir() const noexcept;
  std::string task_dir(uint64_t) const noexcept;
  std::string task_dir(SCL_convertAlgo algo, bool dither) const noexcept;
  std::string colorset_hash_file() const noexcept;

  bool check_coloset_hash_by_filename(std::string_view filename) const noexcept;

  static std::string conevrt_cache_filename(std::string_view taskdir) noexcept;
  static std::string cache_owned_hash_filename(
      std::string_view taskdir) noexcept;
  bool check_cache_owned_hash(uint64_t task_hash) const noexcept;

 public:
  bool check_colorset_hash() const noexcept override;
  bool load_convert_cache(SCL_convertAlgo algo, bool dither) noexcept override;
};

// bool compressFile(const char *sourcePath, const char *destPath);
#endif  // TOKISLOPECRAFT_H
