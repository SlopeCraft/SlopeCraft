cmake_minimum_required(VERSION 3.5)
project(VisualCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities
    )

add_library(VisualCraftL SHARED

    VisualCraftL_global.h
    VisualCraftL.h 
    VisualCraftL.cpp
    TokiVC.h
    TokiVC.cpp  
    Resource_tree.h 
    Resource_tree.cpp
    ParseResourcePack.h 
    ParseResourcePack.cpp
    test_fun.cpp)

find_package(libzip REQUIRED)
find_package(ZLIB REQUIRED)
find_package(png REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR})

target_link_libraries(VisualCraftL PRIVATE libzip::zip z png ColorManip)


target_compile_definitions(VisualCraftL PRIVATE BEFORE "-DVISUALCRAFTL_BUILD")

add_executable(test_VCL test_lib.cpp)

target_link_libraries(test_VCL PRIVATE VisualCraftL)