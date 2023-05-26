project(SlopeCraft_imageCutter VERSION ${SlopeCraft_version})
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)

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
    configure_file(others/imageCutter.rc.in others/imageCutter.rc)
    set(imageCutter_windows_rc_files ${CMAKE_CURRENT_BINARY_DIR}/others/imageCutter.rc)
endif()

set(imageCutter_ts_files
    imageCutter_en_US.ts)

set(imageCutter_project_sources
    CutterWind.h
    CutterWind.cpp
    main.cpp
    CutterWind.ui

    # ${imageCutter_ts_files}
    ${imageCutter_windows_rc_files}
)

qt_add_executable(imageCutter
    MANUAL_FINALIZATION
    ${imageCutter_project_sources})

# translation
qt_add_lupdate(imageCutter
    TS_FILES ${imageCutter_ts_files}
    SOURCES ${imageCutter_project_sources}
    OPTIONS ${SC_lupdate_flags}
)

qt_add_lrelease(imageCutter TS_FILES ${imageCutter_ts_files}
    QM_FILES_OUTPUT_VARIABLE imageCutter_qm_files)
qt_add_resources(imageCutter "translations"
    PREFIX "/i18n"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${imageCutter_qm_files}
)

target_link_libraries(imageCutter
    PRIVATE
    Qt6::Widgets)

set_target_properties(imageCutter PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_ICON_FILE imageCutterIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(imageCutter)

include(install.cmake)