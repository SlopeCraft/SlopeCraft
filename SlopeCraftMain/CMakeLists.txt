cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_Main VERSION 3.9.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(${SlopeCraft_Eigen3_include_dir}
    ${CMAKE_SOURCE_DIR}/SlopeCraftL
    ${CMAKE_SOURCE_DIR}/BlockListManager)

    
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network Concurrent REQUIRED)

set(SlopeCraftMain_windows_rc_file)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	message("Configuring on Windows.")
    set(SlopeCraftMain_windows_rc_file others/SlopeCraft.rc)
endif()

set(SlopeCraftMain_ts_files
    SlopeCraft_en_US.ts
    SlopeCraft_zh_CN.ts)

set(SlopeCraftMain_ui_files
    AiCvterParameterDialog.ui
    BatchUi.ui
    mainwindow.ui
    previewwind.ui
    TaskBox.ui
    tpstrategywind.ui
    VersionDialog.ui)

set(SlopeCraftMain_qrc_files
    ${CMAKE_SOURCE_DIR}/SlopeCraftMain/others/BlockTextures.qrc
    ${CMAKE_SOURCE_DIR}/SlopeCraftMain/others/Pics.qrc)

set(SlopeCraftMain_project_sources
    AiCvterParameterDialog.h
    BatchUi.h
    TaskBox.h
    VersionDialog.h
    MainWindow.h
    previewwind.h
    tpstrategywind.h

    AiCvterParameterDialog.cpp
    BatchUi.cpp
    TaskBox.cpp
    VersionDialog.cpp
    main.cpp
    MainWindow.cpp
    previewwind.cpp
    tpstrategywind.cpp
    
    ${SlopeCraftMain_ts_files}
    ${SlopeCraftMain_ui_files}
    ${SlopeCraftMain_qrc_files}
    ${SlopeCraftMain_windows_rc_file}
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SlopeCraft 
        MANUAL_FINALIZATION
        ${SlopeCraftMain_project_sources})
    qt_create_translation(SlopeCraftMain_qm_files ${CMAKE_SOURCE_DIR} ${SlopeCraftMain_ts_files})

else()
    if(ANDROID)
        add_library(SlopeCraft SHARED ${SlopeCraftMain_project_sources})
    else()
        add_executable(SlopeCraft ${SlopeCraftMain_project_sources})
    endif()
endif()

target_link_libraries(SlopeCraft
    PRIVATE
    BlockListManager
    SlopeCraftL
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Network 
    Qt${QT_VERSION_MAJOR}::Concurrent
    )

set_target_properties(SlopeCraft PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com/ToKiNoBug/SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SlopeCraft)
endif()