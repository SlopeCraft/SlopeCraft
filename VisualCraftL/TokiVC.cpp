#include "TokiVC.h"

#include <mutex>
#include <set>
#include <shared_mutex>
#include <variant>

#include "VCL_internal.h"

libImageCvt::template ImageCvter<false>::basic_colorset_t
    TokiVC::colorset_basic;
libImageCvt::template ImageCvter<false>::allowed_colorset_t
    TokiVC::colorset_allowed;

// bind static members for template classes
template <>
const libImageCvt::template ImageCvter<false>::basic_colorset_t
    &libImageCvt::template ImageCvter<false>::basic_colorset =
        TokiVC::colorset_basic;

template <>
const libImageCvt::template ImageCvter<false>::allowed_colorset_t
    &libImageCvt::template ImageCvter<false>::allowed_colorset =
        TokiVC::colorset_allowed;

template <>
const libImageCvt::template ImageCvter<false>::basic_colorset_t
    *const newTokiColor<
        false, libImageCvt::template ImageCvter<false>::basic_colorset_t,
        libImageCvt::template ImageCvter<false>::allowed_colorset_t>::Basic =
        &TokiVC::colorset_basic;

template <>
const libImageCvt::template ImageCvter<false>::allowed_colorset_t
    *const newTokiColor<
        false, libImageCvt::template ImageCvter<false>::basic_colorset_t,
        libImageCvt::template ImageCvter<false>::allowed_colorset_t>::Allowed =
        &TokiVC::colorset_allowed;

// global variables that VCL uses
VCL_resource_pack TokiVC::pack;
VCL_block_state_list TokiVC::bsl;
SCL_gameVersion TokiVC::version = SCL_gameVersion::MC19;
VCL_face_t TokiVC::exposed_face = VCL_face_t::face_down;
int TokiVC::max_block_layers = 3;
bool TokiVC::is_render_quality_fast{true};
VCL_biome_t TokiVC::biome{VCL_biome_t::the_void};

std::vector<std::variant<const VCL_block *, std::vector<const VCL_block *>>>
    TokiVC::LUT_basic_color_idx_to_blocks;

std::unordered_map<const VCL_block *, uint16_t> TokiVC::blocks_allowed;

namespace TokiVC_internal {
std::shared_mutex global_lock;
bool is_basic_color_set_ready = false;
bool is_allowed_color_set_ready = false;

std::set<TokiVC *> TokiVC_register;
} // namespace TokiVC_internal

TokiVC::TokiVC() {
  TokiVC_internal::global_lock.lock();
  if (TokiVC_internal::is_basic_color_set_ready) {
    this->_step = VCL_Kernel_step::VCL_wait_for_image;
  } else {
    this->_step = VCL_Kernel_step::VCL_wait_for_resource;
  }

  TokiVC_internal::TokiVC_register.emplace(this);

  TokiVC_internal::global_lock.unlock();
}

TokiVC::~TokiVC() {
  TokiVC_internal::global_lock.lock();

  auto it = TokiVC_internal::TokiVC_register.find(this);

  if (it != TokiVC_internal::TokiVC_register.end()) {
    TokiVC_internal::TokiVC_register.erase(it);
  }

  TokiVC_internal::global_lock.unlock();
}

void TokiVC::show_gpu_name() const noexcept {
  std::string msg = this->img_cvter.gpu_resource()->device_vendor_v();
  VCL_report(VCL_report_type_t::information, msg.c_str());
}

void TokiVC::set_ui(void *uiptr,
                    void (*progressRangeSet)(void *, int, int, int),
                    void (*progressAdd)(void *, int)) noexcept {
  this->img_cvter.ui = {uiptr, progressRangeSet, progressAdd};
}

VCL_Kernel_step TokiVC::step() const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  return this->_step;
}

