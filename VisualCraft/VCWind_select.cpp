#include "VCWind.h"
#include "VC_block_class.h"
#include "ui_VCWind.h"

void VCWind::apply_selection(
    std::function<void(const VCL_block *, QCheckBox *)> selector) noexcept {

  for (auto &blk_class : this->map_VC_block_class) {
    for (auto &blk_pair : blk_class.second->blocks_vector()) {
      selector(blk_pair.first, blk_pair.second);
    }
  }
}

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

int VCWind::count_block_matched(std::function<bool(const VCL_block *)>
                                    return_true_when_match) const noexcept {
  int counter = 0;
  for (auto &blk_class : this->map_VC_block_class) {
    for (auto &blk_pair : blk_class.second->blocks_vector()) {
      if (return_true_when_match(blk_pair.first)) {
        counter++;
      }
    }
  }

  return counter;
}

void VCWind::on_pb_invselect_classwise_clicked() noexcept {
  for (auto &blk_class : this->map_VC_block_class) {
    blk_class.second->chbox_enabled()->setChecked(
        !blk_class.second->chbox_enabled()->isChecked());
  }
}

void VCWind::on_pb_invselect_blockwise_clicked() noexcept {
  this->apply_selection([](const VCL_block *, QCheckBox *cb) {
    cb->setChecked(!cb->isChecked());
  });
}