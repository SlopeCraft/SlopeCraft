#include <format>
#include <vector>
#include <optional>
#include <Eigen/Dense>
#include <map>

#include "GPUWrapper/GPU_interface.h"

#include <vulkan/vulkan.hpp>

#define VMA_VULKAN_VERSION 1001000  // Vulkan 1.1
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <vk_mem_alloc.hpp>

void write_error_code(int *errorcode_nullable, int error) noexcept {
  if (errorcode_nullable) {
    *errorcode_nullable = error;
  }
}

void write_error_code(int *errorcode_nullable, std::error_code error) noexcept {
  write_error_code(errorcode_nullable, error.value());
}

struct uuid_compare {
  static bool operator()(const std::array<uint64_t, 2> &a,
                         const std::array<uint64_t, 2> &b) {
    if (a[0] == b[0]) {
      return a[1] < b[1];
    }
    return a[0] < b[0];
  }
  // static uint64_t operator()(const std::array<uint64_t, 2> &a) {
  //   uint64_t hash = a[0];
  //   hash ^= a[1];
  //   return hash;
  // }
};

namespace gpu_wrapper {
const char *api_name() noexcept { return "Vulkan"; }

size_t platform_num() noexcept { return 1; }

class platform_impl : public platform_wrapper {
 public:
  vk::UniqueInstance instance;

  const char *name_v() const noexcept final { return "Vulkan"; }
  std::vector<vk::PhysicalDevice> physical_devices() const {
    auto devices_raw_list = instance->enumeratePhysicalDevices();

    std::map<std::array<uint8_t, 16>, vk::PhysicalDevice> device_set;
    for (auto &info : devices_raw_list) {
      vk::PhysicalDeviceProperties2 prop;
      vk::PhysicalDeviceIDProperties id_prop;
      prop.pNext = &id_prop;

      info.getProperties2(&prop);
      std::array<uint8_t, 16> uuid;
      uuid = id_prop.deviceUUID;
      device_set.emplace(uuid, info);
    }
    std::vector<vk::PhysicalDevice> ret;
    ret.reserve(device_set.size());
    for (auto &[_, info] : device_set) {
      ret.emplace_back(info);
    }

    return ret;
  }

  size_t num_devices_v() const noexcept { return physical_devices().size(); }
};

platform_wrapper *platform_wrapper::create(size_t idx [[maybe_unused]],
                                           int *errorcode
                                           [[maybe_unused]]) noexcept {
  vk::ApplicationInfo app_info{"VisualCraftL", VK_MAKE_VERSION(5, 0, 0),
                               "NoEngine", VK_MAKE_VERSION(1, 0, 0),
                               VK_API_VERSION_1_1};
  std::vector<const char *> layers, extensions;
#ifdef NDEBUG
#else
  layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif
  extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

  vk::InstanceCreateInfo ici{{}, &app_info, layers, extensions, {}};
  try {
    platform_impl *impl = new platform_impl();
    impl->instance = vk::createInstanceUnique(ici);
    write_error_code(errorcode, vk::Result::eSuccess);
    return impl;
  } catch (const vk::SystemError &err) {
    write_error_code(errorcode, err.code());
    return nullptr;
  }
}
void platform_wrapper::destroy(gpu_wrapper::platform_wrapper *p) noexcept {
  delete p;
}

class device_impl : public device_wrapper {
 public:
  std::string name;
  vk::PhysicalDevice physical_device;
  vk::UniqueDevice device;
  vma::UniqueAllocator allocator;
  vk::Queue queue;
  vk::UniqueCommandPool command_pool;
  vk::UniqueDescriptorPool descriptor_pool;
  vk::UniquePipelineCache pipeline_cache;

