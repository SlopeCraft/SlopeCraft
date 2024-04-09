cmake_minimum_required(VERSION 3.20)
project(vccl VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(vccl_win_sources)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    configure_file(others/vccl.rc.in others/vccl.rc)
    set(vccl_win_sources ${CMAKE_CURRENT_BINARY_DIR}/others/vccl.rc)
endif ()

configure_file(vccl-config-to-bin-dir.json.in vccl-config.json)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(fmt REQUIRED)
find_package(magic_enum REQUIRED)

qt_add_executable(vccl

    MANUAL_FINALIZATION
    vccl.cpp
    vccl_internal.h
    vccl_parse_default.cpp
    ${vccl_win_sources}
)

target_link_libraries(vccl PRIVATE VisualCraftL Qt6::Core Qt6::Gui fmt::fmt magic_enum::magic_enum)
target_include_directories(vccl PRIVATE ${cli11_include_dir} ${SlopeCraft_Nlohmann_json_include_dir})

find_package(OpenMP REQUIRED)
target_link_libraries(vccl PRIVATE OpenMP::OpenMP_CXX VCLConfigLoader)

qt_finalize_executable(vccl)

set_target_properties(vccl PROPERTIES
    VERSION ${PROJECT_VERSION}

    # MACOSX_BUNDLE_ICON_FILE imageCutterIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

if (${WIN32})
    add_custom_target(SC_create_symlink_VCCL
        COMMAND mklink VisualCraftL.dll "..\\VisualCraftL\\VisualCraftL.dll"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS VisualCraftL
        COMMENT "Create symlink to VisualCraftL.dll for vccl.exe")
    add_dependencies(SC_create_all_symlinks SC_create_symlink_VCCL)
endif ()

include(install.cmake)

include(add_test_vccl.cmake)