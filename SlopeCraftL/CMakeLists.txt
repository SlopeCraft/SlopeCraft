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
find_package(cereal REQUIRED)
#find_package(zstd REQUIRED)

set(SlopeCraft_SCL_windows_rc_files)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring on Windows. Adding rc file to " ${PROJECT_NAME})
    configure_file(others/SlopeCraftL.rc.in others/SlopeCraftL.rc)
    set(SlopeCraft_SCL_windows_rc_files
        ${CMAKE_CURRENT_BINARY_DIR}/others/SlopeCraftL.rc)
endif ()

set(Slope_SCL_internal_headers
    ${CMAKE_SOURCE_DIR}/utilities/SC_GlobalEnums.h

    # AiCvterOpt.h
    Colors.h
    height_line.h
    optimize_chain.h
    prim_glass_builder.h
    SCLDefines.h
    TokiSlopeCraft.h
    WaterItem.h
    lossyCompressor.h
    mc_block.h
    SlopeCraftL_global.h
    WriteStringDeliver.h
    Colorset.h
    structure_3D.h
    color_table.h
    converted_image.h
)

set(SlopeCraft_SCL_sources


    SlopeCraftL.h
    # AiCvterOpt.cpp
    Colors.cpp
    ColorSource.cpp
    height_line.cpp
    optimize_chain.cpp
    prim_glass_builder.cpp
    TokiSlopeCraft.cpp
    TokiSlopeCraft_static_funs.cpp
    TokiSlopeCraft_build.cpp
    TokiSlopeCraft_convert.cpp
    TokiSlopeCraft_cache.cpp
    imagePreprocess.cpp
    lossyCompressor.cpp
    mc_block.cpp
    SlopeCraftL.cpp
    Colorset.cpp
    color_table.cpp
    structure_3D.cpp
    converted_image.cpp

    #${SlopeCraft_SCL_internal_headers}
    ${SlopeCraft_SCL_windows_rc_files}
)

# add this definations to both targets
add_definitions(-DSLOPECRAFTL_LIBRARY)

add_library(SlopeCraftL SHARED ${SlopeCraft_SCL_sources})

# add_library(SlopeCraftL_C SHARED ${SlopeCraft_SCL_sources})
target_compile_features(SlopeCraftL PRIVATE cxx_std_23)
target_precompile_headers(SlopeCraftL PRIVATE ${SlopeCraft_SCL_internal_headers})
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
    cereal::cereal
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
    #    file(ARCHIVE_CREATE OUTPUT temp.zip PATHS ${src_dir} "${src_dir}/block_list.json" FORMAT zip
    #        #COMPRESSION Zstd
    #        #COMPRESSION_LEVEL 9
    #    )

    include(${CMAKE_SOURCE_DIR}/cmake/optional_deps/7z.cmake)

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