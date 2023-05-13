cmake_minimum_required(VERSION 3.20)

# set version -----------------------------------------------------------------
set(SlopeCraft_version 5.0.1)

# set basic project attributes ------------------------------------------------
project(SlopeCraft VERSION ${SlopeCraft_version} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

if(${LINUX})
    message(STATUS "setting runpath for linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

    # set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    # set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

enable_testing()

# configure options -----------------------------------------------------------
set(SlopeCraft_GPU_API "OpenCL" CACHE STRING "API used to compute. Valid values : OpenCL, None. Metal may be supported.")

option(SlopeCraft_vectorize "Compile with vectorization" ON)

option(SlopeCraft_update_ts_files "Update language files before build. If it is set to ON, everytime CMake reconfigure the project, all .ts files will be updated." OFF)

option(SlopeCraft_update_ts_no_obsolete "Remove obsolete translations from ts files." OFF)

option(SlopeCraft_gprof "Profile with gprof" OFF)

option(SlopeCraft_sanitize "Build with sanitizer" OFF)

# manage the install prefix --------------------------------------------------
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
endif()

# convert relative path to absolute ------------------------------------------
cmake_path(ABSOLUTE_PATH CMAKE_INSTALL_PREFIX OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX)
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

# Remove old compilation database --------------------------------------------
if(EXISTS ${CMAKE_SOURCE_DIR}/compile_commands.json)
    # delete files here
    file(REMOVE ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()

# warn if not gcc ------------------------------------------------------------
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(WARNING "MSVC or Clang are not fully supported. 
        You MAY meet werid errors if you continue to build.
        Please use gcc.")
endif()

# configure global dependencies ----------------------------------------------
include(cmake/add_compiler_path_to_prefix.cmake)
include(cmake/find_nlohmann_json.cmake)
include(cmake/find_Eigen3.cmake)
include(cmake/find_HeuristicFlow.cmake)
include(cmake/find_qt6.cmake)

if(${WIN32})
    include(cmake/install_if_is_shared.cmake)
endif()

# Add compile options --------------------------------------------------------
# if don't vectorize, or no suitable flag found, this variable will be empty
set(SlopeCraft_vectorize_flags)

if(NOT ${SlopeCraft_vectorize})
    if(NOT ${SlopeCraft_GPU_API} STREQUAL "None")
        message(STATUS "You enabled GPU boosting, but disabled vectorization")
    endif()
endif()

if(${SlopeCraft_vectorize})
    include(CheckCXXCompilerFlag)
    add_definitions("-DSC_VECTORIZE_AVX2")

    if(${MSVC})
        set(SlopeCraft_vectorize_flags "/arch:AVX2")
    else()
        set(SlopeCraft_vectorize_flags -mavx -mavx2 -mfma)
    endif()

    message(STATUS "Vectorize using " ${SlopeCraft_vectorize_flags})
    add_compile_options(${SlopeCraft_vectorize_flags})

    # list(APPEND SlopeCraft_vectorize_flags "-O3")
endif()

if(${WIN32})
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Add global compile options -------------------------------------------------
# profile with gprof
if(${SlopeCraft_gprof})
    add_compile_options("-pg")
    add_link_options("-pg")
endif()

if(${SlopeCraft_sanitize})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fsanitize=address")
endif()

add_compile_definitions(_USE_MATH_DEFINES)
add_compile_definitions(SCL_FIND_GlobalEnums_BY_PATH)
add_compile_definitions(SLOPECRAFTL_NOT_INSTALLED)
add_compile_definitions(VISUALCRAFTL_NOT_INSTALLED)

# Add compile options for different compilers
if(${MSVC})
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/wd4819")
    add_compile_options("/utf-8")

# add_compile_definitions("ZLIB_WINAPI")
# 1add_compile_definitions("Z_HAVE_UNISTD_H")
else()
    add_compile_options(-Wall -Wreturn-type -Wsign-compare -Wextra)
    add_link_options(-flto)
endif()

# Add global include dirs ----------------------------------------------------
include_directories(${CMAKE_BINARY_DIR}/utilities)

# Add global custom targets on windows ---------------------------------------
if(${WIN32})
    add_custom_target(SC_create_all_symlinks
        COMMENT "A interface target to create all symlinks on windows")

    add_custom_target(SC_deploy_all
        COMMENT "A interface target to run windeployqt for all targets")
endif()

# Sub dirss ------------------------------------------------------------------
add_subdirectory(utilities)
add_subdirectory(imageCutter)
add_subdirectory(SlopeCraftL)

# add_subdirectory(SlopeCraftMain)
add_subdirectory(SlopeCraft)
add_subdirectory(MapViewer)
add_subdirectory(tests)
add_subdirectory(VisualCraftL)
add_subdirectory(VisualCraft)
add_subdirectory(vccl)

# install and pack -----------------------------------------------------------
include(cmake/install.cmake)
include(cpack/make-packs.cmake)