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

#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>
#include <stddef.h>
//#define SCL_CAPI

#include "SlopeCraftL_global.h"

// define enumerations first
#ifdef SCL_FIND_GlobalEnums_BY_PATH
#include "../utilities/SC_GlobalEnums.h"
#else
#include "SC_GlobalEnums.h"
#endif

// declare classes
#ifdef SCL_CAPI
struct AbstractBlock;
struct AiCvterOpt;
struct Kernel;

#else
namespace SlopeCraft {
class AbstractBlock;
struct AiCvterOpt;
class Kernel;
} // namespace SlopeCraft
#endif

// define exported C functions
#ifdef SCL_CAPI
extern "C" {
SCL_EXPORT AbstractBlock *SCL_createBlock();
unsigned long long SCL_EXPORT SCL_blockSize(const AbstractBlock *);

/// id of this block
const char *SCL_EXPORT SCL_getId(const AbstractBlock *);

/// first version
unsigned char SCL_EXPORT SCL_getVersion(const AbstractBlock *);
/// id in 1.12
const char *SCL_EXPORT SCL_getIdOld(const AbstractBlock *);
/// if this block needs a glass block under it
bool SCL_EXPORT SCL_getNeedGlass(const AbstractBlock *);
/// if this block emits light
bool SCL_EXPORT SCL_getDoGlow(const AbstractBlock *);
/// if this block can be stolen by enderman
bool SCL_EXPORT SCL_getEndermanPickable(const AbstractBlock *);
/// if this block can be burnt
bool SCL_EXPORT SCL_getBurnable(const AbstractBlock *);
// if this block can be used in wall-map
// bool SCL_EXPORT SCL_getWallUseable(const AbstractBlock *);

/// set block id
void SCL_EXPORT SCL_setId(AbstractBlock *, const char *);
/// set first version
void SCL_EXPORT SCL_setVersion(AbstractBlock *, unsigned char);
/// set id in 1.12
void SCL_EXPORT SCL_setIdOld(AbstractBlock *, const char *);
/// set if this block needs a glass block under it
void SCL_EXPORT SCL_setNeedGlass(AbstractBlock *, bool);
/// set if this block emits light
void SCL_EXPORT SCL_setDoGlow(AbstractBlock *, bool);
/// set if this block can be stolen by enderman
void SCL_EXPORT SCL_setEndermanPickable(AbstractBlock *, bool);
/// set if this block can be burnt
void SCL_EXPORT SCL_setBurnable(AbstractBlock *, bool);
// set if this block can be used in wall-map
// void SCL_EXPORT SCL_setWallUseable(AbstractBlock *, bool);
/// let *b equal to *this
void SCL_EXPORT SCL_copyTo(AbstractBlock *, AbstractBlock *b);
/// set this block to air
void SCL_EXPORT SCL_clear(AbstractBlock *);
/// replacement for operator delete
void SCL_EXPORT SCL_destroyBlock(AbstractBlock *);

/// create a Kernel object
Kernel *SCL_EXPORT SCL_createKernel();

void SCL_EXPORT SCL_getColorMapPtrs(const float **const rdata,
                                    const float **const gdata,
                                    const float **const bdata,
                                    const unsigned char **, int *);
// full palette
const float *SCL_EXPORT SCL_getBasicColorMapPtrs();

const char *SCL_EXPORT SCL_getSCLVersion();

unsigned long long SCL_EXPORT SCL_mcVersion2VersionNumber(SCL_gameVersion);

// can do in nothing:
/// real size of Kernel
unsigned long long SCL_EXPORT SCL_kernelSize(Kernel *k);
/// get current step
SCL_step SCL_EXPORT SCL_queryStep(const Kernel *k);
/// revert to a previous step
void SCL_EXPORT SCL_decreaseStep(Kernel *k, SCL_step);
/// replacement for operator delete
void SCL_EXPORT SCL_destroyKernel(Kernel *k);

void SCL_EXPORT SCL_setAiCvterOpt(Kernel *k, const AiCvterOpt *a);
const AiCvterOpt *SCL_EXPORT SCL_getAiCvterOpt(const Kernel *k);

/// function ptr to window object
void SCL_EXPORT SCL_setWindPtr(Kernel *k, void *);
/// a function ptr to show progress of converting and exporting
void SCL_EXPORT SCL_setProgressRangeSet(Kernel *k, void (*)(void *, int min,
                                                            int max, int val));
/// a function ptr to add progress value
void SCL_EXPORT SCL_setProgressAdd(Kernel *k, void (*)(void *, int));
/// a function ptr to prevent window from being syncoped
void SCL_EXPORT SCL_setKeepAwake(Kernel *k, void (*)(void *));

/// a function ptr to show progress of compressing and bridge-building
void SCL_EXPORT SCL_setAlgoProgressRangeSet(Kernel *k,
                                            void (*)(void *, int, int, int));
/// a function ptr to add progress value of compressing and bridge-building
void SCL_EXPORT SCL_setAlgoProgressAdd(Kernel *k, void (*)(void *, int));

/// a function ptr to report error when something wrong happens
void SCL_EXPORT SCL_setReportError(Kernel *k, void (*)(void *, SCL_errorFlag,
                                                       const char *));
/// a function ptr to report working statue especially when busy
void SCL_EXPORT SCL_setReportWorkingStatue(Kernel *k,
                                           void (*)(void *, SCL_workStatues));

// can do in colorSetReady:
/// set map type and blocklist
bool SCL_EXPORT SCL_setType(Kernel *k, SCL_mapTypes, SCL_gameVersion,
                            const bool[64], const AbstractBlock *[64]);
/// get palette (base colors only) in ARGB32
void SCL_EXPORT SCL_getBaseColorsInARGB32(const Kernel *k, unsigned int *);

// can do in wait4Image:
/// set original image from ARGB32 matrix (col-major)
void SCL_EXPORT SCL_setRawImage(Kernel *k, const unsigned int *src, int rows,
                                int cols);
/// get accessible color count
unsigned short SCL_EXPORT SCL_getColorCount(const Kernel *k);
/// make a structure that includes all accessible blocks
void SCL_EXPORT SCL_makeTests(Kernel *k, const AbstractBlock **,
                              const unsigned char *, const char *, char *);
/// get avaliable colors in argb32
void SCL_EXPORT getAvailableColors(const Kernel *k,
                                   unsigned int *const dest_ARGB32,
                                   unsigned char *const dest_map_color,
                                   int *const num);

// can do in convertionReady:
/// convert original image to map
bool SCL_EXPORT SCL_convert(Kernel *k, SCL_convertAlgo = RGB_Better,
                            bool dither = false);
/// get image rows
int SCL_EXPORT SCL_getImageRows(const Kernel *k);
/// get image cols
int SCL_EXPORT SCL_getImageCols(const Kernel *k);
/// query if map is buildable in vanilla survival
bool SCL_EXPORT SCL_isVanilla(const Kernel *k);
/// query if map is a flat one
bool SCL_EXPORT SCL_isFlat(const Kernel *k);

// can do in converted:
/// construct 3D structure
bool SCL_EXPORT SCL_build(Kernel *k, SCL_compressSettings = noCompress,
                          unsigned short = 256,
                          SCL_glassBridgeSettings = noBridge,
                          unsigned short = 3, bool fireProof = false,
                          bool endermanProof = false);

/// get converted image
void SCL_EXPORT SCL_getConvertedImage(const Kernel *k, int *rows, int *cols,
                                      unsigned int *dest);
/// export as map data files
void SCL_EXPORT SCL_exportAsData(const Kernel *k, const char *FolderPath,
                                 const int indexStart, int *fileCount,
                                 char **dest);
/// get converted map(in mapColor array)
void SCL_EXPORT SCL_getConvertedMap(const Kernel *k, int *rows, int *cols,
                                    unsigned char *);

// can do in builded:
/// export map into litematica files (*.litematic)
void SCL_EXPORT SCL_exportAsLitematic(const Kernel *k, const char *TargetName,
                                      const char *LiteName,
                                      const char *RegionName, char *FileName);
/// export map into Structure files (*.NBT)
void SCL_EXPORT SCL_exportAsStructure(const Kernel *k, const char *TargetName,
                                      char *FileName);

/// get x,y,z size
void SCL_EXPORT SCL_get3DSize(const Kernel *k, int *x, int *y, int *z);

/// get 3d structure's size
int SCL_EXPORT SCL_getHeight(const Kernel *k);
/// get 3d structure's size
int SCL_EXPORT SCL_getXRange(const Kernel *k);
/// get 3d structure's size
int SCL_EXPORT SCL_getZRange(const Kernel *k);
/// get block count in total and in detail
void SCL_EXPORT SCL_getBlockCountsInTypes(const Kernel *k, int *total,
                                          int detail[64]);
/// get sum block count
int SCL_EXPORT SCL_getBlockCounts(const Kernel *k);
/// get 3d structure in 3d-matrix (col major)
const unsigned short *SCL_EXPORT SCL_getBuild(const Kernel *k, int *xSize,
                                              int *ySize, int *zSize);

} //  extern "C"
#endif //  ifdef SCL_CAPI

