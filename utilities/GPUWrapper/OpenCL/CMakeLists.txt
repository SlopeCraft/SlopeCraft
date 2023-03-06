
find_package(OpenCL 3.0)

if(NOT ${OpenCL_FOUND})
    return()
endif()

message(STATUS "Configuring OpenCL")
include(${CMAKE_SOURCE_DIR}/cmake/find_ResourceCreator.cmake)
include(${CMAKE_SOURCE_DIR}/3rdParty/ResourceCreator.cmake/ResourceCreator.cmake)

add_library(OCLWrapper STATIC
    OCLWrapper.h
    OCLWrapper.cpp

    GPU_interface.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(OCLWrapper PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif()

add_resource(ColorManip_cl_rc ColorDiff.cl)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/__rsc_ColorManip_cl_rc.c)

mark_as_advanced(SlopeCraft_resource_file_to_touch_and_remove)
list(APPEND SlopeCraft_resource_file_to_touch_and_remove ${CMAKE_CURRENT_BINARY_DIR}/__rsc_ColorManip_cl_rc.c)

target_include_directories(OCLWrapper PRIVATE ${SlopeCraft_Eigen3_include_dir})
target_include_directories(OCLWrapper PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(OCLWrapper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(OCLWrapper PUBLIC cxx_std_20)
target_link_libraries(OCLWrapper PUBLIC OpenCL::OpenCL ColorManip_cl_rc)

target_link_libraries(GPUInterface PUBLIC OCLWrapper)
target_compile_definitions(GPUInterface PUBLIC SLOPECRAFT_HAVE_OPENCL)


if(${LINUX})
    set_target_properties(GPUInterface PROPERTIES TARGET_INDENPENDENT_CODE TRUE)
endif()