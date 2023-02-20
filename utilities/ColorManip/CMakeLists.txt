project(ColorManip VERSION ${SlopeCraft_version} LANGUAGES C CXX)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include_directories(../SlopeCraftL)
find_package(OpenMP REQUIRED)

add_library(ColorManip

    ColorManip.h
    ColorCvt.cpp
    ColorDiff.cpp
    CIEDE00.cpp

    newColorSet.hpp
    newTokiColor.hpp
    StaticMembers.cpp

    colorset_maptical.hpp
    imageConvert.hpp
    newColorSet.hpp
    newTokiColor.hpp
)

target_compile_features(ColorManip PUBLIC cxx_std_20)

# set_target_properties(ColorManip PROPERTIES CXX_STANDARD_REQUIRED ON)

# get_target_property(cm_cxx_std ColorManip CXX_STANDARD)
# message(STATUS "cm_cxx_std = " ${cm_cxx_std})
target_include_directories(ColorManip PUBLIC
    ${SlopeCraft_Eigen3_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities)

target_include_directories(ColorManip PRIVATE
    ${SlopeCraft_HeuristicFlow_include_dir})

target_link_libraries(ColorManip PUBLIC OpenMP::OpenMP_CXX)

# target_compile_options(ColorManip BEFORE PUBLIC "-std=c++17")
target_compile_options(ColorManip PRIVATE ${SlopeCraft_vectorize_flags})

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(ColorManip PUBLIC "-fPIC")
endif()

find_package(OpenCL 3.0)

if(${OpenCL_FOUND})
    message(STATUS "Configuring OpenCL")
    include(${CMAKE_SOURCE_DIR}/cmake/find_ResourceCreator.cmake)
    include(${CMAKE_SOURCE_DIR}/3rdParty/ResourceCreator.cmake/ResourceCreator.cmake)

    add_library(OCLWarpper STATIC
        ColorDiff_OpenCL.h
        ColorDiff_OpenCL.cpp)

    add_resource(ColorManip_cl_rc ColorDiff.cl)

    target_include_directories(OCLWarpper PRIVATE ${CMAKE_SOURCE_DIR})
    target_compile_features(OCLWarpper PUBLIC cxx_std_20)
    target_link_libraries(OCLWarpper PUBLIC OpenCL::OpenCL ColorManip_cl_rc)
endif()