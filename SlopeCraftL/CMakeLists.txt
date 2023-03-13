project(SlopeCraft_SlopeCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${SlopeCraft_Eigen3_include_dir}
    ${SlopeCraft_HeuristicFlow_include_dir}
    ${CMAKE_SOURCE_DIR}/utilities
)

find_package(ZLIB 1.2.11 REQUIRED)
find_package(OpenMP REQUIRED)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ColorManip)

# message("ZLIB_INCLUDE_DIR = " ${ZLIB_INCLUDE_DIR})
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

set(SlopeCraft_SCL_windows_rc_files)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Configuring on Windows. Adding rc file to " ${PROJECT_NAME})

    configure_file(others/SlopeCraftL.rc.in others/SlopeCraftL.rc)

    set(SlopeCraft_SCL_windows_rc_files
        ${CMAKE_CURRENT_BINARY_DIR}/others/SlopeCraftL.rc)
endif()

set(SlopeCraft_SCL_sources
    ${CMAKE_SOURCE_DIR}/utilities/SC_GlobalEnums.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AiCvterOpt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Colors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HeightLine.h

    # ${CMAKE_CURRENT_SOURCE_DIR}/newNBTWriter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/OptiChain.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PrimGlassBuilder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SCLDefines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WaterItem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lossyCompressor.h

    # ${CMAKE_CURRENT_SOURCE_DIR}/object_pool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/simpleBlock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL_global.h

    # ${CMAKE_CURRENT_SOURCE_DIR}/newColorSet.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AiCvterOpt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Colors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ColorSource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HeightLine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/OptiChain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PrimGlassBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft_static_funs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft_build.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TokiSlopeCraft_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaterItem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imagePreprocess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lossyCompressor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/simpleBlock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL.cpp

    ${SlopeCraft_SCL_windows_rc_files}
)

# add this definations to both targets
add_definitions(-DSLOPECRAFTL_LIBRARY)

add_library(SlopeCraftL SHARED ${SlopeCraft_SCL_sources})

# add_library(SlopeCraftL_C SHARED ${SlopeCraft_SCL_sources})
target_compile_features(SlopeCraftL PRIVATE cxx_std_20)

# target_compile_features(SlopeCraftL_C PRIVATE cxx_std_20)

# target_compile_definitions(SlopeCraftL_C PRIVATE -DSCL_CAPI)

# link SlopeCraftL with GAConverter and zlib
set(SlopeCraft_SCL_link_libs

    ZLIB::ZLIB
    OpenMP::OpenMP_CXX
    GAConverter
    ColorManip
    NBTWriter
    Schem
    MapImageCvter)

# link to dependents
target_link_libraries(SlopeCraftL PUBLIC ${SlopeCraft_SCL_link_libs})

# target_link_libraries(SlopeCraftL_C PUBLIC ${SlopeCraft_SCL_link_libs})
target_compile_options(SlopeCraftL PRIVATE ${SlopeCraft_vectorize_flags})

# target_compile_options(SlopeCraftL_C PRIVATE ${SlopeCraft_vectorize_flags})

# target_compile_definitions(SlopeCraftL PUBLIC "-DSLOPECRAFTL_NOT_INSTALLED")
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(SlopeCraftL PRIVATE "-fvisibility=hidden" "-fPIC")

    # target_compile_options(SlopeCraftL_C PRIVATE "-fvisibility=hidden" "-fPIC")
endif()

# set shared-lib properties
set_target_properties(SlopeCraftL PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

# set_target_properties(SlopeCraftL_C PROPERTIES
# VERSION ${PROJECT_VERSION}
# MACOSX_BUNDLE_GUI_IDENTIFIER "github.com.ToKiNoBug.SlopeCraft"
# MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
# MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
# MACOSX_BUNDLE TRUE
# )
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(SlopeCraftL PROPERTIES
        PREFIX "")

    # set_target_properties(SlopeCraftL_C PROPERTIES
    # PREFIX "")
endif()

include(install.cmake)