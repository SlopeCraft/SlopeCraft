project(GPUWrapper LANGUAGES C CXX)

add_library(GPUInterface STATIC
    GPU_interface.h)

target_include_directories(GPUInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(GPUInterface PUBLIC cxx_std_20)

if(${LINUX})
    set_target_properties(GPUInterface PROPERTIES TARGET_INDENPENDENT_CODE TRUE)
endif()

add_subdirectory(OpenCL)