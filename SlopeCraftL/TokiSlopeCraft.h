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
#include <span>

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

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

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
  // 构建三维结构
  bool build(compressSettings cs = SCL_compressSettings::noCompress,
             unsigned short mah = 256,
             glassBridgeSettings gbs = SCL_glassBridgeSettings::noBridge,
             unsigned short gi = 3, bool fp = false, bool ep = false) override {
    return this->build(build_options{SC_VERSION_U64, mah, gi, cs, gbs, fp, ep});
  }

  bool build(const build_options &option) noexcept override;

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
  std::string exportAsLitematic(std::string_view TargetName,  // Local
                                std::string_view LiteName,    // Utf8
                                std::string_view RegionName   // Utf8
  ) const;
  bool exportAsLitematic(
      const char *filename_local,
      const litematic_options &option) const noexcept override {
    auto err = this->exportAsLitematic(filename_local, option.litename_utf8,
                                       option.region_name_utf8);
    return err.empty();
  }

  bool exportAsStructure(
      const char *filename_local,
      const vanilla_structure_options &option) const noexcept override {
    auto err =
        this->exportAsStructure(filename_local, option.is_air_structure_void);
    return err.empty();
  }

  bool exportAsWESchem(const char *filename_local,
                       const WE_schem_options &option) const noexcept override {
    const bool have_req = option.num_required_mods > 0 &&
                          option.required_mods_name_utf8 != nullptr;
    std::span<const char *const> sp{};

    if (have_req) {
      sp = {option.required_mods_name_utf8,
            option.num_required_mods + option.required_mods_name_utf8};
    }
    auto err = this->exportAsWESchem(filename_local, option.offset,
                                     option.we_offset, "", sp);

    return err.empty();
  }

  void exportAsStructure(const char *TargetName, char *FileName) const override;
  std::string exportAsStructure(std::string_view filename,
                                bool is_air_structure_void) const;

  void exportAsWESchem(const char *fileName, const int (&offset)[3],
                       const int (&weOffset)[3], const char *Name,
                       const char *const *const requiredMods,
                       const int requiredModsCount,
                       char *returnVal) const override;

  std::string exportAsWESchem(std::string_view filename,
                              std::span<const int, 3> offset,
                              std::span<const int, 3> weOffset,
                              std::string_view Name,
                              std::span<const char *const> requiredMods) const;

  bool exportAsFlatDiagram(
      const char *filename_local,
      const flag_diagram_options &option) const noexcept override {
    std::string err = this->export_flat_diagram(filename_local, option);
    if (option.err != nullptr) {
      write(*option.err, err);
    }
    return err.empty();
  }

  std::string export_flat_diagram(
      std::string_view filename,
      const flag_diagram_options &option) const noexcept;

  void get3DSize(int *x, int *y, int *z) const override;
  int getHeight() const override;
  int getXRange() const override;
  int getZRange() const override;

  void getBlockCounts(int *total, int detail[64]) const override;
  int64_t getBlockCounts(std::vector<int64_t> *) const;
  int64_t getBlockCounts() const override;

  const uint16_t *getBuild(int *xSize, int *ySize, int *zSize) const override;

  // const Eigen::Tensor<uchar, 3> &getBuild() const;

 private:
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
  /*
  Eigen::ArrayXXi Base;
  Eigen::ArrayXXi HighMap;
  Eigen::ArrayXXi LowMap;
  std::unordered_map<TokiPos, waterItem> WaterList;
  */

  build_options build_opt;
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
  void makeHeight_new(Eigen::ArrayXXi &Base, Eigen::ArrayXXi &HighMap,
                      Eigen::ArrayXXi &LowMap,
                      std::unordered_map<TokiPos, waterItem> &WaterList);
  // void makeHeightInLine(const uint16_t c);
  void buildHeight(
      bool fireProof, bool endermanProof, const Eigen::ArrayXXi &Base,
      const Eigen::ArrayXXi &HighMap, const Eigen::ArrayXXi &LowMap,
      const std::unordered_map<TokiPos, waterItem> &WaterList);  // 构建 Build
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

  bool saveConvertCache(StringDeliver &_err) const noexcept override {
    std::string err;
    this->save_convert_cache(err);
    write(_err, err);
    return err.empty();
  }

  bool loadConvertCache(SCL_convertAlgo algo, bool dither) noexcept override {
    return this->load_convert_cache(algo, dither);
  }

  int getSchemPalette(const char **dest_id,
                      size_t dest_capacity) const noexcept override;

  bool saveBuildCache(StringDeliver &_err) const noexcept override {
    std::string err;
    this->save_build_cache(err);
    write(_err, err);
    return err.empty();
  }

  bool loadBuildCache(const build_options &option) noexcept override {
    return this->load_build_cache(option);
  }

 private:
  // determined by mcver, map type and allowed colorest
  static std::vector<uint8_t> type_hash() noexcept;
  static std::string type_dir_of(std::string_view root_cache_dir) noexcept;
  inline std::string current_type_dir() const noexcept {
    return type_dir_of(this->cache_dir.value());
  }
  std::string type_hash_filename() const noexcept;
  std::string save_type_hash() const noexcept;
  bool check_type_hash() const noexcept;

  // determined by image, convert algo and dither
  static std::string convert_dir(std::string_view type_dir,
                                 uint64_t task_hash) noexcept;
  std::string convert_dir_of(SCL_convertAlgo algo, bool dither) const noexcept;
  std::string current_convert_dir() const noexcept;
  static std::string convert_cache_filename_of(
      std::string_view convert_dir) noexcept;
  std::string current_convert_cache_filename() const noexcept;
  void save_convert_cache(std::string &err) const noexcept;
  bool load_convert_cache(SCL_convertAlgo algo, bool dither) noexcept;

  // determined by mapPic, palette from type and build_options
  static std::vector<uint8_t> build_task_hash_of(
      const Eigen::ArrayXXi &mapPic, std::span<std::string_view> blkids,
      const build_options &opt) noexcept;
  std::vector<uint8_t> current_build_task_hash() const noexcept;
  static std::string build_dir_of(std::string_view convert_dir,
                                  uint64_t short_hash) noexcept;
  std::string current_build_dir() const noexcept;
  static std::string build_cache_filename_of(
      std::string_view build_dir) noexcept;
  std::string current_build_cache_filename() const noexcept;
  void save_build_cache(std::string &err) const noexcept;
  struct build_cache_ir {
    Eigen::ArrayXXi mapPic;
    build_options build_option;
    libSchem::Schem schem;
  };
  static std::string build_hash_filename_of(
      std::string_view build_dir) noexcept;
  bool exmaine_build_cache(const build_options &opt,
                           std::span<const uint8_t> expected_sha3_512,
                           build_cache_ir *ir) const noexcept;
  bool load_build_cache(const build_options &opt) noexcept;

 private:
  std::vector<std::string_view> schem_block_id_list() const noexcept;

  static std::string build_task_dir(std::string_view cvt_task_dir,
                                    uint64_t build_task_hash) noexcept;
  std::string build_task_dir() const noexcept;
  static std::string build_cache_filename(
      std::string_view build_task_dir) noexcept;

  static bool exmaine_build_cache(std::string_view filename,
                                  uint64_t build_task_hash,
                                  build_cache_ir *ir = nullptr) noexcept;

  static const simpleBlock *find_block_for_idx(int idx,
                                               std::string_view blkid) noexcept;
};

// bool compressFile(const char *sourcePath, const char *destPath);
#endif  // TOKISLOPECRAFT_H
