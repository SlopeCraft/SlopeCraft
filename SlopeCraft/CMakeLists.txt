cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_NewGUI VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(magic_enum REQUIRED)
find_package(fmt REQUIRED)
find_package(tl-expected REQUIRED)

set(SlopeCraft_rc_files)

if (${WIN32})
    configure_file(others/SlopeCraft.rc.in ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.rc)
    set(SlopeCraft_rc_files ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.rc)
endif ()

set(SlopeCraft_headers
    SCWind.h
    cvt_task.h
    PoolModel.h
    AdaptiveListView.h
    PreviewWind.h
    ExportTableModel.h
    AiCvterParameterDialog.h
    CopyableTableView.h
    TransparentStrategyWind.h
    CompressEffectViewer.h
    BlockListDialog.h
)

set(SlopeCraft_sources
    SCWind.cpp
    SCWind_slots.cpp
    cvt_task.cpp
    PoolModel.cpp
    AdaptiveListView.cpp
    PreviewWind.cpp
    ExportTableModel.cpp
    AiCvterParameterDialog.cpp
    CopyableTableView.cpp
    TransparentStrategyWind.cpp
    CompressEffectViewer.cpp
    BlockListDialog.cpp

    main.cpp
    ${SlopeCraft_rc_files})

set(SlopeCraft_uis
    SCWind.ui
    PreviewWind.ui
    AiCvterParameterDialog.ui
    TransparentStrategyWind.ui
    CompressEffectViewer.ui
    BlockListDialog.ui
)

set(SlopeCraft_ts_files
    others/SlopeCraft_en_US.ts
)

set(SlopeCraft_project_files
    ${SlopeCraft_headers}
    ${SlopeCraft_sources}
    ${SlopeCraft_uis}

    # ${SlopeCraft_ts_files}
)

qt_add_executable(SlopeCraft
    MANUAL_FINALIZATION
    ${SlopeCraft_project_files})

target_compile_features(SlopeCraft PRIVATE cxx_std_23)
target_precompile_headers(SlopeCraft PRIVATE ${SlopeCraft_headers})

target_link_libraries(SlopeCraft PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    magic_enum::magic_enum
    fmt::fmt
    tl::expected

    SlopeCraftL
    AdaptiveLabel
    VersionDialog
    BlockListManager
    StatMemory
    MemoryPolicyDialog
)

target_include_directories(SlopeCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(SlopeCraft PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_ICON_FILE SlopeCraft.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

file(GLOB SlopeCraft_qrc_images "${CMAKE_CURRENT_SOURCE_DIR}/others/images/*.png")
message(STATUS "SlopeCraft_qrc_images = ${SlopeCraft_qrc_images}")
qt_add_resources(SlopeCraft "SC_images"
    PREFIX "/images/"
    BASE ${CMAKE_CURRENT_SOURCE_DIR}/others/images
    FILES ${SlopeCraft_qrc_images})

qt_add_lupdate(SlopeCraft
    TS_FILES ${SlopeCraft_ts_files}
    SOURCES ${SlopeCraft_project_files}
    OPTIONS ${SC_lupdate_flags}
)

qt_add_lrelease(SlopeCraft TS_FILES ${SlopeCraft_ts_files}
    QM_FILES_OUTPUT_VARIABLE SC_qm_files)

qt_add_resources(SlopeCraft "SC_translations"
    PREFIX "/i18n"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${SC_qm_files})

qt_finalize_executable(SlopeCraft)

if (${WIN32})
    add_custom_target(SC_create_symlink_SC
        COMMAND mklink SlopeCraftL.dll "..\\SlopeCraftL\\SlopeCraftL.dll"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS SlopeCraftL
        COMMENT "Create symlink to SlopeCraftL.dll for SlopeCraft.exe")
    add_dependencies(SC_create_all_symlinks SC_create_symlink_SC)
endif ()

include(install.cmake)