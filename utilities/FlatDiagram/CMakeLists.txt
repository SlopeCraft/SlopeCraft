project(ColorManip VERSION ${SlopeCraft_version} LANGUAGES C CXX)

find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)

add_library(FlatDiagram STATIC
    FlatDiagram.h
    FlatDiagram.cpp)

target_include_directories(FlatDiagram INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(FlatDiagram
    PUBLIC
    ColorManip
    PNG::PNG
    Eigen3::Eigen
    fmt::fmt)
target_compile_features(FlatDiagram PUBLIC cxx_std_20)