bool add_projection_image_for_bsl(const std::vector<VCL_block *> &bs_list,
                                  resource_pack::buffer_t &buff) noexcept {
  for (VCL_block *blkp : bs_list) {
    if (blkp->full_id_ptr() == nullptr) {
      std::string msg = fmt::format(
          "\nError : a VCL_block do not have full_id. The block names are : "
          "{}, {}\n",
          blkp->name_ZH, blkp->name_EN);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    if constexpr (false) {
      std::string msg =
          fmt::format("Computing projection image for full id \"{}\"\n",
                      blkp->full_id_ptr()->c_str());
      VCL_report(VCL_report_type_t::information, msg.c_str());
    }

    block_model::EImgRowMajor_t *img = &blkp->project_image_on_exposed_face;

    if (!TokiVC::pack.compute_projection(*blkp->full_id_ptr(),
                                         TokiVC::exposed_face, img, buff)) {
      std::string msg = fmt::format("failed to compute projection for {}.\n",
                                    blkp->full_id_ptr()->c_str());
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }
  }
  return true;
}

bool add_color_non_transparent(
    const std::vector<VCL_block *> &bs_nontransparent,
    std::vector<std::variant<const VCL_block *, std::vector<const VCL_block *>>>
        &LUT_bcitb,
    std::vector<std::array<uint8_t, 3>> &temp_rgb_rowmajor) noexcept {
  for (VCL_block *blkp : bs_nontransparent) {
    bool ok = true;
    auto ret = compute_mean_color(blkp->project_image_on_exposed_face, &ok);
    if (!ok) {
      return false;
    }

    temp_rgb_rowmajor.emplace_back(ret);
    LUT_bcitb.emplace_back(blkp);
  }
  return true;
}

bool add_color_trans_to_nontrans(
    const block_model::EImgRowMajor_t &front,
    const std::vector<VCL_block *> &bs_nontransparent,
    const std::vector<const VCL_block *> &accumulate_blocks,
    std::vector<std::variant<const VCL_block *, std::vector<const VCL_block *>>>
        &LUT_bcitb,
    std::vector<std::array<uint8_t, 3>> &temp_rgb_rowmajor) noexcept {
  if (front.size() <= 0) {
    return false;
  }

  for (VCL_block *blkp : bs_nontransparent) {
    if (!blkp->is_background()) {
      continue;
    }
    bool ok = true;

    std::array<uint8_t, 3> ret =
        compose_image_and_mean(front, blkp->project_image_on_exposed_face, &ok);

    if (!ok) {
      return false;
    }
    std::vector<const VCL_block *> blocks(accumulate_blocks);
    blocks.emplace_back(blkp);

    temp_rgb_rowmajor.emplace_back(ret);
    LUT_bcitb.emplace_back(std::move(blocks));
  }

  return true;
}

bool add_color_trans_to_trans_recurs(
    const int allowed_depth, const block_model::EImgRowMajor_t &front,
    const std::vector<VCL_block *> &bs_transparent,
    const std::vector<VCL_block *> &bs_nontransparent,
    const std::vector<const VCL_block *> &accumulate_blocks,
    std::vector<std::variant<const VCL_block *, std::vector<const VCL_block *>>>
        &LUT_bcitb,
    std::vector<std::array<uint8_t, 3>> &temp_rgb_rowmajor) noexcept {
  if (allowed_depth <= 0) {
    std::string msg =
        fmt::format("Invalid value for allowed_depth : {}\n", allowed_depth);
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  if (allowed_depth == 1) {
    return add_color_trans_to_nontrans(front, bs_nontransparent,
                                       accumulate_blocks, LUT_bcitb,
                                       temp_rgb_rowmajor);
  }
  block_model::EImgRowMajor_t img(front);

  {
    uint8_t min_alpha = 255;
    for (int i = 0; i < front.size(); i++) {
      min_alpha = std::min(min_alpha, getA(front(i)));
    }

    // if multiple transparent block composed a non transparent image, then the
    // recursion terminate.
    if (min_alpha >= 255) {
      bool ok = true;
      std::array<uint8_t, 3> mean = compute_mean_color(front, &ok);

      if (!ok) {
        VCL_report(VCL_report_type_t::error,
                   "Function add_color_trans_to_trans_recurs failed to "
                   "compute mean color.\n");
        return false;
      }

      LUT_bcitb.emplace_back(accumulate_blocks);
      temp_rgb_rowmajor.emplace_back(mean);
      return true;
    }
  }

  for (const VCL_block *cblkp : bs_transparent) {
    if (cblkp == accumulate_blocks.back()) {
      continue;
    }

    memcpy(img.data(), front.data(), front.size() * sizeof(uint32_t));
    std::vector<const VCL_block *> blocks(accumulate_blocks);
    blocks.emplace_back(cblkp);

    if (!compose_image_background_half_transparent(
            img, cblkp->project_image_on_exposed_face)) {
      VCL_report(VCL_report_type_t::error,
                 "Function add_color_trans_to_trans_recurs failed "
                 "because failed to compose image. This is possible caused by "
                 "images have different sizes.\n");
      return false;
    }

    if (!add_color_trans_to_trans_recurs(allowed_depth - 1, img, bs_transparent,
                                         bs_nontransparent, blocks, LUT_bcitb,
                                         temp_rgb_rowmajor)) {
      VCL_report(VCL_report_type_t::error,
                 "Function add_color_trans_to_trans_recurs failed "
                 "because deeper recursion failed.\n");
      return false;
    }
  }

  return true;
}

bool add_color_trans_to_trans_start_recurse(
    const int max_allowed_depth, const std::vector<VCL_block *> &bs_transparent,
    const std::vector<VCL_block *> &bs_nontransparent,
    std::vector<std::variant<const VCL_block *, std::vector<const VCL_block *>>>
        &LUT_bcitb,
    std::vector<std::array<uint8_t, 3>> &temp_rgb_rowmajor) noexcept {
  if (max_allowed_depth <= 0) {
    return false;
  }

  std::vector<const VCL_block *> accum({nullptr});

  for (const VCL_block *cblkp : bs_transparent) {
    accum[0] = cblkp;
    if (!add_color_trans_to_trans_recurs(max_allowed_depth - 1,
                                         cblkp->project_image_on_exposed_face,
                                         bs_transparent, bs_nontransparent,
                                         accum, LUT_bcitb, temp_rgb_rowmajor)) {
      VCL_report(
          VCL_report_type_t::error,
          "Function add_color_trans_to_trans_start_recurse failed "
          "due to function call to add_color_trans_to_trans_recurs failed.\n");
      return false;
    }
  }

  return true;
}

bool TokiVC::set_resource_no_lock() noexcept {
  switch (TokiVC::version) {
  case SCL_gameVersion::ANCIENT:
  case SCL_gameVersion::FUTURE: {
    std::string msg =
        fmt::format("Invalid MC version : {}\n", int(TokiVC::version));
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }
  default:
    break;
  }

  TokiVC::pack.set_is_MC12(TokiVC::version == SCL_gameVersion::MC12);
  TokiVC::pack.override_textures(TokiVC::biome, TokiVC::is_render_quality_fast);

  TokiVC::bsl.update_foliages(!TokiVC::is_render_quality_fast);

  std::vector<VCL_block *> bs_transparent, bs_nontransparent;

  bs_nontransparent.reserve(TokiVC::bsl.block_states().size() * 2 / 3);

  TokiVC::bsl.avaliable_block_states_by_transparency(
      TokiVC::version, TokiVC::exposed_face, &bs_nontransparent,
      &bs_transparent);

  {
    resource_pack::buffer_t buff;
    {
      buff.pure_id.reserve(256);
      buff.state_list.reserve(16);
      // buff.traits.reserve(16);
    }

    if (!add_projection_image_for_bsl(bs_nontransparent, buff)) {
      VCL_report(VCL_report_type_t::error,
                 "Failed to go through bs_nontransparent\n");
      return false;
    }

    if (!add_projection_image_for_bsl(bs_transparent, buff)) {
      VCL_report(VCL_report_type_t::error,
                 "Failed to go through bs_transparent\n");
      return false;
    }

    std::vector<std::array<uint8_t, 3>> colors_temp;
    colors_temp.reserve(bs_nontransparent.size());
    TokiVC::LUT_basic_color_idx_to_blocks.clear();
    if (!add_color_non_transparent(bs_nontransparent,
                                   TokiVC::LUT_basic_color_idx_to_blocks,
                                   colors_temp)) {
      VCL_report(VCL_report_type_t::error,
                 "Failed to compute mean colors for non transparent "
                 "images.\n");
      return false;
    }

    if constexpr (false) {
      std::string msg = fmt::format(
          "Size of LUT_basic_color_idx_to_blocks = {}, size of colors_temp = "
          "{}\n",
          TokiVC::LUT_basic_color_idx_to_blocks.size(), colors_temp.size());
      VCL_report(VCL_report_type_t::information, msg.c_str());
    }

    for (int layers = 2; layers <= max_block_layers; layers++) {
      if (!add_color_trans_to_trans_start_recurse(
              layers, bs_transparent, bs_nontransparent,
              TokiVC::LUT_basic_color_idx_to_blocks, colors_temp)) {
        VCL_report(VCL_report_type_t::error,
                   "failed to compute colors for composed blocks.\n");
        return false;
      }
    }
    if constexpr (false) {
      std::string msg = fmt::format(
          "Size of LUT_basic_color_idx_to_blocks = {}, size of colors_temp = "
          "{}\n",
          TokiVC::LUT_basic_color_idx_to_blocks.size(), colors_temp.size());
      VCL_report(VCL_report_type_t::information, msg.c_str());
    }

    if (colors_temp.size() != TokiVC::LUT_basic_color_idx_to_blocks.size()) {
      std::string msg = fmt::format(
          "\nImpossible error : "
          "colors_temp.size() (aka {}) "
          "!=TokiVC::LUT_basic_color_idx_to_blocks.size() (aka {})\n",
          colors_temp.size(), TokiVC::LUT_basic_color_idx_to_blocks.size());
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }

    if (colors_temp.size() >= UINT16_MAX - 1) {
      std::string msg = fmt::format(
          "\nError : too much colors. Num of colors should not exceed {}, "
          "but it is {} now.\n",
          UINT16_MAX - 1, colors_temp.size());
      VCL_report(VCL_report_type_t::error,
                 "failed to compute colors for composed blocks.\n");
      return false;
    }
    // here the basic colors are ready.
    {
      Eigen::Array<float, Eigen::Dynamic, 3> arrX3f;
      arrX3f.resize(colors_temp.size(), 3);

      for (int r = 0; r < int(colors_temp.size()); r++) {
        for (int c = 0; c < 3; c++) {
          arrX3f(r, c) = colors_temp[r][c] / 255.0f;
        }
      }
      TokiVC::colorset_basic.set_colors(arrX3f.data(), arrX3f.rows());
    }
  }

  // update steps
  for (auto ptr : TokiVC_internal::TokiVC_register) {
    ptr->_step = VCL_Kernel_step::VCL_wait_for_allowed_list;
    ptr->img_cvter.on_color_set_changed();
  }

  TokiVC_internal::is_basic_color_set_ready = true;

  return true;
}

bool TokiVC::set_allowed_no_lock(const VCL_block *const *const blocks_allowed,
                                 size_t num_block_allowed) noexcept {
  if (!TokiVC_internal::is_basic_color_set_ready) {
    VCL_report(
        VCL_report_type_t::error,
        "You can not set the allowed blocks before basic color set is ready.");
    return false;
  }

  TokiVC::blocks_allowed.clear();
  TokiVC::blocks_allowed.reserve(num_block_allowed);

  for (size_t i = 0; i < num_block_allowed; i++) {
    if (blocks_allowed[i] == nullptr ||
        blocks_allowed[i]->full_id_ptr() == nullptr) {
      VCL_report(VCL_report_type_t::error, "Invalid VCL_block pointer.");
      return false;
    }

    TokiVC::blocks_allowed.emplace(blocks_allowed[i], 0xFFFF);
  }

  {
    uint16_t counter = 1;
    size_t counter_air = 0;
    for (auto &pair : TokiVC::blocks_allowed) {
      if (pair.first->is_air()) {
        pair.second = 0;
        counter_air++;
        continue;
      }

      pair.second = counter;
      counter++;
    }

    if (counter_air != 1) {
      std::string msg =
          fmt::format("Types of air block is {}, expected 1.", counter_air);
      VCL_report(VCL_report_type_t::error, msg.c_str());
      return false;
    }
  }

  std::vector<uint8_t> allowed_list;
  allowed_list.resize(TokiVC::LUT_basic_color_idx_to_blocks.size());
  memset(allowed_list.data(), 0, allowed_list.size());

  if constexpr (false) {
    std::string msg = fmt::format("TokiVC::colorset_basic.color_count() = {}.",
                                  TokiVC::colorset_basic.color_count());
    VCL_report(VCL_report_type_t::information, msg.c_str());
  }

  for (size_t idx = 0; idx < TokiVC::LUT_basic_color_idx_to_blocks.size();
       idx++) {
    const auto &variant = LUT_basic_color_idx_to_blocks[idx];
    // allowed_list[idx] = false;
    if (variant.index() == 0) {
      if (!TokiVC::blocks_allowed.contains(std::get<0>(variant))) {
        continue;
      }
    } else {
      for (const VCL_block *blkp : std::get<1>(variant)) {
        if (!TokiVC::blocks_allowed.contains(blkp)) {
          continue;
        }
      }
    }

    allowed_list[idx] = true;
  }
  if (!TokiVC::colorset_allowed.apply_allowed(
          TokiVC::colorset_basic,
          reinterpret_cast<bool *>(allowed_list.data()))) {
    VCL_report(VCL_report_type_t::error,
               "Function \"TokiVC::colorset_allowed.apply_allowed\" failed.");
    return false;
  }

  TokiVC_internal::is_allowed_color_set_ready = true;

  for (TokiVC *tkvcp : TokiVC_internal::TokiVC_register) {
    tkvcp->_step = VCL_Kernel_step::VCL_wait_for_image;
    tkvcp->img_cvter.on_color_set_changed();
  }

  return true;
}

bool TokiVC::set_image(const int64_t rows, const int64_t cols,
                       const uint32_t *const img_argb,
                       const bool is_row_major) noexcept {
  if (rows <= 0 || cols <= 0 || img_argb == nullptr) {
    return false;
  }

  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_wait_for_image) {
    VCL_report(VCL_report_type_t::error, "Trying to skip steps.");
    return false;
  }

  this->img_cvter.set_raw_image(img_argb, rows, cols, !is_row_major);

  this->_step = VCL_Kernel_step::VCL_wait_for_conversion;

  return true;
}

int64_t TokiVC::rows() const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (this->_step < VCL_Kernel_step::VCL_wait_for_conversion) {
    return 0;
  }

  return this->img_cvter.rows();
}
int64_t TokiVC::cols() const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (this->_step < VCL_Kernel_step::VCL_wait_for_conversion) {
    return 0;
  }

  return this->img_cvter.cols();
}

const uint32_t *TokiVC::raw_image(int64_t *const __rows, int64_t *const __cols,
                                  bool *const is_row_major) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
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

bool TokiVC::convert(::SCL_convertAlgo algo, bool dither) noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (this->_step < VCL_Kernel_step::VCL_wait_for_conversion) {
    return false;
  }
  if (!this->img_cvter.convert_image(algo, dither, this->imgcvter_prefer_gpu)) {
    std::string msg =
        fmt::format("Failed to convert. detail : {}, error code = {}",
                    this->img_cvter.gpu_resource()->error_detail_v(),
                    this->img_cvter.gpu_resource()->error_code_v());
    VCL_report(VCL_report_type_t::error, msg.c_str());
    return false;
  }

  this->_step = VCL_Kernel_step::VCL_wait_for_build;
  return true;
}

void TokiVC::converted_image(uint32_t *dest, int64_t *rows, int64_t *cols,
                             bool write_dest_row_major) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);
  if (this->_step < VCL_Kernel_step::VCL_wait_for_build) {
    return;
  }

  // constexpr size_t sz = sizeof(decltype(this->img_cvter)::TokiColor_t);

  this->img_cvter.converted_image(dest, rows, cols, !write_dest_row_major);
}
