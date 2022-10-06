#ifndef SLOPECRAFT_VISUALCRAFTL_TOKIVC_H
#define SLOPECRAFT_VISUALCRAFTL_TOKIVC_H

#include "VisualCraftL.h"
#include "VisualCraft_classes.h"

class TokiVC : public VCL_Kernel {
public:
  TokiVC() = default;
  virtual ~TokiVC() = default;

  VCL_Kernel_step step() const noexcept override {
    return VCL_Kernel_step::VCL_none;
  }
};

#endif // SLOPECRAFT_VISUALCRAFTL_TOKIVC_H