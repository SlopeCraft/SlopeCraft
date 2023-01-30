#include "TokiVC.h"

bool TokiVC::set_resource_pack(const VCL_resource_pack *const rp) noexcept {
  if (rp == nullptr) {
    return false;
  }
  pack = *rp;

  this->_step = VCL_Kernel_step::VCL_wait_for_image;

  return true;
}

const VCL_resource_pack *TokiVC::resource_pack() const noexcept {
  if (this->_step <= VCL_Kernel_step::VCL_none) {
    return nullptr;
  }

  return &this->pack;
}