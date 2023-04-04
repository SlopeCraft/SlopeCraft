cmake_minimum_required(VERSION 3.20)

set(SlopeCraft_version 5.0.0)

project(SlopeCraft VERSION ${SlopeCraft_version} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

set(SlopeCraft_GPU_API "OpenCL" CACHE STRING "API used to compute. Valid values : OpenCL, None. Metal may be supported.")

option(SlopeCraft_vectorize "Compile with vectorization" ON)

option(SlopeCraft_update_ts_files "Update language files before build. If it is set to ON, everytime CMake reconfigure the project, all .ts files will be updated." OFF)

option(SlopeCraft_update_ts_no_obsolete "Remove obsolete translations from ts files." OFF)

option(SlopeCraft_gprof "Profile with gprof" OFF)

if(${MSVC})
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/wd4819")
    add_compile_definitions("ZLIB_WINAPI")
    add_compile_definitions("Z_HAVE_UNISTD_H")
else()
    add_compile_options(-Wall -Wreturn-type -Wsign-compare -Wextra)
    add_link_options(-flto)
endif()

enable_testing()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(WARNING "MSVC or Clang are not fully supported. 
    You MAY meet werid errors if you continue to build.
    Please use gcc.")
endif()

# manage the install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install
    CACHE PATH "Where to install SlopeCraft.")

# set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX})
# set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX})
if(EXISTS ${CMAKE_SOURCE_DIR}/compile_commands.json)
    # delete files here
    file(REMOVE ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()

include(cmake/add_compiler_path_to_prefix.cmake)

list(LENGTH CMAKE_PREFIX_PATH temp_L)

include(cmake/find_nlohmann_json.cmake)
include(cmake/find_Eigen3.cmake)
include(cmake/find_HeuristicFlow.cmake)

include(cmake/find_qt6.cmake)

# find lupdate executable
if(${SlopeCraft_update_ts_files} AND(NOT DEFINED SlopeCraft_Qt_lupdate_executable))
    find_program(SlopeCraft_Qt_lupdate_executable name lupdate PATHS ${CMAKE_PREFIX_PATH} REQUIRED)
    message(STATUS "Found lupdate at : " ${SlopeCraft_Qt_lupdate_executable})

    if(${SlopeCraft_update_ts_no_obsolete})
        set(SlopeCraft_ts_flags)
    else()
        set(SlopeCraft_ts_flags "-no-obsolete")
    endif()
endif()

add_definitions("-D_USE_MATH_DEFINES")
add_definitions("-DSCL_FIND_GlobalEnums_BY_PATH")

# if don't vectorize, or no suitable flag found, this variable will be empty
set(SlopeCraft_vectorize_flags)

if(${SlopeCraft_vectorize})
    include(CheckCXXCompilerFlag)

    add_definitions("-DSC_VECTORIZE_AVX2")

    if(${MSVC})
        set(SlopeCraft_vectorize_flags "/arch:AVX2")
    else()
        set(SlopeCraft_vectorize_flags -mavx -mavx2 -mfma)
    endif()

    message(STATUS "Vectorize using " ${SlopeCraft_vectorize_flags})

    # add_compile_options(${SlopeCraft_vectorize_flags})

    # list(APPEND SlopeCraft_vectorize_flags "-O3")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "setting runpath for linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_compile_definitions("_CRT_SECURE_NO_WARNINGS")
endif()

# profile with gprof
if(${SlopeCraft_gprof})
    add_compile_options("-pg")
    add_link_options("-pg")
endif()

add_compile_definitions(SLOPECRAFTL_NOT_INSTALLED)
add_compile_definitions(VISUALCRAFTL_NOT_INSTALLED)
include_directories(${CMAKE_BINARY_DIR}/utilities)

set(SlopeCraft_resource_file_to_touch_and_remove)
mark_as_advanced(SlopeCraft_resource_file_to_touch_and_remove)

add_subdirectory(utilities)
add_subdirectory(imageCutter)
add_subdirectory(SlopeCraftL)
add_subdirectory(SlopeCraftMain)
add_subdirectory(MapViewer)
add_subdirectory(tests)
add_subdirectory(VisualCraftL)
add_subdirectory(VisualCraft)
add_subdirectory(vccl)

if(EXISTS ${CMAKE_BINARY_DIR}/utilities/ColorManip/__rsc_ColorManip_cl_rc.c)
    file(SIZE ${CMAKE_BINARY_DIR}/utilities/ColorManip/__rsc_ColorManip_cl_rc.c size_of_file)

    if(${size_of_file} LESS_EQUAL 0)
        # file(REMOVE ${CMAKE_BINARY_DIR}/utilities/ColorManip/__rsc_ColorManip_cl_rc.c)
    endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SlopeCraft_readme_and_license_files

    LICENSE
    README.md
    README-en.md
    license-translations/LICENSE-zh.md
)

install(FILES ${SlopeCraft_readme_and_license_files}
    DESTINATION ${CMAKE_INSTALL_PREFIX})
