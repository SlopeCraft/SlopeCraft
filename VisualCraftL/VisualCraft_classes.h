#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_CLASSES_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_CLASSES_H

#ifdef VISUALCRAFTL_BUILD
#include <utilities/SC_GlobalEnums.h>
#else
#include "SC_GlobalEnums.h"
#endif

struct VCL_Kernel {
public:
  VCL_Kernel() = default;
  virtual ~VCL_Kernel() = default;

  virtual VCL_Kernel_step step() const noexcept = 0;
};

#endif // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_CLASSES_H