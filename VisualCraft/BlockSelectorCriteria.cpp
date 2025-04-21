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

#include "BlockSelector.h"
#include "ui_BlockSelectorCriteria.h"
#include <magic_enum/magic_enum.hpp>

BlockSelectorCriteria::BlockSelectorCriteria(QWidget *parent)
    : QWidget(parent), ui(new Ui::BlockSelectorCriteria) {
  this->ui->setupUi(this);

  auto enum_list = magic_enum::enum_values<VCL_block_attribute_t>();

  for (auto val : enum_list) {
    const auto str = magic_enum::enum_name(val);

    this->ui->cb_attribute->addItem(QString(str.data()),
                                    QVariant::fromValue(val));
  }

  this->ui->cb_attribute->setCurrentIndex(
      size_t(VCL_block_attribute_t::reproducible));

  connect(this->ui->cb_attribute, &QComboBox::currentIndexChanged, this,
          &BlockSelectorCriteria::options_changed);
  connect(this->ui->cb_logic, &QComboBox::currentIndexChanged, this,
          &BlockSelectorCriteria::options_changed);
  connect(this->ui->cb_value, &QComboBox::currentIndexChanged, this,
          &BlockSelectorCriteria::options_changed);
}

BlockSelectorCriteria::~BlockSelectorCriteria() { delete this->ui; }

void BlockSelectorCriteria::set_role(bool is_first, bool is_last) noexcept {
  this->ui->cb_logic->setEnabled(!is_first);

  const bool is_the_only_one = (is_first && is_last);
  this->ui->tb_remove->setEnabled(!is_the_only_one);

  this->ui->tb_append->setEnabled(is_last);
}

void BlockSelectorCriteria::on_tb_append_clicked() noexcept {
  emit this->append();
}
void BlockSelectorCriteria::on_tb_remove_clicked() noexcept {
  emit this->remove(this);
}

void BlockSelectorCriteria::update_criteria(bs_criteria &cr) const noexcept {
  bs_criteria::statement stat;
  stat.logic_is_and =
      (this->ui->cb_logic->currentText() == QStringLiteral("AND"));
  stat.value = (this->ui->cb_value->currentText() == QStringLiteral("TRUE"));
  stat.key =
      this->ui->cb_attribute->currentData().value<VCL_block_attribute_t>();

  cr.statements.emplace_back(stat);
}

bool bs_criteria::match(const VCL_block *blk) const noexcept {
  assert(this->statements.size() > 0);

  bool value = true;

  for (auto stat : this->statements) {
    const bool val_current =
        (VCL_get_block_attribute(blk, stat.key) == stat.value);
    if (stat.logic_is_and) {
      value = value && val_current;
    } else {
      value = value || val_current;
    }
  }

  return value;
}