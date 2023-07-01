/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "VCWind.h"
#include "VC_block_class.h"
#include "ui_VCWind.h"
#include <QFileDialog>
#include <QMessageBox>

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

[[nodiscard]] std::unordered_map<std::string_view, QCheckBox *>
VCWind::id_blockclass_map() noexcept {
  std::unordered_map<std::string_view, QCheckBox *> ret;
  for (auto &blk_class_pair : this->map_VC_block_class) {
    for (auto &blk : blk_class_pair.second->blocks_vector()) {
      ret.emplace(VCL_get_block_id(blk.first), blk.second);
    }
  }

  return ret;
}

void VCWind::on_pb_load_preset_clicked() noexcept {
  static QString prev_dir{""};
  QString preset_file = QFileDialog::getOpenFileName(this, tr("选择预设文件"),
                                                     prev_dir, "*.vc_preset");
  if (preset_file.isEmpty()) {
    return;
  }
  prev_dir = QFileInfo{preset_file}.dir().absolutePath();

  std::string err;
  err.resize(8192);
  VCL_string_deliver vsd{err.data(), err.size(), err.size()};
  auto preset = VCL_load_preset(preset_file.toLocal8Bit().data(), &vsd);
  err.resize(vsd.size);

  if (preset == nullptr) {
    QMessageBox::warning(
        this, tr("加载预设失败"),
        tr("无法解析预设文件，详细信息：\n%1").arg(err.data()),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Ok});
    return;
  }

  for (auto &cls : this->map_VC_block_class) {
    const bool enable = VCL_preset_contains_class(preset, cls.first);
    cls.second->chbox_enabled()->setChecked(enable);
  }
  // select ids
  {
    std::vector<const char *> ids;
    ids.resize(VCL_preset_num_ids(preset));
    {
      const size_t num = VCL_preset_get_ids(preset, ids.data(), ids.size());
      ids.resize(num);
    }

    auto id_blk_mapping = this->id_blockclass_map();
    for (auto &pair : id_blk_mapping) {
      pair.second->setChecked(false);
    }
    for (auto id : ids) {
      auto it = id_blk_mapping.find(id);
      if (it == id_blk_mapping.end()) {
        auto ret = QMessageBox::warning(
            this, tr("无法加载预设"),
            tr("预设中包含%1，但可用方块中没有 id 相同的方块。点击 Ignore "
               "以跳过这个方块，点击 Close 终止此次操作。"),
            QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore},
            QMessageBox::StandardButton::Close);
        if (ret == QMessageBox::StandardButton::Close) {
          VCL_destroy_preset(preset);
          return;
        }
        continue;
      }
      it->second->setChecked(true);
    }
  }

  VCL_destroy_preset(preset);
}

void VCWind::on_pb_save_preset_clicked() noexcept {
  static QString prev_dir{""};
  QString preset_file = QFileDialog::getSaveFileName(this, tr("选择预设文件"),
                                                     prev_dir, "*.vc_preset");
  if (preset_file.isEmpty()) {
    return;
  }
  prev_dir = QFileInfo{preset_file}.dir().absolutePath();

  VCL_preset *preset = VCL_create_preset();

  for (const auto &it : this->map_VC_block_class) {
    if (it.second->chbox_enabled()->isChecked()) {
      VCL_preset_emplace_class(preset, it.first);
    }
    for (const auto &jt : it.second->blocks_vector()) {
      if (jt.second->isChecked()) {
        VCL_preset_emplace_id(preset, VCL_get_block_id(jt.first));
      }
    }
  }

  std::string err;
  err.resize(8192);
  VCL_string_deliver vsd{err.data(), err.size(), err.size()};
  const bool ok =
      VCL_save_preset(preset, preset_file.toLocal8Bit().data(), &vsd);
  err.resize(vsd.size);

  if (!ok) {
    QMessageBox::warning(
        this, tr("保存预设文件失败"),
        tr("无法保存预设文件，详细信息：\n%1").arg(err.c_str()));
    VCL_destroy_preset(preset);
    return;
  }

  VCL_destroy_preset(preset);
}