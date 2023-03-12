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

#ifndef SLOPECRAFT_VISUALCRAFT_VC_BLOCK_CLASS_H
#define SLOPECRAFT_VISUALCRAFT_VC_BLOCK_CLASS_H

#include <QGroupBox>
#include <VisualCraftL.h>
#include <utility>
#include <vector>

namespace Ui {
class VC_block_class;
}

class QCheckBox;

class VC_block_class : public QGroupBox {
  Q_OBJECT
private:
  Ui::VC_block_class *ui;
  std::vector<std::pair<VCL_block *, QCheckBox *>> blocks;

public:
  explicit VC_block_class(QWidget *parent);
  ~VC_block_class();

  void set_blocks(size_t num_blocks, VCL_block *const *const blocks,
                  size_t cols = 3) noexcept;

  size_t selected_blocks(std::vector<VCL_block *> *select_blks,
                         bool append_to_select_blks = false) const noexcept;

  const auto &blocks_vector() const noexcept { return this->blocks; }

  QCheckBox *chbox_enabled() noexcept;

private:
  void erase_blocks() noexcept;

  void set_state_for_all(bool checked) noexcept;

private slots:
};

#endif // SLOPECRAFT_VISUALCRAFT_VC_BLOCK_CLASS_H