  const char *name_v() const noexcept { return name.c_str(); }
};

uint32_t select_queue_family_index(const vk::PhysicalDevice &pd) {
  auto queue_family_props = pd.getQueueFamilyProperties();
  std::vector<float> scores;
  scores.reserve(queue_family_props.size());
  for (uint32_t i = 0; i < queue_family_props.size(); i++) {
    const auto &prop = queue_family_props[i];
    float score = 0;
    const bool can_compute =
        static_cast<bool>(prop.queueFlags & vk::QueueFlagBits::eCompute);
    const bool can_graphics =
        static_cast<bool>(prop.queueFlags & vk::QueueFlagBits::eGraphics);
    if (can_compute) {
      score += 100;
    }
    if (not can_graphics) {
      score += 10;
    }
    scores.push_back(score);
  }
  assert(scores.size() == queue_family_props.size());

  auto best_it = std::max_element(scores.begin(), scores.end());
  const uint32_t selected_queue_family_index = best_it - scores.begin();
  assert(selected_queue_family_index < queue_family_props.size());
  if (*best_it <= 0) {
    throw vk::SystemError{
        vk::Result::eErrorUnknown,
        "Failed to find any queue family that support compute"};
  }

  return selected_queue_family_index;
}

void device_wrapper::destroy(device_wrapper *p) noexcept { delete p; }
device_wrapper *device_wrapper::create(platform_wrapper *pw, size_t idx,
                                       int *errorcode) noexcept {
  try {
    auto &platform = dynamic_cast<platform_impl &>(*pw);
    auto physical_device = platform.physical_devices()[idx];
    const uint32_t selected_queue_family_index =
        select_queue_family_index(physical_device);

    std::array<float, 1> queue_priorities{1.0f};
    vk::DeviceQueueCreateInfo qci{
        {}, selected_queue_family_index, queue_priorities};

    vk::PhysicalDeviceFeatures features{};
    features.setShaderInt16(true);
    vk::DeviceCreateInfo dci{{}, qci, {}, {}, &features};
    vk::PhysicalDeviceVulkan11Features features11;
    features11.setStorageBuffer16BitAccess(true);
    features11.setUniformAndStorageBuffer16BitAccess(true);
    dci.setPNext(&features11);

    auto device = physical_device.createDeviceUnique(dci);

    vma::UniqueAllocator alloc = vma::createAllocatorUnique(
        vma::AllocatorCreateInfo{{},
                                 physical_device,
                                 device.get(),
                                 {},
                                 {},
                                 {},
                                 {},
                                 {},
                                 platform.instance.get(),
                                 VK_API_VERSION_1_1});
    vk::UniquePipelineCache pipeline_cache =
        device->createPipelineCacheUnique(vk::PipelineCacheCreateInfo{});

    vk::Queue queue = device->getQueue(selected_queue_family_index, 0);

    vk::UniqueDescriptorPool descriptor_pool = [&]() {
      vk::DescriptorPoolSize pool_size{vk::DescriptorType::eStorageBuffer,
                                       1024};
      return device->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo{
          vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1024,
          pool_size});
    }();
    vk::UniqueCommandPool cmd_pool =
        device->createCommandPoolUnique(vk::CommandPoolCreateInfo{
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            selected_queue_family_index});

    device_impl *device_ptr = new device_impl{};
    device_ptr->name = physical_device.getProperties().deviceName.data();
    device_ptr->physical_device = physical_device;
    device_ptr->device = std::move(device);
    device_ptr->allocator = std::move(alloc);
    device_ptr->queue = queue;
    device_ptr->command_pool = std::move(cmd_pool);
    device_ptr->descriptor_pool = std::move(descriptor_pool);
    device_ptr->pipeline_cache = std::move(pipeline_cache);
    /*
    *
  std::string name;
  vk::PhysicalDevice physical_device;
  vk::UniqueDevice device;
  vma::UniqueAllocator allocator;
  vk::Queue queue;
  vk::UniqueCommandPool command_pool;
  vk::UniqueDescriptorPool descriptor_pool;
  vk::UniquePipelineCache pipeline_cache;
     *     */

    write_error_code(errorcode, vk::Result::eSuccess);
    return device_ptr;

  } catch (const vk::SystemError &e) {
    write_error_code(errorcode, e.code());
    return nullptr;
  }
}

class gpu_interface_impl : public gpu_interface {
 public:
  vk::Device device;
  std::error_code error_code{int(vk::Result::eSuccess), std::system_category()};

  const char *api_v() const noexcept { return "Vulkan"; }

  int error_code_v() const noexcept { return error_code_v(); }
  bool ok_v() const noexcept {
    return error_code.value() == int(vk::Result::eSuccess);
  }
  std::string error_detail_v() const noexcept { return error_code.message(); }

  void set_colorset_v(size_t color_num,
                      const std::array<const float *, 3> &color_ptrs) noexcept {
    abort();
  };
  ;

  void set_task_v(size_t task_num, const std::array<float, 3> *data) noexcept {
    abort();
  };
  ;

  void execute_v(::SCL_convertAlgo algo, bool wait) noexcept { abort(); };
  ;

  void wait_v() noexcept { abort(); };
  ;

  size_t task_count_v() const noexcept { abort(); };
  ;

  std::string device_vendor_v() const noexcept { abort(); };
  ;

  const uint16_t *result_idx_v() const noexcept { abort(); };
  ;

  const float *result_diff_v() const noexcept { abort(); };
  ;

  size_t local_work_group_size_v() const noexcept { abort(); };
};

void gpu_interface::destroy(gpu_interface *gi) noexcept { delete gi; }

gpu_interface *gpu_interface::create(
    platform_wrapper *pw [[maybe_unused]], device_wrapper *dw,
    std::pair<int, std::string> &err) noexcept {
  try {
    auto device = dynamic_cast<device_impl *>(dw);

    gpu_interface_impl *ret = new gpu_interface_impl{};
    ret->device = device->device.get();
    err.first = int(vk::Result::eSuccess);
    err.second = "";
    return ret;
  } catch (vk::SystemError &e) {
    err.first = e.code().value();
    err.second = e.what();
    return nullptr;
  }
}

}  // namespace gpu_wrapper
