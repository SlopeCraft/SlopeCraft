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

#include <omp.h>

#include <iostream>
#include <thread>

#include <QMessageBox>
#include <QProcess>
#include "VCWind.h"
#include "ui_VCWind.h"

QString get_cpu_name(bool &error) noexcept {
  QProcess proc{0};
  error = true;

#ifdef WIN32
  const char *command = "wmic cpu get name";
#elif defined(__linux__)
  const char *command = "sh -c \"cat /proc/cpuinfo | grep name\"";
#elif defined(__APPLE__)
  const char * command="sysctl machdep.cpu.brand_string";
#else
#warning Unknown OS
  const char * command=nullptr;
  return {};
#endif

  proc.startCommand(command);
  if (!proc.waitForStarted()) {
    return {};
  }
  if (!proc.waitForFinished()) {
    return {};
  }

  QString output = QString::fromUtf8(proc.readAllStandardOutput());

#ifdef WIN32
  output = output.remove(QChar{'\r'});
  auto splitted = output.split('\n');
  if (splitted.size() < 2) {
    return {};
  }

  if (splitted[0].remove(QChar{' '}) != QStringLiteral("Name")) {
    return {};
  }

  const auto cpu_name = splitted[1];

#elif defined(__linux__)

  output.remove('\t');

  auto spilitted = output.split('\n');
  if (spilitted.size() <= 0) {
    return {};
  }
  const auto line0 = spilitted[0];
  if (!line0.contains(':')) {
    return {};
  }
  auto line0_splitted = line0.split(':');
  if (line0_splitted.size() != 2) {
    return {};
  }
  if (line0_splitted[0].remove(' ') != "modelname") {
    return {};
  }
  const auto cpu_name = line0_splitted[1];

#elif defined(__APPLE__)
  output.remove('\n');
  auto split_out=output.split(':');
  if (split_out.size()<2) {
    return {};
  }
  if (split_out[0]!="machdep.cpu.brand_string") {
    return {};
  }
  auto cpu_name=split_out[1];
  cpu_name.remove('\t');
  while(cpu_name.front()==' ') {
    cpu_name.removeFirst();
  }
  while(cpu_name.back()==' ') {
    cpu_name.removeLast();
  }
#else
#warning "Unknown OS"
  QString cpu_name{};
#endif
  error = false;
  return cpu_name;
}

void VCWind::refresh_gpu_info() noexcept {
  this->ui->sb_threads->setValue((int)std::thread::hardware_concurrency());

  this->ui->tw_gpu->clear();
  this->ui->combobox_select_device->clear();

  {
    bool error = false;
    QString text("CPU : " + get_cpu_name(error));
    // QString text("CPU : " +);
    QTreeWidgetItem *cpu = new QTreeWidgetItem;
    cpu->setText(0, text);
    if (error) {
      cpu->setFlags(Qt::ItemFlag::NoItemFlags);
    }
    this->ui->tw_gpu->addTopLevelItem(cpu);

    // x means platform idx, and y means device idx. -1 means cpu.
    this->ui->combobox_select_device->addItem(text, QPoint(-1, -1));

    // cpu->setCheckState(0, Qt::CheckState::Unchecked);
  }

  this->ui->gb_gpu->setTitle(VCWind::tr("可用的计算设备（CPU + %1可用的显卡）")
                                 .arg(VCL_get_GPU_api_name()));

  const size_t num_plats = VCL_platform_num();
  int errcode = 0;

  for (size_t pid = 0; pid < num_plats; pid++) {
    VCL_GPU_Platform *plat = VCL_get_platform(pid, &errcode);
    QString platname{};
    if (plat == nullptr) {
      platname =
          VCWind::tr("无法获取 platform 信息。请检查驱动。OpenCL 错误码：%1.")
              .arg(errcode);
    } else {
      platname = QString::fromLocal8Bit(VCL_get_platform_name(plat));
    }

    QTreeWidgetItem *twi = new QTreeWidgetItem;
    twi->setText(0, platname);
    // twi->setFlags(Qt::ItemFlag::Item);

    this->ui->tw_gpu->addTopLevelItem(twi);

    if (plat == nullptr) {
      twi->setFlags(Qt::ItemFlag::NoItemFlags);
      continue;
    }
    size_t num_devs = VCL_get_device_num(plat);

    for (size_t did = 0; did < num_devs; did++) {
      VCL_GPU_Device *dev = VCL_get_device(plat, did, &errcode);
      QString devicename{};
      if (dev == nullptr) {
        devicename =
            VCWind::tr("无法获取 device 信息。请检查驱动。OpenCL 错误码：%1")
                .arg(errcode);
      } else {
        devicename = QString::fromLocal8Bit(VCL_get_device_name(dev));
      }

      QTreeWidgetItem *twi_device = new QTreeWidgetItem;
      twi_device->setText(0, devicename);

      twi->addChild(twi_device);

      this->ui->combobox_select_device->addItem(
          QStringLiteral("%1 / %2").arg(platname, devicename),
          QPoint(pid, did));

      VCL_release_device(dev);
    }
    twi->setExpanded(true);
    VCL_release_platform(plat);
  }
}

