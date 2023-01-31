#include "TokiVC.h"

#include <mutex>
#include <set>
#include <shared_mutex>

libImageCvt::template ImageCvter<false>::basic_colorset_t
    TokiVC::colorset_basic;
libImageCvt::template ImageCvter<false>::allowed_colorset_t
    TokiVC::colorset_allowed;

template <>
const libImageCvt::template ImageCvter<false>::basic_colorset_t
    &libImageCvt::template ImageCvter<false>::basic_colorset =
        TokiVC::colorset_basic;

template <>
const libImageCvt::template ImageCvter<false>::allowed_colorset_t
    &libImageCvt::template ImageCvter<false>::allowed_colorset =
        TokiVC::colorset_allowed;

VCL_resource_pack TokiVC::pack;
VCL_block_state_list TokiVC::bsl;
SCL_gameVersion TokiVC::version = MC19;
VCL_face_t TokiVC::exposed_face = VCL_face_t::face_down;
int TokiVC::max_block_layers = 3;

namespace {
std::shared_mutex global_lock;
bool is_color_set_ready = false;

std::set<TokiVC *> TokiVC_register;
} // namespace

TokiVC::TokiVC() {
  global_lock.lock();
  if (is_color_set_ready) {
    this->_step = VCL_Kernel_step::VCL_wait_for_image;
  } else {
    this->_step = VCL_Kernel_step::VCL_none;
  }

  TokiVC_register.emplace(this);

  global_lock.unlock();
}

TokiVC::~TokiVC() {
  global_lock.lock();

  auto it = TokiVC_register.find(this);

  if (it != TokiVC_register.end()) {
    TokiVC_register.erase(it);
  }

  global_lock.unlock();
}

VCL_EXPORT_FUN bool VCL_set_resource_and_version_copy(
    const VCL_resource_pack *const rp, const VCL_block_state_list *const bsl,
    SCL_gameVersion version, VCL_face_t face, int __max_block_layers) {

  if (rp == nullptr || bsl == nullptr) {
    return false;
  }

  if (__max_block_layers <= 0) {
    return false;
  }

  std::unique_lock<std::shared_mutex> lkgd(global_lock);

  is_color_set_ready = false;
  TokiVC::pack = *rp;
  TokiVC::bsl = *bsl;

  TokiVC::version = version;
  TokiVC::exposed_face = face;
  TokiVC::max_block_layers = __max_block_layers;

  const bool ret = TokiVC::update_color_set_no_lock();

  return ret;
}

VCL_EXPORT_FUN bool VCL_set_resource_and_version_move(
    VCL_resource_pack **rp_ptr, VCL_block_state_list **bsl_ptr,
    SCL_gameVersion version, VCL_face_t face, int __max_block_layers) {
  if (rp_ptr == nullptr || bsl_ptr == nullptr) {
    return false;
  }

  if (*rp_ptr == nullptr || *bsl_ptr == nullptr) {
    return false;
  }
  if (__max_block_layers <= 0) {
    return false;
  }

  bool ret = true;
  std::unique_lock<std::shared_mutex> lkgd(global_lock);

  is_color_set_ready = false;

  TokiVC::pack = std::move(**rp_ptr);
  VCL_destroy_resource_pack(*rp_ptr);
  *rp_ptr = nullptr;

  TokiVC::bsl = std::move(**bsl_ptr);
  VCL_destroy_block_state_list(*bsl_ptr);
  *bsl_ptr = nullptr;

  TokiVC::version = version;
  TokiVC::exposed_face = face;
  TokiVC::max_block_layers = __max_block_layers;

  if (!TokiVC::update_color_set_no_lock()) {
    ret = false;
  }

  return ret;
}

VCL_EXPORT_FUN bool VCL_is_colorset_ok() {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  return is_color_set_ready;
}

VCL_EXPORT_FUN VCL_resource_pack *VCL_get_resource_pack() {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  if (!is_color_set_ready) {
    return nullptr;
  }

  return &TokiVC::pack;
}

VCL_EXPORT_FUN VCL_block_state_list *VCL_get_block_state_list() {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  if (!is_color_set_ready) {
    return nullptr;
  }
  return &TokiVC::bsl;
}

VCL_EXPORT_FUN SCL_gameVersion VCL_get_game_version() {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  if (!is_color_set_ready) {
    return SCL_gameVersion::ANCIENT;
  }
  return TokiVC::version;
}

VCL_EXPORT_FUN VCL_face_t VCL_get_exposed_face() {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);

  if (!is_color_set_ready) {
    return {};
  }

  return TokiVC::exposed_face;
}

VCL_EXPORT_FUN int VCL_get_max_block_layers() {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  if (!is_color_set_ready) {
    return 0;
  }

  return TokiVC::max_block_layers;
}

VCL_Kernel_step TokiVC::step() const noexcept {

  std::shared_lock<std::shared_mutex> lkgd(global_lock);

  return this->_step;
}

bool TokiVC::update_color_set_no_lock() noexcept {
  switch (TokiVC::version) {
  case SCL_gameVersion::ANCIENT:
    return false;
  case SCL_gameVersion::FUTURE:
    return false;
  default:
    break;
  }
#warning update color set here.

  std::vector<const std::string *> bs_transparent, bs_nontransparent;

  bs_nontransparent.reserve(TokiVC::bsl.block_states().size() * 2 / 3);

  TokiVC::bsl.avaliable_block_states_by_transparency(
      TokiVC::version, &bs_nontransparent, &bs_transparent);

  // update steps
  for (auto ptr : TokiVC_register) {
    ptr->_step = VCL_Kernel_step::VCL_wait_for_image;
    ptr->img_cvter.on_color_set_changed();
  }

  is_color_set_ready = true;

  return true;
}

bool TokiVC::set_image(const int64_t rows, const int64_t cols,
                       const uint32_t *const img_argb,
                       const bool is_row_major) noexcept {
  if (rows <= 0 || cols <= 0 || img_argb == nullptr) {
    return false;
  }

  std::shared_lock<std::shared_mutex> lkgd(global_lock);

  if (this->_step < VCL_Kernel_step::VCL_wait_for_image) {
    return false;
  }

  this->img_cvter.set_raw_image(img_argb, rows, cols, !is_row_major);

  this->_step = VCL_Kernel_step::VCL_wait_for_build;

  return true;
}

int64_t TokiVC::rows() const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  if (this->_step < VCL_Kernel_step::VCL_wait_for_conversion) {
    return 0;
  }

  return this->img_cvter.rows();
}
int64_t TokiVC::cols() const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  if (this->_step < VCL_Kernel_step::VCL_wait_for_conversion) {
    return 0;
  }

  return this->img_cvter.cols();
}

const uint32_t *TokiVC::raw_image(int64_t *const __rows, int64_t *const __cols,
                                  bool *const is_row_major) const noexcept {

  std::shared_lock<std::shared_mutex> lkgd(global_lock);
  if (this->_step < VCL_Kernel_step::VCL_wait_for_conversion) {
    return nullptr;
  }

  if (__rows != nullptr) {
    *__rows = this->img_cvter.rows();
  }

  if (__cols != nullptr) {
    *__cols = this->img_cvter.cols();
  }

  if (is_row_major != nullptr) {
    *is_row_major = false;
  }

  return this->img_cvter.raw_image().data();
}