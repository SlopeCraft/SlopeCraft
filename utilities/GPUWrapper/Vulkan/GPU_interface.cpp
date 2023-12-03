
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>
#include <kompute/Kompute.hpp>
#include <utility>
#include <vector>
#include <span>
#include <mutex>
#include <optional>
#include <Eigen/Dense>
#include <queue>
#include <tl/expected.hpp>
#include "GPUWrapper/GPU_interface.h"

extern "C" {
extern const unsigned char VkComputeShaderSPIRV_rc[];
extern const unsigned int VkComputeShaderSPIRV_rc_length;
}

namespace gpu_wrapper {

enum class error_code : int {
  ok = 0,
  create_instance_failure = 2000000000,
  create_instance_failure_invalid_platform_index = 2000000001,
  create_physical_device_failure = 2000001000,
  create_logical_device_failure = 2000002000,
  create_logical_device_failure_no_suitable_queue_family = 2000002001,
  create_kompute_manager_failure = 2000003000,
  manager_set_colorset_failure = 2000004000,
  manager_set_tasks_failure = 2000005000,
  manager_execute_failure = 2000006000,
  manager_wait_failure = 2000007000,
};

struct error_pair {
  error_code code;
  std::string message;
};

void set_error_code(int* dst_nullable, error_code code) noexcept {
  if (dst_nullable) {
    *dst_nullable = int(code);
  }
}

constexpr uint32_t ceil_up_to(uint32_t num, uint32_t align) noexcept {
  if (num % align == 0) {
    return num;
  }

  return (num / align + 1) * align;
}

std::vector<uint32_t> get_spirv() noexcept {
  assert(VkComputeShaderSPIRV_rc_length % 4 == 0);
  auto data = reinterpret_cast<const uint32_t*>(VkComputeShaderSPIRV_rc);

  return std::vector<uint32_t>{
      data, data + size_t(VkComputeShaderSPIRV_rc_length / 4)};
}

tl::expected<std::shared_ptr<vk::Instance>, error_pair>
create_vk_instance() noexcept {
  vk::ApplicationInfo app_info{"VisualCraftL", VK_MAKE_VERSION(5, 0, 0),
                               "NoEngine", VK_MAKE_VERSION(1, 0, 0),
                               VK_API_VERSION_1_3};
  vk::InstanceCreateInfo ici;
  ici.pApplicationInfo = &app_info;
  std::array<const char*, 1> extension_names{
      VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
  };
  ici.enabledExtensionCount = extension_names.size();
  ici.ppEnabledExtensionNames = extension_names.data();

  std::array<const char*, 1> layer_names{
      //"VK_LAYER_LUNARG_assistant_layer",
      //"VK_LAYER_LUNARG_standard_validation",
      "VK_LAYER_KHRONOS_validation",
  };
  ici.enabledLayerCount = layer_names.size();
  ici.ppEnabledLayerNames = layer_names.data();

  try {
    auto instance_r = vk::createInstance(ici);
    auto instance = std::make_shared<vk::Instance>(instance_r);
    assert(instance != nullptr);
    return instance;

  } catch (const std::exception& e) {
    fmt::print("Failed to create vulkan instance, exception details: {}",
               e.what());
    return nullptr;
  }
}

const char* api_name() noexcept { return "Vulkan"; }

size_t platform_num() noexcept { return 1; }

class platform_impl : public platform_wrapper {
 public:
  platform_impl(std::shared_ptr<vk::Instance> i) : instance{i} {}
  std::shared_ptr<vk::Instance> instance;

