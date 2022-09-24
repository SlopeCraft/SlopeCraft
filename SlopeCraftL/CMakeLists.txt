cmake_minimum_required(VERSION 3.5)
project(SlopeCraft_SlopeCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities
)


find_package(ZLIB 1.2.11)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ColorManip)

#message("ZLIB_INCLUDE_DIR = " ${ZLIB_INCLUDE_DIR})
if(ZLIB_FOUND)
    include_directories(ZLIB_INCLUDE_DIR)
endif()

set(SlopeCraft_SCL_windows_rc_files)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	message("Configuring on Windows. Adding rc file to " ${PROJECT_NAME})
    set(SlopeCraft_SCL_windows_rc_files 
        ${CMAKE_CURRENT_SOURCE_DIR}/others/SlopeCraftL.rc)
endif()


set(SlopeCraft_SCL_sources 
    ${CMAKE_CURRENT_SOURCE_DIR}/AiCvterOpt.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/Colors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HeightLine.h 
    #${CMAKE_CURRENT_SOURCE_DIR}/newNBTWriter.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/OptiChain.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/PrimGlassBuilder.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/SCLDefines.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/WaterItem.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/lossyCompressor.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/object_pool.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/simpleBlock.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL_global.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/defines_of_extern_classes.h
    #${CMAKE_CURRENT_SOURCE_DIR}/newColorSet.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/AiCvterOpt.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/Colors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ColorSource.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/HeightLine.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/OptiChain.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/PrimGlassBuilder.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL_CAPI.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft_build.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft_convert.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/WaterItem.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/imagePreprocess.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/lossyCompressor.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/simpleBlock.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL.cpp
    
    ${SlopeCraft_SCL_windows_rc_files}
    )

add_definitions(-DSLOPECRAFTL_LIBRARY)

add_library(SlopeCraftL SHARED ${SlopeCraft_SCL_sources})
# link SlopeCraftL with GAConverter and zlib
target_link_libraries(SlopeCraftL PUBLIC GAConverter -lz ColorManip NBTWriter)

set_target_properties(SlopeCraftL PROPERTIES
    PREFIX ""
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

install(TARGETS SlopeCraftL 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} 
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})


install(TARGETS SlopeCraftL 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/lib)

set(SlopeCraft_SCL_Cpp_include_files 
    
    SlopeCraftL.h 
    SlopeCraftL_global.h 
    defines_of_extern_classes.h
    )

install(FILES ${SlopeCraft_SCL_Cpp_include_files}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/include)


add_subdirectory(SCL_C)