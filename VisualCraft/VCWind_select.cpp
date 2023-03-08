#include "VCWind.h"
#include "VC_block_class.h"
#include "ui_VCWind.h"

void VCWind::select_blocks(
    std::function<bool(const VCL_block *)> return_true_for_select) noexcept {
  for (auto &blk_class : this->map_VC_block_class) {
    for (auto &blk_pair : blk_class.second->blocks_vector()) {
      if (return_true_for_select(blk_pair.first)) {
        blk_pair.second->setChecked(true);
      }
    }
  }
}

void VCWind::deselect_blocks(
    std::function<bool(const VCL_block *)> return_true_for_deselect) noexcept {
  for (auto &blk_class : this->map_VC_block_class) {
    for (auto &blk_pair : blk_class.second->blocks_vector()) {
      if (return_true_for_deselect(blk_pair.first)) {
        blk_pair.second->setChecked(false);
      }
    }
  }
}

void VCWind::on_pb_select_all_clicked() noexcept {
  this->select_blocks([](const VCL_block *) noexcept { return true; });
}
void VCWind::on_pb_deselect_all_clicked() noexcept {
  this->deselect_blocks([](const VCL_block *) noexcept { return true; });
}

void VCWind::on_pb_deselect_non_reporducible_clicked() noexcept {
  this->deselect_blocks([](const VCL_block *blk) noexcept {
    if (!VCL_get_block_attribute(blk, VCL_block_attribute_t::reproducible)) {
      return true;
    } else {
      return false;
    }
  });
}
void VCWind::on_pb_deselect_rare_clicked() noexcept {
  this->deselect_blocks([](const VCL_block *blk) noexcept {
    if (VCL_get_block_attribute(blk, VCL_block_attribute_t::rare)) {
      return true;
    } else {
      return false;
    }
  });
}