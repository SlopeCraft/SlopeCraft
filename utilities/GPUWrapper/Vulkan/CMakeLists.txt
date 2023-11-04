find_package(Vulkan
    COMPONENTS glslc
    REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/configure_kompute.cmake)
find_package(kompute REQUIRED)

target_sources(GPUInterface PRIVATE GPU_interface.cpp)
target_link_libraries(GPUInterface PUBLIC
    Vulkan::Vulkan
    kompute::kompute)
target_compile_features(GPUInterface PRIVATE cxx_std_20)

get_target_property(glslc_exe Vulkan::glslc LOCATION)
message(STATUS "glslc found at ${glslc_exe}")

set(shader_filename "${CMAKE_CURRENT_BINARY_DIR}/compute.spv")
add_custom_target(GPUInterface_shader ALL
    COMMAND ${glslc_exe} -fshader-stage=compute compute.glsl -o ${shader_filename}
    SOURCES compute.glsl
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS ${shader_filename}
    COMMENT "Building compute shader ${shader_filename}")
add_dependencies(GPUInterface GPUInterface_shader)