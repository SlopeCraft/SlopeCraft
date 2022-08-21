cmake_minimum_required(VERSION 3.5)
project(SC_imageCutter VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(imageCutter_ts_files 
    imageCutter_en_US.ts)

set(imageCutter_project_sources
    CutterWind.h 
    CutterWind.cpp 
    main.cpp
    CutterWind.ui 
    ${imageCutter_en_US.ts}    
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(imageCutter
        MANUAL_FINALIZATION
        ${imageCutter_project_sources}
    )
    qt_create_translation(imageCutter_qm_files ${CMAKE_SOURCE_DIR} ${imageCutter_ts_files})

else()
    if(ANDROID)
        add_library(imageCutter SHARED ${imageCutter_project_sources})
    else()
        add_executable(imageCutter ${imageCutter_project_sources})
    endif()

    qt5_create_translation(imageCutter_qm_files ${CMAKE_SOURCE_DIR} ${imageCutter_ts_files})
endif()

target_link_libraries(imageCutter 
    PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(imageCutter PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com/ToKiNoBug/SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(imageCutter)
endif()