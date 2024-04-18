cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_GAConverter VERSION ${SlopeCraft_version} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
find_package(OpenMP REQUIRED)
find_package(Heu REQUIRED)

set(SlopeCraft_GAConverter_sources

    ../ExternalConverterStaticInterface.h

    GAConverter.h
    GACvterDefines.hpp
    sortColor.h
    uiPack.h
    GAConverter.cpp
    sortColor.cpp
)

add_library(GAConverter STATIC ${SlopeCraft_GAConverter_sources})
target_include_directories(GAConverter PRIVATE "${CMAKE_BINARY_DIR}/SlopeCraftL")
target_link_libraries(GAConverter PUBLIC ColorManip)
#target_include_directories(GAConverter PUBLIC
#    ../../ColorManip
#)

find_package(Eigen3 REQUIRED)
target_link_libraries(GAConverter PUBLIC OpenMP::OpenMP_CXX Eigen3::Eigen Heu::Genetic)

target_compile_features(GAConverter PRIVATE cxx_std_23)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(GAConverter PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
endif ()

# target_compile_options(GAConverter BEFORE PUBLIC "-std=c++17")