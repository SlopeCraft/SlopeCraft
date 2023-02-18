cmake_minimum_required(VERSION 3.20)
project(vccl VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(vccl_win_sources)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    configure_file(others/vccl.rc.in others/vccl.rc)
    set(vccl_win_sources ${CMAKE_CURRENT_BINARY_DIR}/others/vccl.rc)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network Concurrent REQUIRED)

qt_add_executable(vccl

    MANUAL_FINALIZATION
    vccl.cpp
    ${vccl_win_sources}
)

target_link_libraries(vccl PRIVATE VisualCraftL Qt6::Core Qt6::Gui)

include(${CMAKE_SOURCE_DIR}/cmake/configure_cli11.cmake)

target_include_directories(vccl PRIVATE ${cli11_include_dir})

find_package(OpenMP REQUIRED)
target_link_libraries(vccl PRIVATE OpenMP::OpenMP_CXX)

qt_finalize_executable(vccl)

set_target_properties(vccl PROPERTIES
    VERSION ${PROJECT_VERSION}

    # MACOSX_BUNDLE_ICON_FILE imageCutterIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE

    # WIN32_EXECUTABLE TRUE
)

include(install.cmake)

include(add_test_vccl.cmake)