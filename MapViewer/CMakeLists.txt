cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_MapViewer VERSION ${SlopeCraft_version})
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(ZLIB 1.2.11 REQUIRED)

find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)

set(MapViewer_header_files

    MapViewerWind.h
    processMapFiles.h
)

set(MapViewer_source_files

    main.cpp
    processMapFiles.cpp
    MapViewerWind.cpp
    resource_manually.cpp
)

set(MapViewer_windows_rc_files)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Configuring on Windows. Adding rc file to " ${PROJECT_NAME})
    configure_file(others/MapViewer.rc.in others/MapViewer.rc)
    set(MapViewer_windows_rc_files ${CMAKE_CURRENT_BINARY_DIR}/others/MapViewer.rc)
endif ()

set(MapViewer_ts_files
    MapViewer_en_US.ts)

set(MapViewer_project_sources
    ${MapViewer_header_files}
    ${MapViewer_source_files}
    MapViewerWind.ui

    # ${MapViewer_ts_files}
    ${MapViewer_windows_rc_files}
)

qt_add_executable(MapViewer
    MANUAL_FINALIZATION
    ${MapViewer_project_sources})

# qt_create_translation(MapViewer_qm_files ${CMAKE_SOURCE_DIR} ${MapViewer_ts_files})
find_package(OpenMP REQUIRED)
find_package(Eigen3 REQUIRED)

target_link_libraries(MapViewer
    PRIVATE
    SlopeCraftL
    ZLIB::ZLIB
    Qt6::Widgets
    OpenMP::OpenMP_CXX
    Eigen3::Eigen)
target_compile_features(MapViewer PRIVATE cxx_std_23)

set_target_properties(MapViewer PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_ICON_FILE MapViewer.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_add_lupdate(MapViewer
    TS_FILES ${MapViewer_ts_files}
    SOURCES ${MapViewer_project_sources}
    OPTIONS ${SC_lupdate_flags}
)

# translation
qt_add_lrelease(MapViewer TS_FILES ${MapViewer_ts_files}
    QM_FILES_OUTPUT_VARIABLE MapViewer_qm_files)
qt_add_resources(MapViewer "translations"
    PREFIX "/i18n"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${MapViewer_qm_files}
)

qt_finalize_executable(MapViewer)

include(install.cmake)