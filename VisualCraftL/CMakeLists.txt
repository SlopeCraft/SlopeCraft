cmake_minimum_required(VERSION 3.5)
project(VisualCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities
    )

add_library(VisualCraftL SHARED

    VisualCraftL_global.h
    VisualCraftL.h 
    VisualCraftL.cpp
    TokiVC.h
    TokiVC.cpp  
    Resource_tree.h 
    Resource_tree.cpp)

find_package(libzip REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR})

target_link_libraries(VisualCraftL PRIVATE libzip::zip z)


target_compile_definitions(VisualCraftL PRIVATE BEFORE "-DVISUALCRAFTL_BUILD")