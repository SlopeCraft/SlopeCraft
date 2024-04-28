cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_BlockListManager VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Boost REQUIRED)

set(BlockListManager_header_files
    BlockListManager.h
    BaseColor.h
    Block.h)

set(BlockListManager_source_files
    BlockListManager.cpp
    BaseColor.cpp
    Block.cpp
    BLM_preset.cpp)

set(BlockListManager_ui_files
    BaseColorWidget.ui
)

set(BlockListManager_ts_files
    others/BlockListManager_en_US.ts

    # ${CMAKE_CURRENT_SOURCE_DIR}/BlockListManager_en_US.ts
)

set(BlockListManager_project_sources
    ${BlockListManager_header_files}
    ${BlockListManager_source_files}
    ${BlockListManager_ui_files}

    # ${BlockListManager_ts_files}
)

# include_directories(../SlopeCraftL)
add_library(BlockListManager STATIC ${BlockListManager_project_sources})
target_link_libraries(BlockListManager PUBLIC Qt6::Widgets SlopeCraftL)
target_include_directories(BlockListManager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(BlockListManager PRIVATE ${SlopeCraft_Nlohmann_json_include_dir} ${Boost_INCLUDE_DIRS})
target_compile_features(BlockListManager PUBLIC cxx_std_23)

qt_add_lupdate(BlockListManager
    TS_FILES ${BlockListManager_ts_files}
    SOURCES ${BlockListManager_project_sources}
    OPTIONS ${SC_lupdate_flags}
)
qt_add_lrelease(BlockListManager TS_FILES ${BlockListManager_ts_files}
    QM_FILES_OUTPUT_VARIABLE BlockListManager_qm_files)
qt_add_resources(BlockListManager "BLM_translations"
    PREFIX "/i18n"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${BlockListManager_qm_files})

# add_library(cmake_qt_lib STATIC  Cmake_qt_lib.cpp  Cmake_qt_lib.h)

# target_link_libraries(cmake_qt_lib PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# target_compile_definitions(cmake_qt_lib PRIVATE CMAKE_QT_LIB_LIBRARY)
