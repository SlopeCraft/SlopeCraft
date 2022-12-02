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
    ParseResourcePack_png.cpp
    ParseResourcePack_blocks.cpp
    ParseResourcePack_json.cpp
    test_fun.cpp)

find_package(libzip REQUIRED)
find_package(ZLIB 1.2.11 REQUIRED)
find_package(PNG REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR})

if(NOT ${libzip_FOUND})
    message(FATAL_ERROR "libzip not found")
endif()

target_include_directories(VisualCraftL PRIVATE ${SlopeCraft_Nlohmann_json_include_dir})

target_link_libraries(VisualCraftL PRIVATE z png libzip::zip ColorManip)

target_compile_definitions(VisualCraftL PRIVATE BEFORE "-DVISUALCRAFTL_BUILD")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # message("libzip_LIBRARY = " ${libzip_LIBRARY})
    # if (NOT DEFINED libzip_LIBRARY)
    # find_library(sc_libzip_lib NAMES libzip.so PATHS ${CMAKE_PREIFX_PATH})
    # else()
    # set(sc_libzip_lib ${libzip_LIBRARY})
    # endif()

    # message("sc_libzip_lib = " ${sc_libzip_lib})
    target_compile_options(VisualCraftL
        PUBLIC "-fvisibility=hidden"

        # PUBLIC "-rpath-link"
    )
endif()

add_executable(test_VCL test_lib.cpp)

target_link_libraries(test_VCL PRIVATE VisualCraftL)