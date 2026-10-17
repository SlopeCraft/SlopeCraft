cmake_minimum_required(VERSION 3.20)
project(vccl VERSION ${SlopeCraft_version} LANGUAGES CXX)

configure_file(vccl-config-build.json.in vccl-config-build.json)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(magic_enum REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(vccl
    vccl.cpp
    vccl_internal.h
    vccl_parse_default.cpp
)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    configure_file(others/vccl.rc.in ${CMAKE_CURRENT_BINARY_DIR}/others/vccl.rc)
    target_sources(vccl PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/others/vccl.rc)
endif ()

target_link_libraries(vccl PRIVATE
    VisualCraftL
    Qt6::Core
    Qt6::Gui
    magic_enum::magic_enum
    OpenMP::OpenMP_CXX
    VCLConfigLoader
    MCDataVersion
)
target_compile_definitions(vccl PRIVATE SC_RESOURCE_DIR="${SlopeCraft_resource_dir}")
target_include_directories(vccl PRIVATE
    ${cli11_include_dir}
    ${SlopeCraft_Nlohmann_json_include_dir}
)

if (${WIN32})
    add_custom_target(SC_create_symlink_VCCL
        COMMAND mklink VisualCraftL.dll "..\\VisualCraftL\\VisualCraftL.dll"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS VisualCraftL
        COMMENT "Create symlink to VisualCraftL.dll for vccl.exe")
    add_dependencies(SC_create_all_symlinks SC_create_symlink_VCCL)
endif ()

if (${MINGW})
    # Link this to fix std::print under mingw (gcc>=14)
    target_link_libraries(vccl PRIVATE stdc++exp)
endif ()

include(add_test_vccl.cmake)