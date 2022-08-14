#ifndef DEFINES_OF_EXTERN_CLASSES_H
#define DEFINES_OF_EXTERN_CLASSES_H

#include "SlopeCraftL_global.h"

namespace SlopeCraft {

#ifndef SCL_CAPI
    using AbstractBlock=::SlopeCraft::AbstractBlock;
    using AiCvterOpt=::SlopeCraft::AiCvterOpt;
    using Kernel=::SlopeCraft::Kernel;
#endif  //  #ifndef SCL_CAPI

    using step=::SCL_step;
    using mapTypes=::SCL_mapTypes;
    using compressSettings=::SCL_compressSettings;
    using convertAlgo=::SCL_convertAlgo;
    using glassBridgeSettings=::SCL_glassBridgeSettings;
    using gameVersion=::SCL_gameVersion;
    using workStatues=::SCL_workStatues;
    using errorFlag=::SCL_errorFlag;

}; // namespace SlopeCraft


#ifndef SCL_CAPI
namespace SlopeCraft {
#endif  //  #ifndef SCL_CAPI

SCL_external_class AbstractBlock {
public:
  AbstractBlock();
  // virtual ~AbstractBlock() {};
  /// create a block
  static AbstractBlock *create();
  /// real size of this block
  virtual unsigned long long size() const = 0;
  /// id of this block
  virtual const char *getId() const = 0;
  /// first version
  virtual unsigned char getVersion() const = 0;
  /// id in 1.12
  virtual const char *getIdOld() const = 0;
  /// if this block needs a glass block under it
  virtual bool getNeedGlass() const = 0;
  /// if this block emits light
  virtual bool getDoGlow() const = 0;
  /// if this block can be stolen by enderman
  virtual bool getEndermanPickable() const = 0;
  /// if this block can be burnt
  virtual bool getBurnable() const = 0;

  /// set block id
  virtual void setId(const char *) = 0;
  /// set first version
  virtual void setVersion(unsigned char) = 0;
  /// set id in 1.12
  virtual void setIdOld(const char *) = 0;
  /// set if this block needs a glass block under it
  virtual void setNeedGlass(bool) = 0;
  /// set if this block emits light
  virtual void setDoGlow(bool) = 0;
  /// set if this block can be stolen by enderman
  virtual void setEndermanPickable(bool) = 0;
  /// set if this block can be burnt
  virtual void setBurnable(bool) = 0;
  /// let *b equal to *this
  void copyTo(AbstractBlock *b) const;
  /// set this block to air
  void clear();
  /// replacement for operator delete
  virtual void destroy() = 0;
};

SCL_external_class Kernel {
public:
  Kernel();
  // virtual ~Kernel() {};

  /// create a kernel object
  static Kernel *create();

public:
  static void getColorMapPtrs(const float **, const unsigned char **, int *);
  // full palette
  static const float *getBasicColorMapPtrs();

  static const char *getSCLVersion();

  static unsigned long long mcVersion2VersionNumber(::SCL_gameVersion);

  /// function ptr to window object
  virtual void setWindPtr(void *) = 0;
  /// a function ptr to show progress of converting and exporting
  virtual void setProgressRangeSet(void (*)(void *, int, int, int)) = 0;
  /// a function ptr to add progress value
  virtual void setProgressAdd(void (*)(void *, int)) = 0;
  /// a function ptr to prevent window from being syncoped
  virtual void setKeepAwake(void (*)(void *)) = 0;

  /// a function ptr to show progress of compressing and bridge-building
  virtual void setAlgoProgressRangeSet(void (*)(void *, int, int, int)) = 0;
  /// a function ptr to add progress value of compressing and bridge-building
  virtual void setAlgoProgressAdd(void (*)(void *, int)) = 0;

  /// a function ptr to report error when something wrong happens
  virtual void setReportError(void (*)(void *, ::SCL_errorFlag, const char *)) = 0;
  /// a function ptr to report working statue especially when busy
  virtual void setReportWorkingStatue(void (*)(void *, ::SCL_workStatues)) = 0;

  virtual void setAiCvterOpt(const AiCvterOpt *) = 0;

  virtual const AiCvterOpt *aiCvterOpt() const = 0;

  // can do in nothing:
  /// real size of kernel
  virtual unsigned long long size() = 0;
  /// revert to a previous step
  virtual void decreaseStep(::SCL_step) = 0;
  /// replacement for operator delete
  virtual void destroy() = 0;

  // can do in colorSetReady:
  /// get current step
  virtual ::SCL_step queryStep() const = 0;
  /// set map type and blocklist
  virtual bool setType(::SCL_mapTypes, ::SCL_gameVersion, const bool[64],
                       const AbstractBlock *[64]) = 0;
  /// get palette (base colors only) in ARGB32
  virtual void getARGB32(unsigned int *) const = 0;

  // can do in wait4Image:
  /// set original image from ARGB32 matrix (col-major)
  virtual void setRawImage(const unsigned int *src, int rows, int cols) = 0;
  /// get accessible color count
  virtual unsigned short getColorCount() const = 0;
  /// make a structure that includes all accessible blocks
  virtual void makeTests(const AbstractBlock **, const unsigned char *,
                         const char *, char *) = 0;

  // can do in convertionReady:
  /// convert original image to map
  virtual bool convert(::SCL_convertAlgo = ::SCL_convertAlgo::RGB_Better, bool dither = false) = 0;
  /// get image rows
  virtual int getImageRows() const = 0;
  /// get image cols
  virtual int getImageCols() const = 0;
  /// query if map is buildable in vanilla survival
  virtual bool isVanilla() const = 0;
  /// query if map is a flat one
  virtual bool isFlat() const = 0;

  // can do in converted:
  /// construct 3D structure
  virtual bool build(::SCL_compressSettings = ::SCL_compressSettings::noCompress, unsigned short = 256,
                     ::SCL_glassBridgeSettings = ::SCL_glassBridgeSettings::noBridge, unsigned short = 3,
                     bool fireProof = false, bool endermanProof = false) = 0;

  /// get converted image
  virtual void getConvertedImage(int *rows, int *cols,
                                 unsigned int *dest) const = 0;
  /// export as map data files
  virtual void exportAsData(const char *FolderPath, const int indexStart,
                            int *fileCount, char **dest) const = 0;
  /// get converted map(in mapColor array)
  virtual void getConvertedMap(int *rows, int *cols,
                               unsigned char *) const = 0;

  // can do in builded:
  /// export map into litematica files (*.litematic)
  virtual void exportAsLitematic(const char *TargetName, const char *LiteName,
                                 const char *author, const char *RegionName,
                                 char *FileName) const = 0;
  /// export map into Structure files (*.NBT)
  virtual void exportAsStructure(const char *TargetName,
                                 char *FileName) const = 0;

  /// get x,y,z size
  virtual void get3DSize(int *x, int *y, int *z) const = 0;

  /// get 3d structure's size
  virtual int getHeight() const = 0;
  /// get 3d structure's size
  virtual int getXRange() const = 0;
  /// get 3d structure's size
  virtual int getZRange() const = 0;
  /// get block count in total and in detail
  virtual void getBlockCounts(int *total, int detail[64]) const = 0;
  /// get sum block count
  virtual long long int getBlockCounts() const = 0;
  /// get 3d structure in 3d-matrix (col major)
  virtual const unsigned char *getBuild(int *xSize = nullptr,
                                        int *ySize = nullptr,
                                        int *zSize = nullptr) const = 0;

protected:
  /// calling delete is deprecated, use void Kernel::destroy() instead
  void operator delete(void *) {}
};

#ifndef SCL_CAPI
} // namespace SlopeCraft
#endif

#endif // DEFINES_OF_EXTERN_CLASSES_H
