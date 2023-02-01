#ifndef SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H
#define SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H

#ifdef VISUALCRAFTL_BUILD
#include <utilities/SC_GlobalEnums.h>
#else
#include "SC_GlobalEnums.h"
#endif

#include "VisualCraftL_global.h"

#include <stdint.h>

enum class VCL_face_t : uint8_t {
  face_up = 0,
  face_down = 1,
  face_north = 2,
  face_south = 3,
  face_east = 4,
  face_west = 5
};

class VCL_Kernel;
class VCL_resource_pack;
class VCL_block_state_list;
class VCL_block;

class VCL_Kernel {
public:
  VCL_Kernel() = default;
  virtual ~VCL_Kernel() = default;

  virtual VCL_Kernel_step step() const noexcept = 0;

  virtual bool set_image(const int64_t rows, const int64_t cols,
                         const uint32_t *const img_argb32,
                         const bool is_row_major) noexcept = 0;

  virtual int64_t rows() const noexcept = 0;
  virtual int64_t cols() const noexcept = 0;

  virtual const uint32_t *
  raw_image(int64_t *const rows, int64_t *const cols,
            bool *const is_row_major) const noexcept = 0;
};

extern "C" {

[[nodiscard]] VCL_EXPORT_FUN VCL_Kernel *VCL_create_kernel();
VCL_EXPORT_FUN void VCL_destroy_kernel(VCL_Kernel *const ptr);

VCL_EXPORT_FUN bool VCL_set_resource_and_version_copy(
    const VCL_resource_pack *const rp, const VCL_block_state_list *const bsl,
    SCL_gameVersion version, VCL_face_t exposed_face, int max_block_layers);

VCL_EXPORT_FUN bool VCL_set_resource_and_version_move(
    VCL_resource_pack **rp_ptr, VCL_block_state_list **bsl_ptr,
    SCL_gameVersion version, VCL_face_t exposed_face, int max_block_layers);

VCL_EXPORT_FUN bool VCL_is_colorset_ok();
VCL_EXPORT_FUN VCL_resource_pack *VCL_get_resource_pack();
VCL_EXPORT_FUN VCL_block_state_list *VCL_get_block_state_list();
VCL_EXPORT_FUN SCL_gameVersion VCL_get_game_version();
VCL_EXPORT_FUN VCL_face_t VCL_get_exposed_face();
VCL_EXPORT_FUN int VCL_get_max_block_layers();

VCL_EXPORT_FUN void test_VCL();

[[nodiscard]] VCL_EXPORT_FUN VCL_resource_pack *
VCL_create_resource_pack(const int zip_file_count,
                         const char *const *const zip_file_names,
                         bool cover_from_first_to_end = true);
VCL_EXPORT_FUN void VCL_destroy_resource_pack(VCL_resource_pack *const ptr);

[[nodiscard]] VCL_EXPORT_FUN VCL_block_state_list *
VCL_create_block_state_list(const int file_count,
                            const char *const *const json_file_names);

VCL_EXPORT_FUN void
VCL_destroy_block_state_list(VCL_block_state_list *const ptr);

VCL_EXPORT_FUN void VCL_display_resource_pack(const VCL_resource_pack *);
VCL_EXPORT_FUN void VCL_display_block_state_list(const VCL_block_state_list *);
}

#endif // SLOPECRAFT_VISUALCRAFT_VISUALCRAFT_H