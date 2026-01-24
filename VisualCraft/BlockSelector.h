/*
 Copyright © 2021-2026  TokiNoBug
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

#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKSELECTOR_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKSELECTOR_H

#include <QWidget>
#include <VisualCraftL.h>
#include <vector>

class BlockSelector;
class BlockSelectorCriteria;

namespace Ui {
class BlockSelector;
class BlockSelectorCriteria;
}  // namespace Ui

class bs_criteria {
 public:
  struct alignas(4) statement {
    bool logic_is_and{true};
    VCL_block_attribute_t key;
    bool value;
  };

  std::vector<statement> statements;

  bool match(const VCL_block *blk) const noexcept;
};

class BlockSelector : public QWidget {
  Q_OBJECT
 private:
  Ui::BlockSelector *ui;

  std::vector<BlockSelectorCriteria *> criterias;

 public:
  BlockSelector(QWidget *parent);
  ~BlockSelector();

 private:
  void update_criteria_roles() noexcept;

  std::function<bool(const VCL_block *)> match_functor() const noexcept;

 private slots:
  void emplace_back() noexcept;

  void remove_one(BlockSelectorCriteria *) noexcept;

  void when_criteria_changed() noexcept;

  void on_pb_select_matched_clicked() noexcept;
  void on_pb_deselect_matched_clicked() noexcept;
};

class BlockSelectorCriteria : public QWidget {
  Q_OBJECT
 private:
  Ui::BlockSelectorCriteria *ui;

 signals:
  void options_changed();

  void append();
  void remove(BlockSelectorCriteria *);

 public:
  BlockSelectorCriteria(QWidget *parent);
  ~BlockSelectorCriteria();

  void set_role(bool is_first, bool is_last) noexcept;

  void update_criteria(bs_criteria &cr) const noexcept;

 private slots:
  // auto connected
  void on_tb_append_clicked() noexcept;
  void on_tb_remove_clicked() noexcept;
};

#endif  // SLOPECRAFT_VISUALCRAFT_BLOCKSELECTOR_H