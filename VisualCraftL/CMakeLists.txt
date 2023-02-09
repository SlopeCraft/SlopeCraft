cmake_minimum_required(VERSION 3.5)
project(VisualCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(VCL_enable_internal_test OFF)

if(${VCL_enable_internal_test})
    set(VCL_internal_test_source_file test_fun.cpp)
else()
    set(VCL_internal_test_source_file)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(VCL_def_file VisualCraftL.def)
endif()

# message(STATUS ${VCL_def_file})
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
    ResourcePack.cpp

    BlockStateList.h
    BlockStateList.cpp

    ${VCL_internal_test_source_file}
    ${VCL_def_file})

find_package(ZLIB 1.2.11 REQUIRED)
find_package(PNG REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/configure_fmtlib.cmake)
find_package(fmt REQUIRED)

#include(${CMAKE_SOURCE_DIR}/cmake/configure_libzip.cmake)
find_package(libzip REQUIRED)

# include_directories(${ZLIB_INCLUDE_DIR})
#if(NOT ${libzip_FOUND})
#    message(FATAL_ERROR "libzip not found")
#endif()

# extensions for libzip
#find_package(LibLZMA REQUIRED)
#set(VCL_libzip_additions LibLZMA::LibLZMA)

#find_package(zstd)
#if(${zstd_FOUND})
#message(STATUS "zstd found.")
#set(VCL_libzip_additions ${VCL_libzip_additions} Zstd::Zstd)
#endif()

target_include_directories(VisualCraftL PRIVATE
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ${CMAKE_SOURCE_DIR})

target_include_directories(VisualCraftL PUBLIC
    ${CMAKE_SOURCE_DIR}/utilities)

target_include_directories(VisualCraftL INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VisualCraftL 
    PRIVATE 
    libzip::zip 
    ZLIB::ZLIB 
    PNG::PNG 
    ${VCL_libzip_additions} 
    ColorManip 
    Schem 
    fmt::fmt)

#message(STATUS "ret = " ${ret})

target_compile_definitions(VisualCraftL PRIVATE BEFORE "-DVISUALCRAFTL_BUILD")

if(${VCL_enable_internal_test})
    target_compile_definitions(VisualCraftL PUBLIC BEFORE "-DVCL_ENABLE_INTERNAL_TEST")
endif()

target_compile_features(VisualCraftL PRIVATE cxx_std_20)

# target_compile_options(VisualCraftL PRIVATE "-std=c++20")
if(NOT ${MSVC})
    #target_compile_options(VisualCraftL PUBLIC "-flto")
endif()

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

        #PRIVATE "-rpath-link"
        )

    get_target_property(VCL_libzip_type libzip::zip TYPE)

    get_target_property(VCL_libzip_location libzip::zip LOCATION)

    get_filename_component(VCL_libzip_dir ${VCL_libzip_location} DIRECTORY)

    #message(STATUS "VCL_libzip_dir = " ${VCL_libzip_dir})

    #target_link_directories(VisualCraftL PUBLIC ${VCL_libzip_dir})
    #target_link_options(VisualCraftL PRIVATE -rpath=${VCL_libzip_dir})
endif()

if(${VCL_enable_internal_test})
    add_executable(test_VCL_internal tests/test_VCL_internal.cpp)
    target_link_libraries(test_VCL_internal PRIVATE VisualCraftL)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/configure_cli11.cmake)

add_executable(test_VCL_blockstate tests/test_VCL_blockstate.cpp)
target_link_libraries(test_VCL_blockstate PRIVATE VisualCraftL)
target_include_directories(test_VCL_blockstate PRIVATE ${cli11_include_dir})

add_executable(SCL_json_cvt tests/SCL_json_cvt.cpp)
target_include_directories(SCL_json_cvt PRIVATE
    ${SlopeCraft_Nlohmann_json_include_dir})

add_executable(test_block_class tests/test_block_class.cpp)
target_link_libraries(test_block_class PRIVATE VisualCraftL)

add_executable(VCL_json_sort tests/VCL_json_sort.cpp)
target_link_libraries(VCL_json_sort PRIVATE VisualCraftL)
target_include_directories(VCL_json_sort PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir})

add_executable(test_VCL_project_image tests/test_VCL_project_image.cpp)
target_link_libraries(test_VCL_project_image PRIVATE VisualCraftL PNG::PNG)
target_include_directories(test_VCL_project_image PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_Eigen3_include_dir})

add_executable(test_VCL_model tests/test_VCL_model.cpp)
target_link_libraries(test_VCL_model PRIVATE VisualCraftL)
target_include_directories(test_VCL_model PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_Eigen3_include_dir})

include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_12.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_latest.cmake)

add_test(NAME test_block_class COMMAND test_block_class WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# automatic tests
include(generate_tests.cmake)