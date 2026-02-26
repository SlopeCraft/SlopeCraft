/*
Copyright © 2021-2026  TokiNoBug
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
#include <format>
#include <vector>
#include <optional>
#include <variant>
#include <map>
#include <algorithm>

#include <Eigen/Dense>

#include "GPUWrapper/GPU_interface.h"

#include <vulkan/vulkan.hpp>

#define VMA_VULKAN_VERSION 1003000  // Vulkan 1.1
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <vk_mem_alloc.hpp>

#include "shader_spv.h"

void write_error_code(int *errorcode_nullable, int error) noexcept {
  if (errorcode_nullable) {
    *errorcode_nullable = error;
  }
}

void write_error_code(int *errorcode_nullable, std::error_code error) noexcept {
  write_error_code(errorcode_nullable, error.value());
}

namespace gpu_wrapper {
const char *api_name() noexcept { return "Vulkan"; }

size_t platform_num() noexcept { return 1; }

float device_score(const vk::PhysicalDevice &dev) {
  const auto prop = dev.getProperties();
  const auto mem_props = dev.getMemoryProperties();
  constexpr float big = 1e5;
  float score = 0;
  switch (prop.deviceType) {
    case vk::PhysicalDeviceType::eDiscreteGpu:
      score += 2 * big;
      break;
    case vk::PhysicalDeviceType::eIntegratedGpu:
      score += 1 * big;
      break;
    default:
      break;
  }
  size_t total_device_memory_size = 0;
  for (const auto &heap_info : mem_props.memoryHeaps) {
    if (heap_info.flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
      total_device_memory_size += heap_info.size;
    }
  }

  const float memory_score =
      1 - std::exp(-static_cast<float>(total_device_memory_size) / 1e10f);
  assert(memory_score >= 0);
  assert(memory_score <= 1);
  score += big * memory_score;
  return score;
}

bool compare_device(const vk::PhysicalDevice &a, const vk::PhysicalDevice &b) {
  return device_score(a) > device_score(b);
}

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

    std::sort(ret.begin(), ret.end(), compare_device);

    return ret;
  }

  size_t num_devices_v() const noexcept final {
    return physical_devices().size();
  }
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

  try {
    std::array features = {vk::ValidationFeatureEnableEXT::eDebugPrintf};
    vk::InstanceCreateInfo ici{{}, &app_info, layers, extensions, {}};

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

  const char *name_v() const noexcept final { return name.c_str(); }
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

    std::vector<const char *> device_extensions{
        "VK_KHR_synchronization2",
        "VK_KHR_maintenance4",
    };

    vk::PhysicalDeviceFeatures features{};
    features.setShaderInt16(true);
    vk::DeviceCreateInfo dci{{}, qci, {}, device_extensions, &features};
    vk::PhysicalDeviceVulkan11Features features11;
    dci.setPNext(&features11);
    features11.setStorageBuffer16BitAccess(true);
    features11.setUniformAndStorageBuffer16BitAccess(true);

    vk::PhysicalDeviceSynchronization2Features feature_sync2;
    features11.setPNext(&feature_sync2);
    feature_sync2.setSynchronization2(true);

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
                                 VK_API_VERSION_1_3});
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

    write_error_code(errorcode, vk::Result::eSuccess);
    return device_ptr;

  } catch (const vk::SystemError &e) {
    write_error_code(errorcode, e.code());
    return nullptr;
  }
}

struct buffer_with_alloc {
  vma::UniqueBuffer buffer;
  vma::UniqueAllocation alloc;
  vma::AllocationInfo allocation_info;
  size_t size_in_bytes;
};
buffer_with_alloc allocate(vma::Allocator allocator, vk::BufferCreateInfo bci,
                           vma::AllocationCreateInfo aci, size_t bytes) {
  bci.size = bytes;
  vma::AllocationInfo allocation_info;
  auto [alloc, buffer] =
      allocator.createBufferUnique(bci, aci, allocation_info);
  buffer_with_alloc ret;
  ret.buffer = std::move(buffer);
  ret.alloc = std::move(alloc);
  ret.size_in_bytes = bytes;
  ret.allocation_info = allocation_info;
  return ret;
}

bool support_mapping_device_memory(vma::Allocator allocator) {
  auto mem_type_props = allocator.getMemoryProperties();
  const auto expected_flags = vk::MemoryPropertyFlagBits::eHostVisible |
                              vk::MemoryPropertyFlagBits::eHostCoherent |
                              vk::MemoryPropertyFlagBits::eDeviceLocal;
  for (const auto &prop : mem_type_props->memoryTypes) {
    if (prop.propertyFlags & expected_flags) {
      return true;
    }
  }
  return false;
}

/// Get template of buffer create info and allocation create info
std::pair<vk::BufferCreateInfo, vma::AllocationCreateInfo> get_alloc_template(
    bool is_device_buffer, bool require_staging) {
  if (not is_device_buffer) {  // host staging buffer
    return {vk::BufferCreateInfo{{},
                                 0,
                                 vk::BufferUsageFlagBits::eTransferSrc |
                                     vk::BufferUsageFlagBits::eTransferDst |
                                     vk::BufferUsageFlagBits::eStorageBuffer,
                                 vk::SharingMode::eExclusive,
                                 vk::QueueFamilyIgnored},
            vma::AllocationCreateInfo{
                vma::AllocationCreateFlagBits::eHostAccessRandom,
                vma::MemoryUsage::eAutoPreferHost,
                vk::MemoryPropertyFlagBits::eHostVisible |
                    vk::MemoryPropertyFlagBits::eHostCoherent,
                {}}};
  }

  if (require_staging) {  // device local buffer, not mappable
    return {vk::BufferCreateInfo{{},
                                 0,
                                 vk::BufferUsageFlagBits::eTransferSrc |
                                     vk::BufferUsageFlagBits::eTransferDst |
                                     vk::BufferUsageFlagBits::eStorageBuffer,
                                 vk::SharingMode::eExclusive,
                                 vk::QueueFamilyIgnored},
            vma::AllocationCreateInfo{
                vma::AllocationCreateFlagBits::eHostAccessRandom |
                    vma::AllocationCreateFlagBits::eMapped,
                vma::MemoryUsage::eGpuOnly,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                {}}};
  }
  // device local buffer, mappable
  return {vk::BufferCreateInfo{{},
                               0,
                               vk::BufferUsageFlagBits::eTransferSrc |
                                   vk::BufferUsageFlagBits::eTransferDst |
                                   vk::BufferUsageFlagBits::eStorageBuffer,
                               vk::SharingMode::eExclusive,
                               vk::QueueFamilyIgnored},
          vma::AllocationCreateInfo{
              vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
              vma::MemoryUsage::eAutoPreferDevice,
              vk::MemoryPropertyFlagBits::eDeviceLocal |
                  vk::MemoryPropertyFlagBits::eHostVisible |
                  vk::MemoryPropertyFlagBits::eHostCoherent,
              {}}};
}

constexpr inline uint32_t divup(uint32_t a, uint32_t b) {
  return (a + b - 1) / b;
}

struct required_buffers {
  buffer_with_alloc colorset;
  buffer_with_alloc task;
  buffer_with_alloc result_index;
  buffer_with_alloc result_diff;
};

struct cpu_results {
  std::vector<uint16_t> result_index;
  std::vector<float> result_diff;
};

struct color_diff_push_const {
  alignas(uint32_t) uint32_t task_num;
  alignas(uint32_t) uint32_t colorset_size;
  alignas(uint32_t) uint32_t algo;
};

class gpu_interface_impl : public gpu_interface {
 public:
  // references
  vk::PhysicalDevice physical_device;
  vk::Device device;
  vk::DescriptorPool descriptor_pool;
  vk::Queue queue;
  vma::Allocator allocator;
  // error code
  std::error_code error_code_{static_cast<int>(vk::Result::eSuccess),
                              std::system_category()};
  // buffers
  required_buffers device_buf;
  std::variant<required_buffers, cpu_results> host_buf;
  // Pipeline items
  vk::UniquePipeline pipeline;
  vk::UniquePipelineLayout pipeline_layout;
  vk::UniqueDescriptorSetLayout descriptor_set_layout;
  // Descriptor set
  vk::UniqueDescriptorSet descriptor_set;
  // command buffer and fence
  vk::UniqueFence fence;
  vk::UniqueCommandBuffer cmd_buffer;

  uint32_t colorset_size_ = 0;
  uint32_t task_count_ = 0;

  const char *api_v() const noexcept final { return "Vulkan"; }

  int error_code_v() const noexcept final { return error_code_.value(); }
  bool ok_v() const noexcept final {
    return error_code_.value() == static_cast<int>(vk::Result::eSuccess);
  }
  std::string error_detail_v() const noexcept final {
    return error_code_.message();
  }

  void set_colorset_v(
      size_t color_num,
      const std::array<const float *, 3> &color_ptrs) noexcept final {
    auto write = [&](float *const dest) {
      for (size_t i = 0; i < color_num; i++) {
        dest[3 * i + 0] = color_ptrs[0][i];
        dest[3 * i + 1] = color_ptrs[1][i];
        dest[3 * i + 2] = color_ptrs[2][i];
      }
    };
    try {
      this->adjust_buffer_for_colorset(color_num);

      if (auto buf = std::get_if<required_buffers>(&this->host_buf)) {
        write(static_cast<float *>(buf->colorset.allocation_info.pMappedData));
      } else {
        void *const data =
            this->allocator.mapMemory(this->device_buf.colorset.alloc.get());
        write(static_cast<float *>(data));
        this->allocator.unmapMemory(this->device_buf.colorset.alloc.get());
      }
    } catch (const vk::SystemError &e) {
      this->error_code_ = e.code();
    }
    this->colorset_size_ = color_num;
  }

  void set_task_v(size_t task_num,
                  const std::array<float, 3> *data) noexcept final {
    auto write = [&](float *const dest) {
      memcpy(dest, data, task_num * sizeof(float[3]));
    };
    try {
      adjust_buffer_for_task(task_num);
      if (auto buf = std::get_if<required_buffers>(&this->host_buf)) {
        write(static_cast<float *>(buf->task.allocation_info.pMappedData));
      } else {
        void *const data =
            this->allocator.mapMemory(this->device_buf.task.alloc.get());
        write(static_cast<float *>(data));
        this->allocator.unmapMemory(this->device_buf.task.alloc.get());
      }
    } catch (const vk::SystemError &e) {
      this->error_code_ = e.code();
    }
    this->task_count_ = task_num;
  }
  void execute_v(::SCL_convertAlgo algo, bool wait) noexcept final {
    try {
      this->cmd_buffer->begin(vk::CommandBufferBeginInfo{
          vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
      std::vector<vk::BufferMemoryBarrier2> bmbs;
      // enqueue a buffer copy
      if (auto staging_bufs = std::get_if<required_buffers>(&this->host_buf)) {
        cmd_buffer->copyBuffer(
            staging_bufs->colorset.buffer.get(),
            this->device_buf.colorset.buffer.get(),
            vk::BufferCopy{0, 0, colorset_size_ * sizeof(float[3])});
        cmd_buffer->copyBuffer(
            staging_bufs->task.buffer.get(), this->device_buf.task.buffer.get(),
            vk::BufferCopy{0, 0, task_count_ * sizeof(float[3])});
        for (auto rbuf : {staging_bufs->task.buffer.get(),
                          staging_bufs->colorset.buffer.get()}) {
          bmbs.emplace_back(vk::PipelineStageFlagBits2::eTransfer,
                            vk::AccessFlagBits2::eTransferWrite,
                            vk::PipelineStageFlagBits2::eNone,
                            vk::AccessFlagBits2::eNone, vk::QueueFamilyIgnored,
                            vk::QueueFamilyIgnored, rbuf, 0, vk::WholeSize);
        }
      }
      // enqueue pipeline
      {
        for (auto &bmb : bmbs) {
          bmb.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
          bmb.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
        }
        if (not bmbs.empty()) {
          vk::DependencyInfo deps;
          deps.setBufferMemoryBarriers(bmbs);
          cmd_buffer->pipelineBarrier2(deps);
          bmbs.clear();
        }

        const color_diff_push_const push_const{
            .task_num = task_count_,
            .colorset_size = colorset_size_,
            .algo = static_cast<uint32_t>(algo),
        };
        cmd_buffer->bindPipeline(vk::PipelineBindPoint::eCompute,
                                 pipeline.get());
        cmd_buffer->bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                       pipeline_layout.get(), 0,
                                       descriptor_set.get(), {});
        cmd_buffer->pushConstants<color_diff_push_const>(
            pipeline_layout.get(), vk::ShaderStageFlagBits::eCompute, 0,
            push_const);
        cmd_buffer->dispatch(divup(task_count_, 64) * 64, 1, 1);
        for (auto &wbuf : {device_buf.result_diff.buffer.get(),
                           device_buf.result_index.buffer.get()}) {
          bmbs.emplace_back(vk::PipelineStageFlagBits2::eComputeShader,
                            vk::AccessFlagBits2::eShaderWrite,
                            vk::PipelineStageFlagBits2::eNone,
                            vk::AccessFlagBits2::eNone, vk::QueueFamilyIgnored,
                            vk::QueueFamilyIgnored, wbuf, 0, vk::WholeSize);
        }
      }
      // copy to staging buffer
      if (auto staging_buf = std::get_if<required_buffers>(&this->host_buf)) {
        for (auto &bmb : bmbs) {
          bmb.dstAccessMask = vk::AccessFlagBits2::eTransferRead;
          bmb.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
        }
        if (not bmbs.empty()) {
          vk::DependencyInfo deps;
          deps.setBufferMemoryBarriers(bmbs);
          cmd_buffer->pipelineBarrier2(deps);
          bmbs.clear();
        }

        cmd_buffer->copyBuffer(
            device_buf.result_diff.buffer.get(),
            staging_buf->result_diff.buffer.get(),
            vk::BufferCopy{0, 0, task_count_ * sizeof(float)});
        cmd_buffer->copyBuffer(
            device_buf.result_index.buffer.get(),
            staging_buf->result_index.buffer.get(),
            vk::BufferCopy{0, 0, task_count_ * sizeof(uint16_t)});
      }

      cmd_buffer->end();
      queue.submit(vk::SubmitInfo{{}, {}, cmd_buffer.get()}, fence.get());
      if (wait) {
        this->wait_v();
      }
    } catch (const vk::SystemError &e) {
      this->error_code_ = e.code();
    }
  }

  void wait_v() noexcept final {
    auto result = this->device.waitForFences(fence.get(), true, UINT64_MAX);
    if (result not_eq vk::Result::eSuccess) {
      this->error_code_ =
          std::error_code{static_cast<int>(result), std::system_category()};
      return;
    }
    device.resetFences(fence.get());
    cmd_buffer->reset();
    if (auto results = std::get_if<cpu_results>(&this->host_buf)) {
      results->result_index.resize(task_count_);
      results->result_diff.resize(task_count_);
      memcpy(results->result_diff.data(),
             allocator.mapMemory(device_buf.result_diff.alloc.get()),
             task_count_ * sizeof(float));
      allocator.unmapMemory(device_buf.result_diff.alloc.get());

      memcpy(results->result_index.data(),
             allocator.mapMemory(device_buf.result_index.alloc.get()),
             task_count_ * sizeof(uint16_t));
      allocator.unmapMemory(device_buf.result_index.alloc.get());
    }
  }

  size_t task_count_v() const noexcept final { return this->task_count_; }

  std::string device_vendor_v() const noexcept final {
    auto vendor_name = this->physical_device.getProperties().vendorID;
    return std::to_string(vendor_name);
  }

  const uint16_t *result_idx_v() const noexcept final {
    if (auto host_buf = std::get_if<required_buffers>(&this->host_buf)) {
      return static_cast<const uint16_t *>(
          host_buf->result_index.allocation_info.pMappedData);
    }
    return std::get<cpu_results>(this->host_buf).result_index.data();
  }
  const float *result_diff_v() const noexcept final {
    const float *data = nullptr;
    if (auto host_buf = std::get_if<required_buffers>(&this->host_buf)) {
      data = static_cast<const float *>(
          host_buf->result_diff.allocation_info.pMappedData);
    } else {
      data = std::get<cpu_results>(this->host_buf).result_diff.data();
    }
    assert(data);
    return data;
  }
  size_t local_work_group_size_v() const noexcept final { return 64; }

  void adjust_buffer_for_colorset(size_t color_num);
  void adjust_buffer_for_task(size_t task_num);
  void update_descriptor_set();
};

void gpu_interface::destroy(gpu_interface *gi) noexcept { delete gi; }


gpu_interface *gpu_interface::create(
    platform_wrapper *pw [[maybe_unused]], device_wrapper *dw,
    std::pair<int, std::string> &err) noexcept {
  try {
    auto device = dynamic_cast<device_impl *>(dw);
    auto mem_props = device->allocator->getMemoryProperties();

    auto ret = new gpu_interface_impl{};
    ret->physical_device = device->physical_device;
    ret->device = device->device.get();
    ret->descriptor_pool = device->descriptor_pool.get();
    ret->queue = device->queue;
    ret->allocator = device->allocator.get();
    {
      vk::CommandBufferAllocateInfo ai{device->command_pool.get(),
                                       vk::CommandBufferLevel::ePrimary, 1};
      ret->cmd_buffer =
          std::move(device->device->allocateCommandBuffersUnique(ai)[0]);
    }
    ret->fence = device->device->createFenceUnique(vk::FenceCreateFlags());

    const bool need_staging_buffer =
        not support_mapping_device_memory(ret->allocator);
    // Allocate device buffers
    {
      auto [bci, aci] = get_alloc_template(true, need_staging_buffer);
      ret->device_buf.colorset = allocate(ret->allocator, bci, aci, 4);
      ret->device_buf.task = allocate(ret->allocator, bci, aci, 4);
      ret->device_buf.result_diff = allocate(ret->allocator, bci, aci, 4);
      ret->device_buf.result_index = allocate(ret->allocator, bci, aci, 4);
    }
    if (need_staging_buffer) {
      auto [bci, aci] = get_alloc_template(false, need_staging_buffer);
      required_buffers bufs;
      bufs.colorset = allocate(ret->allocator, bci, aci, 4);
      bufs.task = allocate(ret->allocator, bci, aci, 4);
      bufs.result_diff = allocate(ret->allocator, bci, aci, 4);
      bufs.result_index = allocate(ret->allocator, bci, aci, 4);
      ret->host_buf = std::move(bufs);
    } else {
      cpu_results results;
      ret->host_buf = std::move(results);
    }
    // create pipeline
    {
      vk::UniqueShaderModule shader_module = [&]() {
        assert(compute_shader_spv_len % sizeof(uint32_t) == 0);
        std::span<const uint32_t> spirv{
            reinterpret_cast<const uint32_t *>(compute_shader_spv),
            compute_shader_spv_len / sizeof(uint32_t)};
        return ret->device.createShaderModuleUnique(
            vk::ShaderModuleCreateInfo{{}, spirv});
      }();
      vk::PipelineShaderStageCreateInfo shader_stage_ci{
          {},
          vk::ShaderStageFlagBits::eCompute,
          shader_module.get(),
          "main",
      };
      std::array<vk::DescriptorSetLayoutBinding, 4> layout_bindings;
      for (uint32_t i = 0; i < layout_bindings.size(); ++i) {
        layout_bindings[i].binding = i;
        layout_bindings[i].descriptorType = vk::DescriptorType::eStorageBuffer;
        layout_bindings[i].descriptorCount = 1;
        layout_bindings[i].pImmutableSamplers = nullptr;
        layout_bindings[i].stageFlags = vk::ShaderStageFlagBits::eCompute;
      }
      vk::UniqueDescriptorSetLayout descriptor_set_layout =
          ret->device.createDescriptorSetLayoutUnique(
              vk::DescriptorSetLayoutCreateInfo{{}, layout_bindings});
      vk::PushConstantRange push_constant_range{
          vk::ShaderStageFlagBits::eCompute,
          0,
          sizeof(color_diff_push_const),
      };
      vk::UniquePipelineLayout pipeline_layout =
          ret->device.createPipelineLayoutUnique(vk::PipelineLayoutCreateInfo{
              {}, descriptor_set_layout.get(), push_constant_range});
      vk::ComputePipelineCreateInfo ci{
          {}, shader_stage_ci, pipeline_layout.get()};
      auto pipeline_res = ret->device.createComputePipelineUnique(
          device->pipeline_cache.get(), ci);

      ret->descriptor_set_layout = std::move(descriptor_set_layout);
      ret->pipeline_layout = std::move(pipeline_layout);
      ret->pipeline = std::move(pipeline_res.value);
    }
    ret->update_descriptor_set();

    err.first = static_cast<int>(vk::Result::eSuccess);
    err.second = "";
    return ret;
  } catch (vk::SystemError &e) {
    err.first = e.code().value();
    err.second = e.what();
    return nullptr;
  }
}

void gpu_interface_impl::adjust_buffer_for_colorset(size_t color_num) {
  const uint32_t required_size = color_num * sizeof(float) * 3;
  const uint32_t device_capacity = this->device_buf.colorset.size_in_bytes;
  const bool need_staging_buffer =
      std::get_if<required_buffers>(&this->host_buf);
  if (device_capacity < required_size) {
    auto [bci, aci] = get_alloc_template(true, need_staging_buffer);
    this->device_buf.colorset =
        allocate(this->allocator, bci, aci, required_size);
    this->update_descriptor_set();
  }
  if (need_staging_buffer) {
    required_buffers &bufs = std::get<required_buffers>(this->host_buf);
    const uint32_t host_capacity = bufs.colorset.size_in_bytes;
    if (host_capacity < device_capacity) {
      auto [bci, aci] = get_alloc_template(false, need_staging_buffer);
      bufs.colorset = allocate(this->allocator, bci, aci, device_capacity);
    }
  } else {
    // do nothing
  }
}

void gpu_interface_impl::adjust_buffer_for_task(size_t task_num) {
  const bool need_staging_buffer =
      std::get_if<required_buffers>(&this->host_buf);
  // device
  {
    auto [bci, aci] = get_alloc_template(true, need_staging_buffer);
    bool buffer_updated = false;
    if (this->device_buf.task.size_in_bytes < task_num * sizeof(float[3])) {
      this->device_buf.task =
          allocate(this->allocator, bci, aci, task_num * sizeof(float[3]));
      buffer_updated = true;
    }
    if (this->device_buf.result_diff.size_in_bytes < task_num * sizeof(float)) {
      this->device_buf.result_diff =
          allocate(this->allocator, bci, aci, task_num * sizeof(float));
      buffer_updated = true;
    }
    if (this->device_buf.result_index.size_in_bytes <
        task_num * sizeof(uint16_t)) {
      this->device_buf.result_index =
          allocate(this->allocator, bci, aci, task_num * sizeof(uint16_t));
      buffer_updated = true;
    }
    if (buffer_updated) this->update_descriptor_set();
  }
  // host
  if (need_staging_buffer) {
    auto [bci, aci] = get_alloc_template(false, need_staging_buffer);
    auto &bufs = std::get<required_buffers>(this->host_buf);
    if (bufs.task.size_in_bytes < task_num * sizeof(float[3])) {
      bufs.task =
          allocate(this->allocator, bci, aci, task_num * sizeof(float[3]));
    }
    if (bufs.result_diff.size_in_bytes < task_num * sizeof(float)) {
      bufs.result_diff =
          allocate(this->allocator, bci, aci, task_num * sizeof(float));
    }
    if (bufs.result_index.size_in_bytes < task_num * sizeof(uint16_t)) {
      bufs.result_index =
          allocate(this->allocator, bci, aci, task_num * sizeof(uint16_t));
    }
  } else {
    auto &bufs = std::get<cpu_results>(this->host_buf);
    bufs.result_diff.resize(task_num,
                            std::numeric_limits<float>::signaling_NaN());
    bufs.result_index.resize(task_num, std::numeric_limits<uint16_t>::max());
  }
}

void gpu_interface_impl::update_descriptor_set() {
  this->descriptor_set = std::move(
      device.allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo{
          descriptor_pool, descriptor_set_layout.get()})[0]);
  std::vector<vk::DescriptorBufferInfo> dbi;
  dbi.emplace_back(this->device_buf.colorset.buffer.get(), 0, vk::WholeSize);
  dbi.emplace_back(this->device_buf.task.buffer.get(), 0, vk::WholeSize);
  dbi.emplace_back(this->device_buf.result_diff.buffer.get(), 0, vk::WholeSize);
  dbi.emplace_back(this->device_buf.result_index.buffer.get(), 0,
                   vk::WholeSize);
  std::vector<vk::WriteDescriptorSet> wds;
  for (uint32_t i = 0; i < dbi.size(); i++) {
    wds.emplace_back(vk::WriteDescriptorSet{descriptor_set.get(),
                                            i,
                                            0,
                                            vk::DescriptorType::eStorageBuffer,
                                            {},
                                            dbi[i]});
  }
  device.updateDescriptorSets(wds, {});
}

}  // namespace gpu_wrapper
