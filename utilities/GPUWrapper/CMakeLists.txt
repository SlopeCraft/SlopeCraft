project(GPUWrapper LANGUAGES C CXX)

add_library(GPUInterface STATIC
    GPU_interface.h
    GPU_interface.cpp)

target_link_directories(GPUInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(GPUInterface PUBLIC cxx_std_20)

include(add_opencl_backend.cmake)