#ifndef SLOPECRAFT_VISUALCRAFTL_TOKIVC_H
#define SLOPECRAFT_VISUALCRAFTL_TOKIVC_H

#include "BlockStateList.h"
#include "ParseResourcePack.h"
#include "VisualCraftL.h"

#include <shared_mutex>
#include <utilities/ColorManip/colorset_optical.hpp>
#include <utilities/ColorManip/imageConvert.hpp>
#include <utilities/Schem/Schem.h>
#include <utilities/uiPack/uiPack.h>
#include <variant>

#include <unordered_map>

class TokiVC : public VCL_Kernel {
public:
  TokiVC();
  virtual ~TokiVC();
  void set_ui(void *uiptr, void (*progressRangeSet)(void *, int, int, int),
              void (*progressAdd)(void *, int)) noexcept override;

  bool have_gpu_resource() const noexcept override {
    return this->img_cvter.have_ocl();
  }

  bool set_gpu_resource(size_t platform_idx,
                        size_t device_idx) noexcept override {
    this->img_cvter.set_ocl(
        ocl_warpper::ocl_resource(platform_idx, device_idx));
    return this->img_cvter.ocl_resource().ok();
  }

  bool prefer_gpu() const noexcept override {
    return this->imgcvter_prefer_gpu;
  }

  void set_prefer_gpu(bool try_gpu) noexcept override {
    this->imgcvter_prefer_gpu = try_gpu;
  }

  void show_gpu_name() const noexcept override;
  size_t get_gpu_name(char *string_buffer,
                      size_t buffer_capacity) const noexcept override {
    const std::string result = this->img_cvter.ocl_resource().device_vendor();
    if (string_buffer == nullptr || buffer_capacity <= 0) {
      return result.size();
    }

    memcpy(string_buffer, result.c_str(),
           std::min(buffer_capacity, result.size()));
    return result.size();
  }

  VCL_Kernel_step step() const noexcept override;

  bool set_image(const int64_t rows, const int64_t tcols,
                 const uint32_t *const img_argb,
                 const bool is_row_major) noexcept override;

  int64_t rows() const noexcept override;
  int64_t cols() const noexcept override;

  const uint32_t *raw_image(int64_t *const rows, int64_t *const cols,
                            bool *const is_row_major) const noexcept override;

  bool convert(::SCL_convertAlgo algo, bool dither) noexcept override;
  void converted_image(uint32_t *dest, int64_t *rows, int64_t *cols,
                       bool write_dest_row_major) const noexcept override;

  bool build() noexcept override;
  int64_t xyz_size(int64_t *x, int64_t *y, int64_t *z) const noexcept override;

public:
  static libImageCvt::template ImageCvter<false>::basic_colorset_t
      colorset_basic;
  static libImageCvt::template ImageCvter<false>::allowed_colorset_t
      colorset_allowed;

  static VCL_resource_pack pack;
  static VCL_block_state_list bsl;
  static SCL_gameVersion version;
  static VCL_face_t exposed_face;
  static int max_block_layers;

  static bool set_resource_no_lock() noexcept;
  static bool set_allowed_no_lock(const VCL_block *const *const blocks_allowed,
                                  size_t num_block_allowed) noexcept;

private:
  static std::vector<
      std::variant<const VCL_block *, std::vector<const VCL_block *>>>
      LUT_basic_color_idx_to_blocks;
  static std::unordered_map<const VCL_block *, uint16_t> blocks_allowed;

private:
  VCL_Kernel_step _step{VCL_Kernel_step::VCL_wait_for_resource};
  bool imgcvter_prefer_gpu{false};

  libImageCvt::ImageCvter<false> img_cvter;
  libSchem::Schem schem;

  void fill_schem_blocklist_no_lock() noexcept;
};

namespace TokiVC_internal {
extern std::shared_mutex global_lock;
extern bool is_basic_color_set_ready;
extern bool is_allowed_color_set_ready;
} // namespace TokiVC_internal

#endif // SLOPECRAFT_VISUALCRAFTL_TOKIVC_H