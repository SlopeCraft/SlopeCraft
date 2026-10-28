//
// Created by Joseph on 2026/10/28.
//
#include <QList>
#include <QProcess>

#include "cpu_info.h"
std::optional<QString> get_cpu_name() noexcept {
  QProcess proc{nullptr};
  const auto command =
      QStringLiteral("sh -c \"cat /proc/cpuinfo | grep name\"");

  proc.start(command);
  if (!proc.waitForStarted()) {
    return std::nullopt;
  }
  if (!proc.waitForFinished()) {
    return std::nullopt;
  }

  QString output = QString::fromUtf8(proc.readAllStandardOutput());

  output.remove('\t');

  auto spilitted = output.split('\n');
  if (spilitted.size() <= 0) {
    return std::nullopt;
  }
  const auto line0 = spilitted[0];
  if (!line0.contains(':')) {
    return std::nullopt;
  }
  auto line0_splitted = line0.split(':');
  if (line0_splitted.size() != 2) {
    return std::nullopt;
  }
  if (line0_splitted[0].remove(' ') != "modelname") {
    return std::nullopt;
  }
  const auto cpu_name = line0_splitted[1];
  return cpu_name;
}