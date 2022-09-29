project(ColorManip VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${SlopeCraft_Eigen3_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities
    ${SlopeCraft_HeuristicFlow_include_dir}
    )

#include_directories(../SlopeCraftL)

find_package(OpenMP REQUIRED)

add_library(ColorManip 
    
    ColorManip.h 
    ColorCvt.cpp 
    ColorDiff.cpp 
    CIEDE00.cpp 

    newColorSet.hpp
    newTokiColor.hpp
    StaticMembers.cpp)

#target_compile_options(ColorManip BEFORE PUBLIC "-std=c++17")
target_compile_options(ColorManip PRIVATE ${SlopeCraft_vectorize_flags})