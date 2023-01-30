cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_GAConverter VERSION ${SlopeCraft_version} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
find_package(OpenMP REQUIRED)

set(SlopeCraft_GAConverter_sources

    ../ExternalConverterStaticInterface.h

    GAConverter.h
    GACvterDefines.hpp
    sortColor.h
    uiPack.h
    GAConverter.cpp
    sortColor.cpp
)

add_library(GAConverter STATIC ${SlopeCraft_GAConverter_sources})

target_include_directories(GAConverter PUBLIC ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ../../ColorManip
)

target_link_libraries(GAConverter PUBLIC OpenMP::OpenMP_CXX)

target_compile_features(GAConverter PRIVATE cxx_std_20)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(GAConverter PUBLIC "-fPIC" "-fvisibility=hidden")
endif()

# target_compile_options(GAConverter BEFORE PUBLIC "-std=c++17")