find_package(Vulkan
    COMPONENTS glslangValidator
    REQUIRED)
get_target_property(glslangValidator_exe Vulkan::glslangValidator LOCATION)
message(STATUS "glslangValidator found at ${glslangValidator_exe}")
find_package(Eigen3 REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/optional_deps/vma-hpp.cmake)
find_package(VulkanMemoryAllocator-Hpp CONFIG REQUIRED)

target_sources(GPUInterface PRIVATE GPU_interface.cpp)
target_compile_features(GPUInterface PRIVATE cxx_std_23)
target_link_libraries(GPUInterface PUBLIC
    Vulkan::Vulkan
    Eigen3::Eigen
    GPUOpen::VulkanMemoryAllocator
    VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp
)

set(shader_file "${CMAKE_CURRENT_BINARY_DIR}/color_diff.spv")
add_custom_target(ColorDiff_spirv
    COMMAND ${glslangValidator_exe} color_diff.comp.glsl -o ${shader_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    BYPRODUCTS ${shader_file}
    COMMENT "Color diff shader"
    SOURCES color_diff.comp.glsl
)

