cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_GAConverter VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories(${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ../../ColorManip
)

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

#target_compile_options(GAConverter BEFORE PUBLIC "-std=c++17")