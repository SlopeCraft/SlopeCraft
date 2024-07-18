//
// Created by Joseph on 2024/7/16.
//

#ifndef SLOPECRAFT_MEMORYPOLICYDIALOG_H
#define SLOPECRAFT_MEMORYPOLICYDIALOG_H

#include <memory>
#include <QDialog>

#include "stat_memory.h"

struct memory_policy {
  bool auto_cache{true};
  uint64_t self_maximum_memory{4ull << 30};
  double system_minimum_free{0.2};

  //  memory_policy() = default;

  [[nodiscard]] bool should_cache(
      const system_memory_info& smi) const noexcept {
    if (not this->auto_cache) {
      return false;
    }
    return double(smi.free) < smi.total * this->self_maximum_memory;
  }

  [[nodiscard]] bool should_cache(const self_memory_usage& smu) const noexcept {
    if (not this->auto_cache) {
      return false;
    }
    return smu.used > this->self_maximum_memory;
  }

  [[nodiscard]] bool should_cache() const noexcept {
    if (not this->auto_cache) {
      return false;
    }
    const auto sys_info_opt = get_self_memory_info();
    if (sys_info_opt and this->should_cache(sys_info_opt.value())) {
      return true;
    }
    const auto self_info_opt = get_self_memory_info();
    if (self_info_opt and this->should_cache(self_info_opt.value())) {
      return true;
    }
    return false;
  }
};

class MemoryPolicyDialog;
namespace Ui {
class MemoryPolicyDialog;
}

class MemoryPolicyDialog : public QDialog {
  Q_OBJECT
 private:
  std::unique_ptr<Ui::MemoryPolicyDialog> ui;

 public:
  explicit MemoryPolicyDialog(QWidget* parent);
  MemoryPolicyDialog(QWidget* parent, const memory_policy&);
  ~MemoryPolicyDialog();

  [[nodiscard]] memory_policy current_value() const noexcept;
  void reset(const memory_policy&) noexcept;

 private slots:
  void on_pb_ok_clicked() noexcept;
  void on_pb_cancel_clicked() noexcept;
  void on_pb_reset_clicked() noexcept;
};

#endif  // SLOPECRAFT_MEMORYPOLICYDIALOG_H
