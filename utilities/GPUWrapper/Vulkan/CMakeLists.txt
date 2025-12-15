find_package(Vulkan
    COMPONENTS glslc
    REQUIRED)
find_package(Eigen3 REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/optional_deps/kompute.cmake)
find_package(kompute REQUIRED)

find_package(tl-expected REQUIRED)

target_sources(GPUInterface PRIVATE GPU_interface.cpp)
target_link_libraries(GPUInterface PUBLIC
    Vulkan::Vulkan
    kompute::kompute
    Eigen3::Eigen
    tl::expected
)
target_compile_features(GPUInterface PRIVATE cxx_std_23)

get_target_property(glslc_exe Vulkan::glslc LOCATION)
message(STATUS "glslc found at ${glslc_exe}")

set(shader_filename "${CMAKE_CURRENT_BINARY_DIR}/compute.spv")

message(STATUS "Building compute shader to spirv...")
execute_process(COMMAND ${glslc_exe} -fshader-stage=compute compute.glsl -o ${shader_filename}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}

    COMMAND_ERROR_IS_FATAL ANY)

add_resource_config_time(VkComputeShaderSPIRV_rc ${shader_filename})
target_link_libraries(GPUInterface PRIVATE VkComputeShaderSPIRV_rc)

set(shader_test_filename "${CMAKE_CURRENT_BINARY_DIR}/compute_test.spv")
add_custom_target(GPUInterface_shader_test ALL
    COMMAND ${glslc_exe} -fshader-stage=compute compute.glsl -o ${shader_test_filename}
    SOURCES compute.glsl
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS ${shader_test_filename}
    COMMENT "Building compute shader ${shader_test_filename}")