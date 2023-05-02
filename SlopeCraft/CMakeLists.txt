cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_NewGUI VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools Network REQUIRED)

set(SlopeCraft_headers
    SCWind.h)
set(SlopeCraft_sources
    SCWind.cpp
    main.cpp)
set(SlopeCraft_uis
    SCWind.ui)

set(SlopeCraft_project_files
    ${SlopeCraft_headers}
    ${SlopeCraft_sources}
    ${SlopeCraft_uis})

qt_add_executable(SlopeCraft
    MANUAL_FINALIZATION
    ${SlopeCraft_project_files})

target_link_libraries(SlopeCraft PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    SlopeCraftL
    AdaptiveLabel
    VersionDialog)

set_target_properties(SlopeCraft PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_ICON_FILE SlopeCraftIconNew.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(SlopeCraft)

if(${WIN32})
    add_custom_target(SC_create_symlink_SC
        COMMAND mklink SlopeCraftL.dll "..\\SlopeCraftL\\SlopeCraftL.dll"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS SlopeCraftL
        COMMENT "Create symlink to SlopeCraftL.dll for SlopeCraft.exe")
    add_dependencies(SC_create_all_symlinks SC_create_symlink_SC)
endif()

include(install.cmake)