#include "VCWind.h"
#include "ui_VCWind.h"
#include <intrin.h>
#include <omp.h>
#include <thread>

void VCWind::refresh_gpu_info() noexcept {
  this->ui->sb_threads->setValue(std::thread::hardware_concurrency());

  this->ui->tw_gpu->clear();
  this->ui->combobox_select_device->clear();

  {
    std::array<int, 4> result;
    constexpr uint32_t input[3] = {0x80000002, 0x80000003, 0x80000004};

    char str[1024] = "";

    bool error = false;
    for (auto i : input) {
      try {
        __cpuid(result.data(), i);
      } catch (std::exception &e) {
        strcpy(str, "Instruction cpuid failed. Detail : ");
        strcpy(str, e.what());
        error = true;
        break;
      }
      strcat(str, reinterpret_cast<char *>(result.data()));
      // str += std::string_view();
    }

    QString text("CPU : " + QString::fromUtf8(str));
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
  // reload
  if (current_choice != this->prev_compute_device) {
    this->prev_compute_device = current_choice;

    const bool is_gpu = (current_choice.x() >= 0 && current_choice.y() >= 0);

    if (!is_gpu) {
      this->kernel->set_prefer_gpu(false);
      return;
    }

    this->kernel->set_prefer_gpu(true);
    this->kernel->set_gpu_resource(current_choice.x(), current_choice.y());
  }
}