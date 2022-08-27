cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_MapViewer VERSION ${SlopeCraft_version})
set(CMAKE_CXX_STANDARD 17)

include_directories(${SlopeCraft_Eigen3_include_dir})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(ZLIB 1.2.11)

#message("ZLIB_INCLUDE_DIR = " ${ZLIB_INCLUDE_DIR})
if(ZLIB_FOUND)
    include_directories(ZLIB_INCLUDE_DIR)
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
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
    )

set(MapViewer_windows_rc_files)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Configuring on Windows. Adding rc file to " ${PROJECT_NAME})
    set(MapViewer_windows_rc_files others/MapViewer.rc)
endif()

set(MapViewer_ts_files
    MapViewer_en_US.ts)


if(${SlopeCraft_update_ts_files})
    execute_process(
        COMMAND ${SlopeCraft_Qt_lupdate_executable} MapViewerWind.ui ${MapViewer_header_files} ${MapViewer_source_files} "-ts" ${MapViewer_ts_files}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endif()

execute_process(COMMAND ${SlopeCraft_Qt_lrelease_executable} ${MapViewer_ts_files}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )


set(MapViewer_project_sources
    ${MapViewer_header_files}
    ${MapViewer_source_files}
    MapViewerWind.ui
    images.qrc
    ${MapViewer_ts_files}
    ${MapViewer_windows_rc_files}
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MapViewer
        MANUAL_FINALIZATION
        ${MapViewer_project_sources})
    #qt_create_translation(MapViewer_qm_files ${CMAKE_SOURCE_DIR} ${MapViewer_ts_files})

else()
    if(ANDROID)
        add_library(MapViewer SHARED ${MapViewer_project_sources})
    else()
        add_executable(MapViewer ${MapViewer_project_sources})
    endif()
endif()

target_link_libraries(MapViewer 
    PRIVATE
    -lz
    Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(MapViewer PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_ICON_FILE MapViewer.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MapViewer)
endif()

install(TARGETS MapViewer
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    BUNDLE  DESTINATION ${CMAKE_INSTALL_PREFIX}
    )