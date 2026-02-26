
find_package(Eigen3 REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/optional_deps/vulkan.cmake)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/optional_deps/vma-hpp.cmake)
find_package(VulkanMemoryAllocator-Hpp CONFIG REQUIRED)

target_sources(GPUInterface PRIVATE
    GPU_interface.cpp
    shader_spv.h
    shader_spv.c
)
target_compile_features(GPUInterface PRIVATE cxx_std_23 c_std_23)
target_link_libraries(GPUInterface PUBLIC
    Vulkan::Vulkan
    Eigen3::Eigen
    GPUOpen::VulkanMemoryAllocator
    VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp
)

set(shader_file "${CMAKE_CURRENT_BINARY_DIR}/color_diff.spv")
add_custom_target(ColorDiff_spirv
    COMMAND ${glslangValidator_exe} -V -o ${shader_file} ${CMAKE_CURRENT_SOURCE_DIR}/color_diff.comp.glsl --target-env vulkan1.1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    BYPRODUCTS ${shader_file}
    COMMENT "Color diff shader"
    SOURCES color_diff.comp.glsl
)
add_dependencies(GPUInterface ColorDiff_spirv)
target_compile_definitions(GPUInterface PRIVATE SHADER_FILE="${shader_file}")

