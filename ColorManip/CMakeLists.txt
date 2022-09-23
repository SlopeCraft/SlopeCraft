project(ColorManip VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(${SlopeCraft_Eigen3_include_dir})

#include_directories(../SlopeCraftL)

add_library(ColorManip 
    
    ColorManip.h 
    ColorCvt.cpp 
    ColorDiff.cpp 
    CIEDE00.cpp 

    newColorSet.hpp
    newTokiColor.hpp
    StaticMembers.cpp)

target_compile_options(ColorManip BEFORE PUBLIC "-std=c++17")