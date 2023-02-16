cmake_minimum_required(VERSION 3.20)

set(SlopeCraft_version 3.10.1)

project(SlopeCraft VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

if(${MSVC})
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/wd4819")
endif()

enable_testing()

if(${MSVC})
    message(WARNING "MSVC or MSVC-like compilers(cl or clang-cl) are not fully supported. 
    Please use a gnu-like compiler, for example, gcc/g++ or clang/clang++.")
endif()

# manage the install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install_app)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX})

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

# If it is set to ON, everytime CMake reconfigure the project, all .ts files will be updated.
if(NOT DEFINED SlopeCraft_update_ts_files)
    option(SlopeCraft_update_ts_files "Update language files before build" OFF)
endif()

# find lupdate executable
if(${SlopeCraft_update_ts_files} AND(NOT DEFINED SlopeCraft_Qt_lupdate_executable))
    find_program(SlopeCraft_Qt_lupdate_executable name lupdate PATHS ${CMAKE_PREFIX_PATH} REQUIRED)
    message(STATUS "Found lupdate at : " ${SlopeCraft_Qt_lupdate_executable})
endif()

if(NOT DEFINED SlopeCraft_vectorize)
    option(SlopeCraft_vectorize "Compile with vectorization" ON)
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

    # list(APPEND SlopeCraft_vectorize_flags "-O3")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # set(CMAKE_INSTALL_RPATH "../lib")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_compile_definitions("_CRT_SECURE_NO_WARNINGS")
endif()

add_subdirectory(utilities)
add_subdirectory(imageCutter)
add_subdirectory(SlopeCraftL)
add_subdirectory(SlopeCraftMain)
add_subdirectory(MapViewer)
add_subdirectory(tests)
add_subdirectory(VisualCraftL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SlopeCraft_readme_and_license_files

    LICENSE
    README.md
    README_EN.md
    docs/简体中文版协议.txt
)

install(FILES ${SlopeCraft_readme_and_license_files}
    DESTINATION ${CMAKE_INSTALL_PREFIX})
