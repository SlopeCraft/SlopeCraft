#include "TokiVC.h"

bool TokiVC::set_resource(const VCL_resource_pack *const rp,
                          const VCL_block_state_list *const bsl) noexcept {
  if (rp == nullptr || bsl == nullptr) {
    return false;
  }
  this->pack = *rp;

  this->bsl = *bsl;

  this->_step = VCL_Kernel_step::VCL_wait_for_image;

  return true;
}

bool TokiVC::set_resource_move(VCL_resource_pack **rp_ptr,
                               VCL_block_state_list **bsl_ptr) noexcept {
  if (rp_ptr == nullptr || bsl_ptr == nullptr) {
    return false;
  }

  if (*rp_ptr == nullptr || *bsl_ptr == nullptr) {
    return false;
  }

  this->pack = std::move(**rp_ptr);
  this->bsl = std::move(**bsl_ptr);

  VCL_destroy_block_state_list(*bsl_ptr);
  *bsl_ptr = nullptr;

  VCL_destroy_resource_pack(*rp_ptr);
  *rp_ptr = nullptr;

  return true;
}

const VCL_resource_pack *TokiVC::resource_pack() const noexcept {
  if (this->_step <= VCL_Kernel_step::VCL_none) {
    return nullptr;
  }

  return &this->pack;
}

const VCL_block_state_list *TokiVC::block_state_list() const noexcept {

  if (this->_step <= VCL_Kernel_step::VCL_none) {
    return nullptr;
  }

  return &this->bsl;
}