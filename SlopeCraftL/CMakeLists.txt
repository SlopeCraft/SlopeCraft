project(SlopeCraft_SlopeCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)

find_package(ZLIB 1.2.11 REQUIRED)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Heu REQUIRED)
find_package(Boost COMPONENTS iostreams REQUIRED)
find_package(libzip REQUIRED)
find_package(tl-expected REQUIRED)
find_package(PNG REQUIRED)
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
    libpng_reader

    ZLIB::ZLIB
    OpenMP::OpenMP_CXX
    fmt::fmt
    Eigen3::Eigen
    Heu::Genetic
    Boost::iostreams
    libzip::zip
    tl::expected
    ${SC_zstd_target_name})

# link to dependents
target_link_libraries(SlopeCraftL PRIVATE $<BUILD_INTERFACE:${SlopeCraft_SCL_link_libs}>)

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

function(SC_create_SCL_block_list src_dir archive_path)
    if (NOT EXISTS "${src_dir}/BlockList.json")
        message(FATAL_ERROR "${src_dir}/BlockList.json doesn't exist, block list json is missing")
    endif ()

    cmake_path(GET archive_path PARENT_PATH archive_dir)
    file(MAKE_DIRECTORY ${archive_dir})

    if (EXISTS ${archive_path})
        file(REMOVE ${archive_path})
    endif ()

    #    file(GLOB_RECURSE file_list "${src_dir}/*.png")
    #
    #    file(ARCHIVE_CREATE OUTPUT temp.zip PATHS ${src_dir} "${src_dir}/BlockList.json" FORMAT zip
    #        #COMPRESSION Zstd
    #        #COMPRESSION_LEVEL 9
    #    )

    find_program(z7_exe NAMES 7z REQUIRED
        HINTS "C:/Program Files/7-Zip/7z.exe;/usr/bin/7z"
        DOC "Find 7z to archive SlopeCraft block lists. 7z is required because file(ARCHIVE_CREATE in cmake too stupid to be used"
        HINTS "C:/Program Files/7-Zip/7z.exe")

    execute_process(COMMAND ${z7_exe} a ${archive_path} -scsUTF-8 "${src_dir}/*" -tZIP
        COMMAND_ERROR_IS_FATAL ANY)


endfunction()

SC_create_SCL_block_list(${CMAKE_SOURCE_DIR}/Blocks/FixedBlocks "${CMAKE_BINARY_DIR}/SCL_block_lists/FixedBlocks.zip")
SC_create_SCL_block_list(${CMAKE_SOURCE_DIR}/Blocks/CustomBlocks "${CMAKE_BINARY_DIR}/SCL_block_lists/CustomBlocks.zip")


add_executable(test_scl_load_blocklist tests/load_scl_blocklist.cpp)
target_link_libraries(test_scl_load_blocklist PRIVATE SlopeCraftL)
target_compile_features(test_scl_load_blocklist PRIVATE cxx_std_23)
if (${WIN32})
    DLLD_add_deploy(SlopeCraftL BUILD_MODE)
    DLLD_add_deploy(test_scl_load_blocklist BUILD_MODE VERBOSE)
endif ()


include(install.cmake)