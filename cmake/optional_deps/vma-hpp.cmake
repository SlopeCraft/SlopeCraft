find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)

include(FetchContent)
FetchContent_Declare(
    VulkanMemoryAllocator-Hpp
    URL https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/releases/download/v3.3.0%2B2/VulkanMemoryAllocator-Hpp-3.3.0.tar.gz
    URL_HASH SHA256=44767b5990dd36bdc3934b48a7bda2906a5573199a12b61ef2f0de8e72f988f1 # Copy from the asset list on the release page
    OVERRIDE_FIND_PACKAGE
)
message(STATUS "Downloading YaaZ/VulkanMemoryAllocator-Hpp......")
FetchContent_MakeAvailable(VulkanMemoryAllocator-Hpp)
find_package(VulkanMemoryAllocator-Hpp CONFIG REQUIRED)