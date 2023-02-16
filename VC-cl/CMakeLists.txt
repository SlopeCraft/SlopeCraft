cmake_minimum_required(VERSION 3.20)
project(VC-cl VERSION ${SlopeCraft_version} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network Concurrent REQUIRED)

qt_add_executable(VC-cl

    MANUAL_FINALIZATION
    VC-cl.cpp)

target_link_libraries(VC-cl PRIVATE VisualCraftL Qt6::Core Qt6::Gui)

include(${CMAKE_SOURCE_DIR}/cmake/configure_cli11.cmake)

target_include_directories(VC-cl PRIVATE ${cli11_include_dir})

find_package(OpenMP REQUIRED)
target_link_libraries(VC-cl PRIVATE OpenMP::OpenMP_CXX)

qt_finalize_executable(VC-cl)
