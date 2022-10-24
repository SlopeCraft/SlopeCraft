cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_imageCutter VERSION ${SlopeCraft_version})
set(CMAKE_CXX_STANDARD 17)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(imageCutter_header_files 
    CutterWind.h
    )

set(imageCutter_source_files 
    CutterWind.cpp 
    main.cpp
    )

set(imageCutter_windows_rc_files)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Configuring on Windows. Adding rc file to " ${PROJECT_NAME})
    set(imageCutter_windows_rc_files others/imageCutter.rc)
endif()

set(imageCutter_ts_files 
    imageCutter_en_US.ts)

if(${SlopeCraft_update_ts_files})
    execute_process(
        COMMAND ${SlopeCraft_Qt_lupdate_executable} CutterWind.ui ${imageCutter_header_files} ${imageCutter_source_files} "-ts" ${imageCutter_ts_files}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endif()

execute_process(COMMAND ${SlopeCraft_Qt_lrelease_executable} ${imageCutter_ts_files}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

set(imageCutter_project_sources
    CutterWind.h 
    CutterWind.cpp 
    main.cpp
    CutterWind.ui 
    ${imageCutter_ts_files}
    ${imageCutter_windows_rc_files}
    imageCutter_translate.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(imageCutter
        MANUAL_FINALIZATION
        ${imageCutter_project_sources}
    )
    #qt_create_translation(imageCutter_qm_files ${CMAKE_SOURCE_DIR} ${imageCutter_ts_files})

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
    MACOSX_BUNDLE_ICON_FILE imageCutterIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(imageCutter)
endif()


install(TARGETS imageCutter
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    BUNDLE  DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

    
# Install icon for macOS
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")

file(GLOB imageCutter_Icon ${CMAKE_SOURCE_DIR}/imageCutter/others/imageCutterIconNew.icns)
install(FILES ${imageCutter_Icon}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/imageCutter.app/Contents/Resources)
endif()