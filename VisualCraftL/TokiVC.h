#ifndef SLOPECRAFT_VISUALCRAFTL_TOKIVC_H
#define SLOPECRAFT_VISUALCRAFTL_TOKIVC_H

#include "BlockStateList.h"
#include "ParseResourcePack.h"
#include "VisualCraftL.h"

class TokiVC : public VCL_Kernel {
public:
  TokiVC() = default;
  virtual ~TokiVC() = default;

  VCL_Kernel_step step() const noexcept override { return this->_step; }

  bool set_resource(const VCL_resource_pack *const rp,
                    const VCL_block_state_list *const bsl) noexcept override;

  bool set_resource_move(VCL_resource_pack **rp_ptr,
                         VCL_block_state_list **bsl_ptr) noexcept override;

  const VCL_resource_pack *resource_pack() const noexcept override;

  const VCL_block_state_list *block_state_list() const noexcept override;

private:
  VCL_Kernel_step _step{VCL_Kernel_step::VCL_none};

  VCL_resource_pack pack;
  VCL_block_state_list bsl;
};

#endif // SLOPECRAFT_VISUALCRAFTL_TOKIVC_H