// expand definations of AbstractBlock and Kernel if it is cpp api
#ifndef SCL_CAPI
#include "defines_of_extern_classes.h"
#endif //  ifndef SLOPECRAFT_CAPI

// these functions are
extern "C" {
#ifndef SCL_CAPI
namespace SlopeCraft {
#endif

SCL_EXPORT AiCvterOpt *SCL_createAiCvterOpt();
void SCL_EXPORT SCL_destroyAiCvterOpt(AiCvterOpt *);

void SCL_EXPORT SCL_setPopSize(AiCvterOpt *, unsigned int p);
void SCL_EXPORT SCL_setMaxGeneration(AiCvterOpt *, unsigned int p);
void SCL_EXPORT SCL_setMaxFailTimes(AiCvterOpt *, unsigned int p);
void SCL_EXPORT SCL_setCrossoverProb(AiCvterOpt *, double p);
void SCL_EXPORT SCL_setMutationProb(AiCvterOpt *, double p);

unsigned int SCL_EXPORT SCL_getPopSize(const AiCvterOpt *);
unsigned int SCL_EXPORT SCL_getMaxGeneration(const AiCvterOpt *);
unsigned int SCL_EXPORT SCL_getMaxFailTimes(const AiCvterOpt *);
double SCL_EXPORT SCL_getCrossoverProb(const AiCvterOpt *);
double SCL_EXPORT SCL_getMutationProb(const AiCvterOpt *);

void SCL_EXPORT
SCL_preprocessImage(unsigned int *ARGB32ptr, const uint64_t imageSize,
                    const SCL_PureTpPixelSt = ReplaceWithBackGround,
                    const SCL_HalfTpPixelSt = ComposeWithBackGround,
                    unsigned int backGround = 0xFFFFFFFF);

unsigned char SCL_EXPORT SCL_maxAvailableVersion();

#ifndef SCL_CAPI
} //  namespace SlopeCraft
#endif

} //  extern "C"

// void SCL_EXPORT SCL_test();

#endif // KERNEL_H
