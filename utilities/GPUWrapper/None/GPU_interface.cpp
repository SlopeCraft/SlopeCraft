#include "../GPU_interface.h"
#include <climits>

const char *gpu_wrapper::api_name() noexcept { return "None"; }

size_t gpu_wrapper::platform_num() noexcept { return 0; }

gpu_wrapper::platform_wrapper *gpu_wrapper::platform_wrapper::create(
    size_t, int *) noexcept {
  return nullptr;
}

void gpu_wrapper::platform_wrapper::destroy(
    gpu_wrapper::platform_wrapper *) noexcept {
  return;
}

gpu_wrapper::device_wrapper *gpu_wrapper::device_wrapper::create(
    platform_wrapper *, size_t, int *) noexcept {
  return nullptr;
}

void gpu_wrapper::device_wrapper::destroy(device_wrapper *) noexcept { return; }

gpu_wrapper::gpu_interface *gpu_wrapper::gpu_interface::create(
    platform_wrapper *, device_wrapper *) noexcept {
  return nullptr;
}

gpu_wrapper::gpu_interface *gpu_wrapper::gpu_interface::create(
    gpu_wrapper::platform_wrapper *pw, gpu_wrapper::device_wrapper *dw,
    std::pair<int, std::string> &err) noexcept {
  err.first = INT_MAX;
  err.second = "VisualCraft is built without any GPU api.";
  return nullptr;
}

void gpu_wrapper::gpu_interface::destroy(gpu_interface *) noexcept { return; }
