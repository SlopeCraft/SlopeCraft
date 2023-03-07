#include "TokiSlopeCraft.h"
#include <SC_version_buildtime.h>

const char *TokiSlopeCraft::getSCLVersion() { return SC_VERSION_STR; }

void TokiSlopeCraft::getColorMapPtrs(const float **const r,
                                     const float **const g,
                                     const float **const b,
                                     const unsigned char **m, int *rows) {
  if (r != nullptr)
    *r = TokiSlopeCraft::Allowed.rgb_data(0);
  if (g != nullptr)
    *g = TokiSlopeCraft::Allowed.rgb_data(1);
  if (b != nullptr)
    *b = TokiSlopeCraft::Allowed.rgb_data(2);

  if (m != nullptr)
    *m = TokiSlopeCraft::Allowed.map_data();
  if (rows != nullptr)
    *rows = TokiSlopeCraft::Allowed.color_count();
}

const float *TokiSlopeCraft::getBasicColorMapPtrs() {
  return TokiSlopeCraft::Basic.RGB_mat().data();
}

uint64_t TokiSlopeCraft::mcVersion2VersionNumber(SCL_gameVersion g) {
  switch (g) {
  case SCL_gameVersion::ANCIENT:
    return 114514;
  case SCL_gameVersion::MC12:
    return 1631;
  case SCL_gameVersion::MC13:
    return 1976;
  case SCL_gameVersion::MC14:
    return 2230;
  case SCL_gameVersion::MC15:
    return 2230;
  case SCL_gameVersion::MC16:
    return 2586;
  case SCL_gameVersion::MC17:
    return 2730;
  case SCL_gameVersion::MC18:
    return 2865;
  case SCL_gameVersion::MC19:
    return 3105; // 1.19.0
  default:
    return 1919810;
  }
}