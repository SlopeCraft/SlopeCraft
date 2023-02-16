cmake_minimum_required(VERSION 3.20)
project(VisualCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(VCL_enable_internal_test OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(VCL_win_sources
        VisualCraftL.def
        ${CMAKE_CURRENT_BINARY_DIR}/VisualCraftL.rc)
else()
    set(VCL_win_sources)
endif()

include(config_versions.cmake)

set(VCL_source_files
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

    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)

# message(STATUS ${VCL_win_sources})
add_library(VisualCraftL SHARED
    ${VCL_source_files}
    ${VCL_win_sources})

add_library(VisualCraftL_static STATIC
    ${VCL_source_files})

find_package(ZLIB 1.2.11 REQUIRED)
find_package(PNG 1.6.37 REQUIRED)
find_package(libzip 1.9.2 REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/configure_fmtlib.cmake)
find_package(fmt 9.1.0 REQUIRED)

set(VCL_include_dirs
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ${CMAKE_SOURCE_DIR})
target_include_directories(VisualCraftL PRIVATE ${VCL_include_dirs})
target_include_directories(VisualCraftL_static PUBLIC ${VCL_include_dirs})

target_include_directories(VisualCraftL PUBLIC
    ${CMAKE_SOURCE_DIR}/utilities)
target_include_directories(VisualCraftL_static PUBLIC
    ${CMAKE_SOURCE_DIR}/utilities)

target_include_directories(VisualCraftL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(VisualCraftL_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(VCL_link_libs
    libzip::zip
    ZLIB::ZLIB
    PNG::PNG
    ${VCL_libzip_additions}
    ColorManip
    Schem
    fmt::fmt)

target_link_libraries(VisualCraftL PRIVATE ${VCL_link_libs})
target_link_libraries(VisualCraftL_static PUBLIC ${VCL_link_libs})

# message(STATUS "ret = " ${ret})
target_compile_definitions(VisualCraftL PRIVATE "-DVISUALCRAFTL_BUILD")
target_compile_definitions(VisualCraftL_static PRIVATE "-DVISUALCRAFTL_BUILD")

target_compile_features(VisualCraftL PRIVATE cxx_std_20)
target_compile_features(VisualCraftL_static PUBLIC cxx_std_20)

target_compile_options(VisualCraftL PRIVATE ${SlopeCraft_vectorize_flags})
target_compile_options(VisualCraftL_static PRIVATE ${SlopeCraft_vectorize_flags})

# target_compile_options(VisualCraftL PRIVATE "-std=c++20")
if(NOT ${MSVC})
    target_compile_options(VisualCraftL PUBLIC "-flto")
endif()

# profile with gprof
# target_compile_options(VisualCraftL PUBLIC "-pg")
# target_link_options(VisualCraftL PUBLIC "-pg")
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # if the system is windows, remove the "lib" prefix.
    set_target_properties(VisualCraftL PROPERTIES PREFIX "")
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

        # PRIVATE "-rpath-link"
    )

    # get_target_property(VCL_libzip_type libzip::zip TYPE)

    # get_target_property(VCL_libzip_location libzip::zip LOCATION)

    # get_filename_component(VCL_libzip_dir ${VCL_libzip_location} DIRECTORY)

    # message(STATUS "VCL_libzip_dir = " ${VCL_libzip_dir})

    # target_link_directories(VisualCraftL PUBLIC ${VCL_libzip_dir})
    # target_link_options(VisualCraftL PRIVATE -rpath=${VCL_libzip_dir})
endif()

include(${CMAKE_SOURCE_DIR}/cmake/configure_cli11.cmake)

# test block state and resource parsing
add_executable(test_VCL_blockstate tests/test_VCL_blockstate.cpp)
target_link_libraries(test_VCL_blockstate PRIVATE VisualCraftL)
target_include_directories(test_VCL_blockstate PRIVATE ${cli11_include_dir})

# convert json from SCL to VCL
add_executable(SCL_json_cvt tests/SCL_json_cvt.cpp)
target_include_directories(SCL_json_cvt PRIVATE
    ${SlopeCraft_Nlohmann_json_include_dir})

# test block class properties
add_executable(test_block_class tests/test_block_class.cpp)
target_link_libraries(test_block_class PRIVATE VisualCraftL)

# sort VCL json
add_executable(VCL_json_sort tests/VCL_json_sort.cpp)
target_link_libraries(VCL_json_sort PRIVATE VisualCraftL)
target_include_directories(VCL_json_sort PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir})

# test computing projection images
add_executable(test_VCL_project_image tests/test_VCL_project_image.cpp)
target_link_libraries(test_VCL_project_image PRIVATE VisualCraftL PNG::PNG)
target_include_directories(test_VCL_project_image PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_Eigen3_include_dir})

# test showing model
add_executable(test_VCL_model tests/test_VCL_model.cpp)
target_link_libraries(test_VCL_model PRIVATE VisualCraftL)
target_include_directories(test_VCL_model PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_Eigen3_include_dir})

# internal test for rotation
add_executable(itest_VCL_rotate tests/itest_VCL_rotate.cpp)
target_link_libraries(itest_VCL_rotate PRIVATE VisualCraftL_static)
target_include_directories(itest_VCL_rotate PRIVATE
    ${cli11_include_dir})

add_test(NAME test_rotate COMMAND itest_VCL_rotate WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_12.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_latest.cmake)

add_test(NAME test_block_class COMMAND test_block_class WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# automatic tests
include(generate_tests.cmake)