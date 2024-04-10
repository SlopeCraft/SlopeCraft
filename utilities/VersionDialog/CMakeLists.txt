project(SlopeCraft_VersionDialog VERSION ${SlopeCraft_version} LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools Core Gui Network REQUIRED)

set(VersionDialog_header_files
    VersionDialog.h)
set(VersionDialog_source_files
    VersionDialog.cpp)
set(VersionDialog_ui_files
    VersionDialog.ui)
set(VersionDialog_ts_files
    others/VersionDialog_en_US.ts
)

set(VersionDialog_project_sources
    ${VersionDialog_header_files}
    ${VersionDialog_source_files}
    ${VersionDialog_ui_files})

add_library(VersionDialog STATIC
    ${VersionDialog_project_sources})

target_include_directories(VersionDialog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VersionDialog PUBLIC Qt::Widgets Qt::Core Qt::Network)
target_compile_features(VersionDialog PUBLIC cxx_std_20)
target_include_directories(VersionDialog PRIVATE ${SlopeCraft_Nlohmann_json_include_dir})

# translation
qt_add_lupdate(VersionDialog
    TS_FILES ${BlockListManager_ts_files}
    SOURCES ${VersionDialog_project_sources}
    OPTIONS ${SC_lupdate_flags}
)
qt_add_lrelease(VersionDialog TS_FILES ${VersionDialog_ts_files}
    QM_FILES_OUTPUT_VARIABLE VD_qm_files)
qt_add_resources(VersionDialog "VersionDialog_translations"
    PREFIX "/i18n"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${VD_qm_files}
)