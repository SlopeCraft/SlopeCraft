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

#include <cpuid.h>
#include <omp.h>

#include <QMessageBox>
#include <iostream>
#include <thread>

#include "VCWind.h"
#include "ui_VCWind.h"

std::string get_cpu_name(bool &error) noexcept {
  int buffer[5];
  uint8_t *const buffer_cptr = reinterpret_cast<uint8_t *>(buffer);
  constexpr uint32_t input[3] = {0x80000002, 0x80000003, 0x80000004};

  char str[1024] = "";

  error = false;
  for (auto i : input) {
    memset(buffer, 0, sizeof(buffer));
    try {
      __cpuid(i, buffer[0], buffer[1], buffer[2], buffer[3]);
    } catch (std::exception &e) {
      strcpy(str, "Instruction cpuid failed. Detail : ");
      strcpy(str, e.what());
      error = true;
      return str;
    }
    if constexpr (false) {
      for (size_t o = 0; o < 4 * sizeof(uint32_t); o++) {
        std::cout << (int)buffer_cptr[o] << ", ";
      }
      std::cout << std::endl;
    }

    strcat(str, reinterpret_cast<char *>(buffer_cptr));
  }
  return str;
}

void VCWind::refresh_gpu_info() noexcept {
  this->ui->sb_threads->setValue(std::thread::hardware_concurrency());

  this->ui->tw_gpu->clear();
  this->ui->combobox_select_device->clear();

  {
    bool error = false;
    QString text("CPU : " + QString::fromUtf8(get_cpu_name(error)));
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
          VCWind::tr("无法获取platform信息. 请检查驱动. OpenCL错误码: %1.")
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
            VCWind::tr("无法获取device信息. 请检查驱动. OpenCL错误码: %1")
                .arg(errcode);
      } else {
        devicename = QString::fromLocal8Bit(VCL_get_device_name(dev));
      }

      QTreeWidgetItem *twi_device = new QTreeWidgetItem;
      twi_device->setText(0, devicename);

      twi->addChild(twi_device);

      this->ui->combobox_select_device->addItem(devicename, QPoint(pid, did));

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
        this, VCWind::tr("设置计算设备失败"), err,
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Close,
                                     QMessageBox::StandardButton::Ignore});
    if (ret == QMessageBox::StandardButton::Close) {
      abort();
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
    return VCWind::tr("创建GPU平台失败，平台序号为%1，设备序号为%2")
        .arg(current_choice.x())
        .arg(current_choice.y());
  }

  VCL_GPU_Device *dev = VCL_get_device(plat, current_choice.y());
  if (dev == nullptr) {
    return VCWind::tr("创建GPU设备失败，平台序号为%1，设备序号为%2")
        .arg(current_choice.x())
        .arg(current_choice.y());
  }

  if (!this->kernel->set_gpu_resource(plat, dev)) {
    return VCWind::tr("设置GPU设备失败。，平台序号为%1，设备序号为%2")
        .arg(current_choice.x())
        .arg(current_choice.y());
  }

  VCL_release_device(dev);
  VCL_release_platform(plat);

  return {};
}