project(SlopeCraft_SlopeCraftL VERSION ${SlopeCraft_version} LANGUAGES CXX)

find_package(ZLIB 1.2.11 REQUIRED)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Heu REQUIRED)
find_package(Boost COMPONENTS iostreams OPTIONAL_COMPONENTS multi_array REQUIRED)
find_package(libzip REQUIRED)
find_package(tl-expected REQUIRED)
find_package(PNG REQUIRED)
find_package(cereal REQUIRED)
find_package(libnbt++ REQUIRED)
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

    # GA_converter_option.h
    SCLDefines.h
    water_item.h
    string_deliver.h
    structure_3D.h

    color_table.h
    converted_image.h
    height_line.h
    lossy_compressor.h
    mc_block.h
    optimize_chain.h
    prim_glass_builder.h
)

set(SlopeCraft_SCL_sources


    SlopeCraftL.h
    # GA_converter_option.cpp
    color_source.cpp
    height_line.cpp
    optimize_chain.cpp
    prim_glass_builder.cpp
    image_preprocess.cpp
    lossy_compressor.cpp
    mc_block.cpp
    SlopeCraftL.cpp
    color_table.cpp
    structure_3D.cpp
    converted_image.cpp

    #${SlopeCraft_SCL_internal_headers}
    ${SlopeCraft_SCL_windows_rc_files}
    block_list.cpp
    blocklist.h
)

# add this definations to both targets
add_definitions(-DSLOPECRAFTL_LIBRARY)

add_library(SlopeCraftL SHARED ${SlopeCraft_SCL_sources})

include(GenerateExportHeader)
generate_export_header(SlopeCraftL BASE_NAME SCL EXPORT_FILE_NAME SlopeCraftL_export.h)

# add_library(SlopeCraftL_C SHARED ${SlopeCraft_SCL_sources})
target_compile_features(SlopeCraftL PRIVATE cxx_std_23)
target_precompile_headers(SlopeCraftL PRIVATE ${SlopeCraft_SCL_internal_headers})
target_include_directories(SlopeCraftL PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
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
    sNBT-formatter
    version_set

    ZLIB::ZLIB
    OpenMP::OpenMP_CXX
    fmt::fmt
    Eigen3::Eigen
    Heu::Genetic
    Boost::iostreams
    Boost::multi_array
    libzip::zip
    tl::expected
    cereal::cereal
    nbt++
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
    if (NOT EXISTS "${src_dir}/block_list.json")
        message(FATAL_ERROR "${src_dir}/block_list.json doesn't exist, block list json is missing")
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

file(GLOB block_list_folders "${CMAKE_SOURCE_DIR}/Blocks/*")
foreach (dir ${block_list_folders})
    if (NOT IS_DIRECTORY ${dir})
        continue()
    endif ()
    cmake_path(GET dir FILENAME block_list_name)
    message(STATUS "Processing block list: ${dir}, block list name: \"${block_list_name}\"")
    SC_create_SCL_block_list(${dir} "${CMAKE_BINARY_DIR}/SCL_block_lists/${block_list_name}.zip")
endforeach ()

add_executable(test_scl_load_blocklist tests/load_scl_blocklist.cpp)
target_link_libraries(test_scl_load_blocklist PRIVATE SlopeCraftL)
target_compile_features(test_scl_load_blocklist PRIVATE cxx_std_23)
if (${WIN32})
    DLLD_add_deploy(SlopeCraftL BUILD_MODE)
    DLLD_add_deploy(test_scl_load_blocklist BUILD_MODE VERBOSE)
endif ()


include(install.cmake)