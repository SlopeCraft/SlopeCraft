cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_MapViewer VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${SlopeCraft_Eigen3_include_dir})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(MapViewer_ts_files
    MapViewer_en_US.ts)

set(MapViewer_project_sources
    main.cpp 
    ../SlopeCraftL/ColorSource.cpp
    processMapFiles.h 
    processMapFiles.cpp
    MapViewerWind.h 
    MapViewerWind.cpp 
    MapViewerWind.ui 
    images.qrc
    ${MapViewer_ts_files}
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MapViewer
        MANUAL_FINALIZATION
        ${MapViewer_project_sources})
    qt_create_translation(MapViewer_qm_files ${CMAKE_SOURCE_DIR} ${MapViewer_ts_files})

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
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com/ToKiNoBug/SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MapViewer)
endif()

install(TARGETS MapViewer
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})