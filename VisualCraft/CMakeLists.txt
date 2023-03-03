cmake_minimum_required(VERSION 3.5)
project(VisualCraft_Main VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools Network Concurrent REQUIRED)

find_package(magic_enum REQUIRED)

find_package(OpenMP REQUIRED)

set(VisualCraft_header_files
    VCWind.h
    advanced_qlist_widget_item.h
    CallBackFunctions.h
    VC_block_class.h
    BlockBrowser.h
)

set(VisualCraft_source_files

    main.cpp

    # sources for VCWind
    VCWind.cpp
    VCWind_gpu.cpp
    VCWind_export.cpp
    VCWind_subwindows.cpp

    advanced_qlist_widget_item.cpp
    CallBackFunctions.cpp
    VC_block_class.cpp

    BlockBrowser.cpp
)

set(VisualCraft_ui_files
    VCWind.ui
    VC_block_class.ui
    BlockBrowser.ui)

set(VisualCraft_ts_files)

set(VisualCraft_project_sources
    ${VisualCraft_header_files}
    ${VisualCraft_source_files}
    ${VisualCraft_ui_files}
    ${VisualCraft_ts_files}
)

qt_add_executable(VisualCraft
    MANUAL_FINALIZATION
    ${VisualCraft_project_sources})

target_compile_features(VisualCraft PRIVATE cxx_std_20)

target_link_libraries(VisualCraft PRIVATE
    VisualCraftL
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent
    magic_enum::magic_enum
    OpenMP::OpenMP_CXX)

set_target_properties(VisualCraft PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_ICON_FILE SlopeCraftIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(VisualCraft)

include(install.cmake)