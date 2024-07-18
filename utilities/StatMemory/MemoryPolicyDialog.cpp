//
// Created by Joseph on 2024/7/16.
//

#include <cmath>

#include "MemoryPolicyDialog.h"
#include "ui_MemoryPolicyDialog.h"

MemoryPolicyDialog::MemoryPolicyDialog(QWidget *parent)
    : QDialog{parent}, ui{new Ui::MemoryPolicyDialog} {
  this->ui->setupUi(this);
  this->reset(memory_policy{});
}

MemoryPolicyDialog::MemoryPolicyDialog(QWidget *parent,
                                       const memory_policy &value)
    : MemoryPolicyDialog{parent} {
  this->reset(value);
}

MemoryPolicyDialog::~MemoryPolicyDialog() { this->ui.reset(); }

memory_policy MemoryPolicyDialog::current_value() const noexcept {
  return memory_policy{
      .auto_cache = this->ui->cb_auto_cache->isChecked(),
      .self_maximum_memory =
          uint64_t(this->ui->sb_self_limit->value()) * 1024 * 1024 * 1024,
      .system_minimum_free = this->ui->dsb_system_limit->value(),
  };
}

void MemoryPolicyDialog::reset(const memory_policy &value) noexcept {
  this->ui->cb_auto_cache->setCheckState(
      value.auto_cache ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  this->ui->sb_self_limit->setValue(
      uint64_t(std::ceil(value.self_maximum_memory / (1024.0 * 1024 * 1024))));

  this->ui->dsb_system_limit->setValue(value.system_minimum_free);
}

void MemoryPolicyDialog::on_pb_ok_clicked() noexcept { emit this->accept(); }

void MemoryPolicyDialog::on_pb_cancel_clicked() noexcept {
  emit this->reject();
}

void MemoryPolicyDialog::on_pb_reset_clicked() noexcept {
  this->reset(memory_policy{});
}