void VCWind::select_default_device() noexcept {
  if (this->ui->combobox_select_device->count() <= 0) {
    abort();
  }
  int selected_idx = 0;
  for (int idx = 0; idx < this->ui->combobox_select_device->count(); idx++) {
    const auto &data = this->ui->combobox_select_device->itemData(idx);

    const QPoint &point = data.toPoint();
    // is cpu. Use the first gpu if possible.
    if (point.x() < 0 || point.y() < 0) {
      continue;
    } else {
      selected_idx = idx;
      break;
    }
  }

  this->ui->combobox_select_device->setCurrentIndex(selected_idx);
}

void VCWind::on_sb_threads_valueChanged(int val) noexcept {
  omp_set_num_threads(val);
}

void VCWind::on_combobox_select_device_currentIndexChanged(int idx) noexcept {
  const QPoint current_choice =
      this->ui->combobox_select_device->itemData(idx).toPoint();

  if (current_choice == this->prev_compute_device) {
    return;
  }

  const QString err = this->update_gpu_device(current_choice);

  if (!err.isEmpty()) {
    auto ret = QMessageBox::critical(
        this, VCWind::tr("设置计算设备失败"),
        tr("%1\n\n这不是一个致命错误，您可以选择其他的显卡，或者只使用 CPU "
           "计算。点击 Ignore 将忽略这个错误，点击 Close 将关闭 VisualCraft")
            .arg(err),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                     QMessageBox::StandardButton::Ignore});
    if (ret == QMessageBox::StandardButton::Close) {
      exit(1);
    }
  }
}

QString VCWind::update_gpu_device(QPoint current_choice) noexcept {
  // reload
  this->prev_compute_device = current_choice;

  const bool is_gpu = (current_choice.x() >= 0 && current_choice.y() >= 0);

  if (!is_gpu) {
    this->kernel->set_prefer_gpu(false);
    return {};
  }

  this->kernel->set_prefer_gpu(true);

  VCL_GPU_Platform *plat = VCL_get_platform(current_choice.x());
  if (plat == nullptr) {
    return VCWind::tr("创建 GPU 平台失败，平台序号为%1，设备序号为%2")
        .arg(current_choice.x())
        .arg(current_choice.y());
  }

  VCL_GPU_Device *dev = VCL_get_device(plat, current_choice.y());
  if (dev == nullptr) {
    return VCWind::tr("创建 GPU 设备失败，平台序号为%1，设备序号为%2")
        .arg(current_choice.x())
        .arg(current_choice.y());
  }
  {
    std::string error_msg;
    error_msg.resize(4096);
    VCL_Kernel::gpu_options gpu_options;
    VCL_string_deliver s{
        .data = error_msg.data(), .size = 0, .capacity = error_msg.size()};
    gpu_options.error_message = &s;
    if (!this->kernel->set_gpu_resource(plat, dev, gpu_options)) {
      error_msg.resize(s.size);
      return VCWind::tr(
                 "设置 GPU "
                 "设备失败。平台序号为%1，设备序号为%2，详细错误信息：\n%3")
          .arg(current_choice.x())
          .arg(current_choice.y())
          .arg(error_msg.data());
    }
  }

  VCL_release_device(dev);
  VCL_release_platform(plat);

  return {};
}