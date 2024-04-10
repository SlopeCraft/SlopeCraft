project(GPUWrapper LANGUAGES C CXX)

add_library(GPUInterface STATIC
    GPU_interface.h)

target_include_directories(GPUInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(GPUInterface PUBLIC cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(GPUInterface PUBLIC fmt::fmt)

if (${LINUX})
    set_target_properties(GPUInterface PROPERTIES TARGET_INDENPENDENT_CODE TRUE)
endif ()

if (${SlopeCraft_GPU_API} STREQUAL "OpenCL")
    add_subdirectory(OpenCL)
    target_compile_definitions(GPUInterface PUBLIC -DSLOPECRAFT_GPU_API="OpenCL")
    return()
elseif (${SlopeCraft_GPU_API} STREQUAL "Vulkan")
    add_subdirectory(Vulkan)
    target_compile_definitions(GPUInterface PUBLIC -DSLOPECRAFT_GPU_API="Vulkan")
    return()
elseif (${SlopeCraft_GPU_API} STREQUAL "None")
    add_subdirectory(None)
    return()
endif ()

message(FATAL_ERROR "Invalid value for \"SlopeCraft_GPU_API\" : ${SlopeCraft_GPU_API}")