cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_MapViewer VERSION ${SlopeCraft_version})
set(CMAKE_CXX_STANDARD 17)

include_directories(${SlopeCraft_Eigen3_include_dir})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(ZLIB 1.2.11 REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(MapViewer_header_files

    MapViewerWind.h
    processMapFiles.h
)

set(MapViewer_source_files

    main.cpp
    ../SlopeCraftL/ColorSource.cpp
    processMapFiles.cpp
    MapViewerWind.cpp
    resource_manually.cpp
)

set(MapViewer_windows_rc_files)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Configuring on Windows. Adding rc file to " ${PROJECT_NAME})
    configure_file(others/MapViewer.rc.in others/MapViewer.rc)
    set(MapViewer_windows_rc_files ${CMAKE_CURRENT_BINARY_DIR}/others/MapViewer.rc)
endif()

set(MapViewer_ts_files
    MapViewer_en_US.ts)

if(${SlopeCraft_update_ts_files})
    execute_process(
        COMMAND ${SlopeCraft_Qt_lupdate_executable} MapViewerWind.ui ${MapViewer_header_files} ${MapViewer_source_files} "-ts" ${MapViewer_ts_files} ${SlopeCraft_ts_flags}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

set(MapViewer_project_sources
    ${MapViewer_header_files}
    ${MapViewer_source_files}
    MapViewerWind.ui
    ${MapViewer_ts_files}
    ${MapViewer_windows_rc_files}
)

qt_add_executable(MapViewer
    MANUAL_FINALIZATION
    ${MapViewer_project_sources})

# qt_create_translation(MapViewer_qm_files ${CMAKE_SOURCE_DIR} ${MapViewer_ts_files})
find_package(OpenMP REQUIRED)

target_link_libraries(MapViewer
    PRIVATE
    ZLIB::ZLIB
    Qt${QT_VERSION_MAJOR}::Widgets
    OpenMP::OpenMP_CXX)

set_target_properties(MapViewer PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_ICON_FILE MapViewer.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
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