#ifndef SLOPECRAFT_VISUALCRAFTL_TOKIVC_H
#define SLOPECRAFT_VISUALCRAFTL_TOKIVC_H

#include "VisualCraftL.h"

#include "ParseResourcePack.h"
#include "Resource_tree.h"

class TokiVC : public VCL_Kernel {
public:
  TokiVC() = default;
  virtual ~TokiVC() = default;

  VCL_Kernel_step step() const noexcept override { return this->_step; }

  bool set_resource_pack(const VCL_resource_pack *const rp) noexcept override;

  const VCL_resource_pack *resource_pack() const noexcept override;

private:
  VCL_Kernel_step _step{VCL_Kernel_step::VCL_none};

  VCL_resource_pack pack;
};

#endif // SLOPECRAFT_VISUALCRAFTL_TOKIVC_H