  const char* name_v() const noexcept final { return "Vulkan"; }
  size_t num_devices_v() const noexcept final {
    return this->instance->enumeratePhysicalDevices().size();
  }
};

platform_wrapper* platform_wrapper::create(size_t idx,
                                           int* errorcode) noexcept {
  if (idx != 0) {
    set_error_code(errorcode,
                   error_code::create_instance_failure_invalid_platform_index);
    return nullptr;
  }
  auto i_exp = create_vk_instance();
  if (i_exp) {
    set_error_code(errorcode, error_code::ok);
    return new platform_impl{i_exp.value()};
  }
  set_error_code(errorcode, i_exp.error().code);
  return nullptr;
}

void platform_wrapper::destroy(gpu_wrapper::platform_wrapper* p) noexcept {
  delete p;
}

class device_impl : public device_wrapper {
 public:
  uint32_t device_index;
  std::shared_ptr<vk::PhysicalDevice> phy_device;
  vk::PhysicalDeviceProperties properties;
  std::shared_ptr<vk::Device> device;
  uint32_t selected_queue_family_index{};
  const char* name_v() const noexcept final {
    return this->properties.deviceName;
  }
};

std::optional<uint32_t> select_queue_family(
    std::span<const vk::QueueFamilyProperties> queue_family_props) noexcept {
  std::vector<uint32_t> indices;
  indices.reserve(queue_family_props.size());

  // for (auto [idx, family] : stdrange::enumerate_view{queue_family_props})
  for (size_t idx = 0; idx < queue_family_props.size(); idx++) {
    auto& family = queue_family_props[idx];
    auto flags = family.queueFlags;
    if (flags & vk::QueueFlagBits::eCompute) {
      indices.emplace_back(idx);
    }
  }
  if (indices.empty()) return std::nullopt;

  std::sort(indices.begin(), indices.end(),
            [queue_family_props](uint32_t i, uint32_t j) -> bool {
              const auto& a = queue_family_props[i];
              const auto& b = queue_family_props[j];
              if (a.queueFlags == b.queueFlags) {
                return a.queueCount > b.queueCount;
              }
              const auto compute_required_flags =
                  vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer;
              const int a_compute = bool(a.queueFlags & compute_required_flags);
              const int b_compute = bool(b.queueFlags & compute_required_flags);

              if (a_compute != b_compute) {
                if (a_compute)
                  return true;
                else
                  return false;
              }

              assert(a_compute + b_compute == 2);

              const int a_graphics =
                  bool(a.queueFlags & vk::QueueFlagBits::eGraphics);
              const int b_graphics =
                  bool(b.queueFlags & vk::QueueFlagBits::eGraphics);

              if (a_graphics != b_graphics) {
                if (a_graphics)
                  return false;
                else
                  return true;
              }

              return a.queueCount > b.queueCount;
            });

  return indices.front();
}

device_wrapper* device_wrapper::create(gpu_wrapper::platform_wrapper* pw,
                                       size_t idx, int* ec) noexcept {
  auto& plat = dynamic_cast<platform_impl&>(*pw);
  std::vector<vk::PhysicalDevice> devices;
  try {
    devices = plat.instance->enumeratePhysicalDevices();
  } catch (const std::exception& e) {
    fmt::println("Failed to enumerate physical devices, detail: {}", e.what());
    set_error_code(ec, error_code::create_physical_device_failure);
    return nullptr;
  }
  try {
    auto result = new device_impl;
    result->device_index = idx;
    result->phy_device = std::make_shared<vk::PhysicalDevice>(devices[idx]);
    auto& phy_device = *result->phy_device;
    result->properties = phy_device.getProperties();
    {
      auto qf_props = phy_device.getQueueFamilyProperties();

      auto sqfi_opt = select_queue_family(qf_props);
      if (!sqfi_opt) {
        set_error_code(
            ec,
            error_code::create_logical_device_failure_no_suitable_queue_family);
        delete result;
        return nullptr;
      }
      result->selected_queue_family_index = sqfi_opt.value();
    }
    {
      vk::DeviceCreateInfo dci{};
      std::array<const char*, 2> extensions{
          VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
          VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
      };
      dci.enabledExtensionCount = extensions.size();
      dci.ppEnabledExtensionNames = extensions.data();

      // check if 16 bit support is fully ok
      vk::PhysicalDeviceFeatures2 pdf2;
      vk::PhysicalDeviceVulkan11Features pdfv11;
      pdf2.pNext = &pdfv11;
      phy_device.getFeatures2(&pdf2);
      assert(pdf2.features.shaderInt16);
      assert(pdfv11.uniformAndStorageBuffer16BitAccess);
      assert(pdfv11.storageBuffer16BitAccess);
      dci.pEnabledFeatures = nullptr;
      dci.pNext = &pdf2;

      // queue info
      vk::DeviceQueueCreateInfo dqci;
      dqci.queueCount = 1;
      constexpr float prior = 1.0f;
      dqci.pQueuePriorities = &prior;
      dqci.queueFamilyIndex = result->selected_queue_family_index;

      dci.pQueueCreateInfos = &dqci;
      dci.queueCreateInfoCount = 1;

      try {
        result->device =
            std::make_shared<vk::Device>(phy_device.createDevice(dci));
      } catch (const std::exception& e) {
        fmt::println("Failed to create logical device, details: {}", e.what());
        set_error_code(ec, error_code::create_logical_device_failure);
        delete result;
        return nullptr;
      }
    }

    set_error_code(ec, error_code::ok);
    return result;
  } catch (const std::exception& e) {
    fmt::println("Failed to create logical device because {}", e.what());
    set_error_code(ec, error_code::create_logical_device_failure);
    return nullptr;
  } catch (...) {
    fmt::println("Failed to create logical device because unknown exception.");
    set_error_code(ec, error_code::create_logical_device_failure);
    return nullptr;
  }
}

void device_wrapper::destroy(gpu_wrapper::device_wrapper* dw) noexcept {
  delete dw;
}

struct task_option {
  uint32_t task_num;
  uint32_t color_count;
  uint32_t algo;
};

class gpu_impl : public gpu_interface {
 private:
  kp::Manager manager;

