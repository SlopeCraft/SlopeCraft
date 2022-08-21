cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_SlopeCraftL VERSION 3.9.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

include_directories(${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ../GAConverter
)

set(SlopeCraft_SCL_sources 
    AiCvterOpt.h 
    ColorSet.h 
    HeightLine.h 
    newNBTWriter.hpp 
    OptiChain.h 
    PrimGlassBuilder.h 
    SCLDefines.h 
    TokiColor.h 
    TokiSlopeCraft.h 
    WaterItem.h 
    lossyCompressor.h 
    object_pool.hpp 
    simpleBlock.h 
    SlopeCraftL.h 
    SlopeCraftL_global.h 
    defines_of_extern_classes.h

    AiCvterOpt.cpp 
    CIEDE00.cpp 
    ColorSource.cpp 
    HeightLine.cpp 
    OptiChain.cpp 
    PrimGlassBuilder.cpp 
    SlopeCraftL_CAPI.cpp 
    TokiSlopeCraft.cpp 
    TokiSlopeCraft_build.cpp 
    TokiSlopeCraft_convert.cpp 
    WaterItem.cpp 
    colorset.cpp 
    imagePreprocess.cpp 
    lossyCompressor.cpp 
    simpleBlock.cpp 
    tokicolor.cpp 
    SlopeCraftL.cpp)

add_definitions(-DSLOPECRAFTL_LIBRARY)

add_library(SlopeCraftL SHARED ${SlopeCraft_SCL_sources})
# link SlopeCraftL with GAConverter and zlib
target_link_libraries(SlopeCraftL PUBLIC GAConverter -lz)

set_target_properties(SlopeCraftL PROPERTIES
    PREFIX ""
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com/ToKiNoBug/SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)