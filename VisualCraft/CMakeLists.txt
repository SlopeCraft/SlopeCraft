cmake_minimum_required(VERSION 3.5)
project(VisualCraft_Main VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools Network REQUIRED)

find_package(magic_enum REQUIRED)

find_package(OpenMP REQUIRED)

if (${WIN32})
    configure_file(others/VisualCraft.rc.in others/VisualCraft.rc)
    set(VisualCraft_win_files ${CMAKE_CURRENT_BINARY_DIR}/others/VisualCraft.rc)
else ()
    set(VisualCraft_win_files)
endif ()

set(VisualCraft_header_files
    VCWind.h

    # advanced_qlist_widget_item.h
    CallbackFunctions.h
    VC_block_class.h
    BlockBrowser.h
    BlockSelector.h
    BiomeBrowser.h
    ColorBrowser.h
)

set(VisualCraft_source_files

    main.cpp

    # sources for VCWind
    VCWind.cpp
    VCWind_select.cpp
    VCWind_gpu.cpp
    VCWind_export.cpp
    VCWind_subwindows.cpp

    # advanced_qlist_widget_item.cpp
    CallbackFunctions.cpp
    VC_block_class.cpp

    BlockBrowser.cpp

    BlockSelector.cpp
    BlockSelectorCriteria.cpp

    BiomeBrowser.cpp

    ColorBrowser.cpp
)

set(VisualCraft_ui_files
    VCWind.ui
    VC_block_class.ui
    BlockBrowser.ui
    BlockSelector.ui
    BlockSelectorCriteria.ui
    BiomeBrowser.ui
    ColorBrowser.ui)

set(VisualCraft_ts_files
    others/VisualCraft_en_US.ts)

set(VisualCraft_project_sources
    ${VisualCraft_header_files}
    ${VisualCraft_source_files}
    ${VisualCraft_ui_files}

    # ${VisualCraft_ts_files}
    ${VisualCraft_win_files}
)

qt_add_executable(VisualCraft
    MANUAL_FINALIZATION
    ${VisualCraft_project_sources})

target_compile_features(VisualCraft PRIVATE cxx_std_20)

target_link_libraries(VisualCraft PRIVATE
    VisualCraftL
    Qt6::Widgets
    Qt6::Network
    magic_enum::magic_enum
    OpenMP::OpenMP_CXX
    VersionDialog
    VCLConfigLoader)

target_include_directories(VisualCraft PRIVATE ${SlopeCraft_Nlohmann_json_include_dir})

set_target_properties(VisualCraft PROPERTIES
    VERSION ${PROJECT_VERSION}

    MACOSX_BUNDLE_ICON_FILE VisualCraft.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# translation
qt_add_lupdate(VisualCraft
    TS_FILES ${VisualCraft_ts_files}
    SOURCES ${VisualCraft_project_sources}
    OPTIONS ${SC_lupdate_flags}
)
qt_add_lrelease(VisualCraft TS_FILES ${VisualCraft_ts_files}
    QM_FILES_OUTPUT_VARIABLE VC_qm_files)
qt_add_resources(VisualCraft "VC_translations"
    PREFIX "/i18n"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${VC_qm_files}
)

qt_finalize_executable(VisualCraft)

configure_file(vc-config-to-bin-dir.json.in vc-config.json)

if (${WIN32})
    add_custom_target(SC_create_symlink_VC
        COMMAND mklink VisualCraftL.dll "..\\VisualCraftL\\VisualCraftL.dll"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS VisualCraftL
        COMMENT "Create symlink to VisualCraftL.dll for VisualCraft.exe")
    add_dependencies(SC_create_all_symlinks SC_create_symlink_VC)
endif ()

include(install.cmake)