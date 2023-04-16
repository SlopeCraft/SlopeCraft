cmake_minimum_required(VERSION 3.20)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
add_library(AdaptiveLabel STATIC
    AdaptiveLabel.h
    AdaptiveLabel.cpp)

target_link_libraries(AdaptiveLabel PUBLIC Qt6::Widgets)
target_include_directories(AdaptiveLabel INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})