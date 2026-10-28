//
// Created by Joseph on 2026/10/28.
//
#include <QProcess>
#include <QList>

#include "cpu_info.h"

std::optional<QString> get_cpu_name() noexcept {
  QProcess proc{nullptr};
  const auto command = QStringLiteral("sysctl machdep.cpu.brand_string");

  proc.start(command);
  if (!proc.waitForStarted()) {
    return std::nullopt;
  }
  if (!proc.waitForFinished()) {
    return std::nullopt;
  }

  QString output = QString::fromUtf8(proc.readAllStandardOutput());
  output.remove('\n');
  auto split_out = output.split(':');
  if (split_out.size() < 2) {
    return std::nullopt;
  }
  if (split_out[0] != "machdep.cpu.brand_string") {
    return std::nullopt;
  }
  auto cpu_name = split_out[1];
  cpu_name.remove('\t');
  while (cpu_name.front() == ' ') {
    cpu_name.removeFirst();
  }
  while (cpu_name.back() == ' ') {
    cpu_name.removeLast();
  }

  return cpu_name;
}