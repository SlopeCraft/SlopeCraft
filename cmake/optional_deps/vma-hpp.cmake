find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG)

find_package(VulkanMemoryAllocator-Hpp CONFIG QUIET)
if (VulkanMemoryAllocator-Hpp_FOUND)
    return()
endif ()

if (${SlopeCraft_no_download})
    message(FATAL_ERROR "VulkanMemoryAllocator-Hpp not found. SlopeCraft is forbidden to download. Install it from https://github.com/YaaZ/VulkanMemoryAllocator-Hpp and retry")
endif ()

message(STATUS "Downloading YaaZ/VulkanMemoryAllocator-Hpp......")
include(FetchContent)
FetchContent_Declare(
    VulkanMemoryAllocator-Hpp
    URL https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/releases/download/v3.3.0%2B3/VulkanMemoryAllocator-Hpp-3.3.0.tar.gz
    URL_HASH SHA256=8c2a0573babe1f86f3241d12b4c9df40bb944649b7f4239c1c611797f8af6cd5 # Copy from the asset list on the release page
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(VulkanMemoryAllocator-Hpp)
find_package(VulkanMemoryAllocator-Hpp CONFIG REQUIRED)