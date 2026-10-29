//
// Created by Joseph on 2026/10/28.
//
#include <QList>
#include <QRegularExpression>
#include <QFile>

#include "cpu_info.h"
std::optional<QString> get_cpu_name() noexcept {
  QFile file("/proc/cpuinfo");

  if (not file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return std::nullopt;
  }

  const auto lines = QString::fromUtf8(file.readAll()).split('\n');
  // Expect cpu name contains whitespace and non-empty chars
  const QRegularExpression cpu_re(R"(^model name[\s]*:[\s]*([\S ]+)[\s]*$)");
  assert(cpu_re.isValid());
  if (not cpu_re.isValid()) {
    return std::nullopt;
  }

  for (const auto &line : lines) {
    auto match = cpu_re.match(line);
    if (not match.hasMatch()) {
      continue;
    }

    return match.captured(1);
  }

  return std::nullopt;
}