  // Its size won't be changed
  std::shared_ptr<kp::TensorT<task_option>> compute_option;
  // Their sizes may be changed
  std::shared_ptr<kp::TensorT<float>> colorset;
  // std::vector<float> colorset_host;
  std::shared_ptr<kp::TensorT<float>> tasks;
  // std::vector<float> tasks_host;
  std::shared_ptr<kp::TensorT<uint16_t>> result_idx;
  // std::vector<uint16_t> result_idx_host;
  std::shared_ptr<kp::TensorT<float>> result_diff;
  // std::vector<float> result_diff_host;

  std::shared_ptr<kp::Algorithm> algorithm;

  std::shared_ptr<kp::Sequence> sequence;

  size_t task_count{UINT64_MAX};
  uint16_t color_count{UINT16_MAX};

  error_pair error{error_code::ok, ""};

  template <typename T>
  void resize_tensor_size(std::shared_ptr<kp::TensorT<T>> tensor,
                          size_t required_size) {
    const size_t cur_size = tensor->size();
    if (cur_size >= required_size) {
      return;
    }

    tensor->rebuild(nullptr, required_size, sizeof(T));
  }

 public:
  gpu_impl(platform_impl& pi, device_impl& di)
      : manager{pi.instance, di.phy_device, di.device} {
    // setup compute queues that ought to be setup by manager constructor
    this->manager.computeQueueFamilyIndices() = {
        di.selected_queue_family_index};
    this->manager.computeQueues() = {std::make_shared<vk::Queue>(
        di.device->getQueue(di.selected_queue_family_index, 0))};

    this->compute_option = this->manager.tensorT<task_option>({task_option{}});

    this->colorset =
        this->manager.tensorT<float>({0}, kp::Tensor::TensorTypes::eDevice);
    this->tasks =
        this->manager.tensorT<float>({0}, kp::Tensor::TensorTypes::eDevice);
    this->result_diff =
        this->manager.tensorT<float>({0}, kp::Tensor::TensorTypes::eDevice);
    this->result_idx =
        this->manager.tensorT<uint16_t>({0}, kp::Tensor::TensorTypes::eDevice);

    this->algorithm = this->manager.algorithm(
        {this->compute_option, this->colorset, this->tasks, this->result_diff,
         this->result_idx},
        get_spirv());

    this->sequence = this->manager.sequence();
  }

  const char* api_v() const noexcept final { return "Vulkan"; }

  void set_colorset_v(
      size_t color_num,
      const std::array<const float*, 3>& color_ptrs) noexcept final {
    try {
      this->color_count = color_num;
      this->resize_tensor_size(this->colorset, 3 * color_num);
      // this->colorset_host.resize(3 * color_num);

      Eigen::Map<
          Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
          trans{this->colorset->data(), static_cast<int64_t>(color_num), 3};

      for (size_t c = 0; c < 3; c++) {
        for (size_t r = 0; r < color_num; r++) {
          trans(r, c) = color_ptrs[c][r];
        }
      }
    } catch (const std::exception& e) {
      this->error.code = error_code::manager_set_colorset_failure;
      this->error.message =
          fmt::format("Failed to set colorset, detail: {}", e.what());
    }
  }

