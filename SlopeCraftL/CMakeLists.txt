project(SlopeCraft_SlopeCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/find_nlohmann_json.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_fmtlib.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_boost.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_zstd.cmake)

find_package(ZLIB 1.2.11 REQUIRED)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Heu REQUIRED)
find_package(Boost COMPONENTS iostreams REQUIRED)
#find_package(zstd REQUIRED)

set(SlopeCraft_SCL_windows_rc_files)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring on Windows. Adding rc file to " ${PROJECT_NAME})
    configure_file(others/SlopeCraftL.rc.in others/SlopeCraftL.rc)
    set(SlopeCraft_SCL_windows_rc_files
        ${CMAKE_CURRENT_BINARY_DIR}/others/SlopeCraftL.rc)
endif ()

set(SlopeCraft_SCL_sources
    ${CMAKE_SOURCE_DIR}/utilities/SC_GlobalEnums.h

    # AiCvterOpt.h
    Colors.h
    HeightLine.h

    OptiChain.h
    PrimGlassBuilder.h
    SCLDefines.h
    TokiSlopeCraft.h
    WaterItem.h
    lossyCompressor.h

    simpleBlock.h
    SlopeCraftL.h
    SlopeCraftL_global.h
    WriteStringDeliver.h

    # AiCvterOpt.cpp
    Colors.cpp
    ColorSource.cpp
    HeightLine.cpp
    OptiChain.cpp
    PrimGlassBuilder.cpp
    TokiSlopeCraft.cpp
    TokiSlopeCraft_static_funs.cpp
    TokiSlopeCraft_build.cpp
    TokiSlopeCraft_convert.cpp
    TokiSlopeCraft_cache.cpp
    WaterItem.cpp
    imagePreprocess.cpp
    lossyCompressor.cpp
    simpleBlock.cpp
    SlopeCraftL.cpp

    ${SlopeCraft_SCL_windows_rc_files}
)

# add this definations to both targets
add_definitions(-DSLOPECRAFTL_LIBRARY)

add_library(SlopeCraftL SHARED ${SlopeCraft_SCL_sources})

# add_library(SlopeCraftL_C SHARED ${SlopeCraft_SCL_sources})
target_compile_features(SlopeCraftL PRIVATE cxx_std_20)
target_include_directories(SlopeCraftL INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)
target_include_directories(SlopeCraftL PRIVATE
    ${SlopeCraft_Nlohmann_json_include_dir}
    ${SlopeCraft_Chocobo1_Hash_include_dir}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/utilities)

set(SlopeCraft_SCL_link_libs
    GAConverter
    ColorManip
    NBTWriter
    Schem
    MapImageCvter
    FlatDiagram

    ZLIB::ZLIB
    OpenMP::OpenMP_CXX
    fmt::fmt
    Eigen3::Eigen
    Heu::Genetic
    Boost::iostreams
    ${SC_zstd_target_name})

# link to dependents
target_link_libraries(SlopeCraftL PUBLIC $<BUILD_INTERFACE:${SlopeCraft_SCL_link_libs}>)

target_compile_options(SlopeCraftL PRIVATE ${SlopeCraft_vectorize_flags})

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_options(SlopeCraftL PRIVATE "-fvisibility=hidden" "-fPIC")
endif ()

# set shared-lib properties
set_target_properties(SlopeCraftL PROPERTIES
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "github.com.ToKiNoBug.SlopeCraft"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(SlopeCraftL PROPERTIES
        PREFIX "")
endif ()

include(install.cmake)