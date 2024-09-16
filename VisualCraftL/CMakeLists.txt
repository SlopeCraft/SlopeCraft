cmake_minimum_required(VERSION 3.20)
project(VisualCraftL VERSION ${SlopeCraft_version} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(VCL_enable_internal_test OFF)

include(config_versions.cmake)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(STRINGS others/VisualCraftL.def.in VCL_def_funs)

    set(VCL_counter 0)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/others/VisualCraftL.def "")

    foreach (VCL_fun_name ${VCL_def_funs})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/others/VisualCraftL.def ${VCL_fun_name})

        if (VCL_counter GREATER 0)
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/others/VisualCraftL.def "    @" ${VCL_counter} "\n")
        else ()
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/others/VisualCraftL.def "\n")
        endif ()

        math(EXPR VCL_counter "${VCL_counter} +1")
    endforeach (VCL_fun_name ${VCL_def_funs})

    unset(VCL_def_funs)
    unset(VCL_counter)

    set(VCL_win_sources
        ${CMAKE_CURRENT_BINARY_DIR}/others/VisualCraftL.def
        ${CMAKE_CURRENT_BINARY_DIR}/others/VisualCraftL.rc)

else ()
    set(VCL_win_sources)
endif ()

# find_package(OpenCL)
set(VCL_source_files
    VisualCraftL_global.h
    VisualCraftL.h
    VisualCraftL.cpp

    # textures_need_to_override.h
    # textures_need_to_override.cpp
    TokiVC.h
    TokiVC.cpp
    TokiVC_flagdiagram.cpp
    TokiVC_build.cpp
    TokiVC_export_test.cpp

    Resource_tree.h
    Resource_tree.cpp

    ParseResourcePack.h
    ParseResourcePack_png.cpp
    ParseResourcePack_blocks.cpp
    ParseResourcePack_json.cpp
    ResourcePack.cpp

    BlockStateList.h
    BlockStateList.cpp

    DirectionHandler.hpp

    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)

# message(STATUS ${VCL_win_sources})
add_library(VisualCraftL SHARED
    ${VCL_source_files}
    ${VCL_win_sources})

add_library(VisualCraftL_static STATIC
    ${VCL_source_files})

find_package(ZLIB 1.2.11 REQUIRED)
find_package(libzip 1.7.0 REQUIRED)
find_package(fmt 10.0.0 REQUIRED)

set(VCL_include_dirs
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ${CMAKE_SOURCE_DIR})
target_include_directories(VisualCraftL PRIVATE ${VCL_include_dirs})
target_include_directories(VisualCraftL_static PUBLIC ${VCL_include_dirs})

target_include_directories(VisualCraftL PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/utilities>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)
target_include_directories(VisualCraftL_static PUBLIC
    ${CMAKE_SOURCE_DIR}/utilities
    ${CMAKE_CURRENT_SOURCE_DIR})

set(VCL_link_libs

    ColorManip
    version_set
    libzip::zip
    ZLIB::ZLIB
    PNG::PNG
    ${VCL_libzip_additions}
    Schem
    fmt::fmt
    ProcessBlockId
)

target_link_libraries(VisualCraftL PRIVATE $<BUILD_INTERFACE:${VCL_link_libs}>)
target_link_libraries(VisualCraftL_static PUBLIC ${VCL_link_libs})

# message(STATUS "ret = " ${ret})
target_compile_definitions(VisualCraftL PRIVATE "-DVISUALCRAFTL_BUILD")
target_compile_definitions(VisualCraftL_static PRIVATE "-DVISUALCRAFTL_BUILD")

target_compile_features(VisualCraftL PRIVATE cxx_std_20)
target_compile_features(VisualCraftL_static PUBLIC cxx_std_20)

target_compile_options(VisualCraftL PRIVATE ${SlopeCraft_vectorize_flags})
target_compile_options(VisualCraftL_static PRIVATE ${SlopeCraft_vectorize_flags})

# target_compile_options(VisualCraftL PRIVATE "-std=c++20")
if (NOT ${MSVC})
    target_compile_options(VisualCraftL PUBLIC "-flto")

    # target_compile_options(VisualCraftL PUBLIC "-Wall")
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # if the system is windows, remove the "lib" prefix.
    set_target_properties(VisualCraftL PROPERTIES PREFIX "")
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(VisualCraftL
        PUBLIC "-fvisibility=hidden"
    )
    get_target_property(VCL_libzip_type libzip::zip TYPE)
    get_target_property(VCL_libzip_location libzip::zip LOCATION)

    if (VCL_libzip_type STREQUAL "SHARED_LIBRARY")
        get_filename_component(VCL_libzip_dir ${VCL_libzip_location} DIRECTORY)
        message(STATUS "VCL_libzip_dir = " ${VCL_libzip_dir})

        # target_link_directories(VisualCraftL PUBLIC ${VCL_libzip_dir})
        target_link_libraries(VisualCraftL PUBLIC $<BUILD_INTERFACE:libzip::zip>)
        target_link_libraries(VisualCraftL_static PUBLIC libzip::zip)
    endif ()

    # target_link_options(VisualCraftL PRIVATE -rpath=${VCL_libzip_dir})
endif ()

set_target_properties(VisualCraftL PROPERTIES
    VERSION ${PROJECT_VERSION}

    # MACOSX_BUNDLE_ICON_FILE imageCutterIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

include(install.cmake)

# include(add_test_executables.cmake)