  void set_task_v(size_t task_num,
                  const std::array<float, 3>* data) noexcept final {
    try {
      this->task_count = task_num;
      this->resize_tensor_size(this->tasks, task_num * 3);
      this->resize_tensor_size(this->result_idx, task_num);
      this->resize_tensor_size(this->result_diff, task_num);

      memcpy(this->tasks->data(), data, task_num * sizeof(float[3]));
    } catch (const std::exception& e) {
      this->error.code = error_code::manager_set_tasks_failure;
      this->error.message =
          fmt::format("Failed to set tasks, detail: {}", e.what());
    }
  }

  void wait_v() noexcept final {
    try {
      this->sequence->eval();
    } catch (const std::exception& e) {
      this->error.code = error_code::manager_wait_failure;
      this->error.message =
          fmt::format("Failed to wait for results, detail: {}", e.what());
    }
    //    fmt::println("Computation result: [");
    //    for (uint32_t i = 0; i < this->task_count; i++) {
    //      fmt::println("\t[task {}, result idx = {}, result diff = {}]", i,
    //                   this->result_idx->data()[i],
    //                   this->result_diff->data()[i]);
    //    }
    //    fmt::println("]");
  }

  void execute_v(::SCL_convertAlgo algo, bool wait) noexcept final {
    try {
      task_option option{uint32_t(this->task_count), this->color_count,
                         uint32_t(algo)};
      this->compute_option->setData({option});

      this->sequence->record<kp::OpTensorSyncDevice>(
          {this->compute_option, this->colorset, this->tasks});

      const uint32_t local_wg_size = this->local_work_group_size_v();

      const uint32_t work_group_num =
          ceil_up_to(this->task_count, local_wg_size) / local_wg_size;
      this->algorithm->rebuild(
          {this->compute_option, this->colorset, this->tasks, this->result_diff,
           this->result_idx},
          get_spirv(), {work_group_num, 1, 1});

      this->sequence->record<kp::OpAlgoDispatch>(this->algorithm)
          ->record<kp::OpTensorSyncLocal>(
              {this->result_diff, this->result_idx});

      if (wait) {
        this->wait_v();
      }
    } catch (const std::exception& e) {
      this->error.code = error_code::manager_execute_failure;
      this->error.message =
          fmt::format("Failed to execute computation, detail: {}", e.what());
    }
  }

  size_t task_count_v() const noexcept final { return this->task_count; }
  std::string device_vendor_v() const noexcept final {
    return this->manager.getDeviceProperties().deviceName;
  }
  const uint16_t* result_idx_v() const noexcept final {
    return this->result_idx->data();
  }
  const float* result_diff_v() const noexcept final {
    return this->result_diff->data();
  }
  size_t local_work_group_size_v() const noexcept { return 64; }

  // error handling
  int error_code_v() const noexcept final { return int(this->error.code); }
  bool ok_v() const noexcept final {
    return (this->error.code == error_code::ok);
  }
  std::string error_detail_v() const noexcept final {
    return this->error.message;
  }
};

gpu_interface* gpu_interface::create(gpu_wrapper::platform_wrapper* pw,
                                     gpu_wrapper::device_wrapper* dw) noexcept {
  std::pair<int, std::string> temp;
  return create(pw, dw, temp);
}
gpu_interface* gpu_interface::create(
    platform_wrapper* pw, device_wrapper* dw,
    std::pair<int, std::string>& err) noexcept {
  auto p = dynamic_cast<platform_impl*>(pw);
  auto d = dynamic_cast<device_impl*>(dw);
  err.first = 0;
  // err.second.clear();
  auto result = new gpu_impl{*p, *d};
  err.first = result->error_code_v();
  err.second = result->error_detail_v();
  if (err.first != 0) {
    delete result;
    return nullptr;
  }
  return result;
}

void gpu_interface::destroy(gpu_wrapper::gpu_interface* gi) noexcept {
  delete gi;
}
}  // namespace gpu_wrapper