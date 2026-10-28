//
// Created by Joseph on 2026/10/28.
//

#include <QSettings>

#include "cpu_info.h"

std::optional<QString> get_cpu_name() noexcept {
  QSettings reg{
    R"(HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\CentralProcessor\0)",
    QSettings::NativeFormat};

  const auto name = reg.value("ProcessorNameString").toString().trimmed();
  if (reg.status() == QSettings::NoError) {
    return name;
  }
  return std::nullopt;
}