cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_GAConverter VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ../SlopeCraftL
)

set(SlopeCraft_GAConverter_sources 
    GAConverter.h
    GACvterDefines.hpp
    sortColor.h
    uiPack.h 
    GAConverter.cpp
    sortColor.cpp
    )

add_library(GAConverter STATIC ${SlopeCraft_GAConverter_sources})