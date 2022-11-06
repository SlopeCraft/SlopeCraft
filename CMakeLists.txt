cmake_minimum_required(VERSION 3.20)

set(SlopeCraft_version 3.10.1)

project(SlopeCraft VERSION ${SlopeCraft_version} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

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

# manage the include dir of HeuristicFlow
# set(SlopeCraft_HeuristicFlow_include_dir D:/CppLibs/HeuristicFlow-1.6.2.1)

# manage the include dir of Eigen
# set(SlopeCraft_Eigen3_include_dir D:/CppLibs/libEigen3/include/eigen3)
include(cmake/find_nlohmann_json.cmake)
include(cmake/find_Eigen3.cmake)
include(cmake/find_HeuristicFlow.cmake)

if(NOT DEFINED SlopeCraft_Qt_root_dir)
    # set(SlopeCraft_Qt_root_dir "D:/ProgramFiles/Qt/Qt/Qt6.2.4-gcc12-shared")
endif()

message(STATUS "SlopeCraft_Qt_root_dir = " ${SlopeCraft_Qt_root_dir})

include(cmake/find_qt6.cmake)

message("SlopeCraft_Qt_root_dir = " ${SlopeCraft_Qt_root_dir})

message("Length of CMAKE_PREFIX_PATH = " ${temp_L})

# find lupdate and lrelease of qt
# include(cmake/find_qt_translate.cmake)

# If it is set to ON, everytime CMake reconfigure the project, all .ts files will be updated.
set(SlopeCraft_update_ts_files OFF)

if(${SlopeCraft_update_ts_files} AND NOT DEFINED SlopeCraft_Qt_lupdate_executable)
    if(NOT DEFINED SlopeCraft_Qt_root_dir)
        message(FATAL_ERROR "Please set the variable \"SlopeCraft_Qt_root_dir\". Otherwise cmake can\'t find lupdate")
    endif()

    set(SlopeCraft_Qt_lupdate_executable ${SlopeCraft_Qt_root_dir}/bin/lupdate)
endif()

if(NOT DEFINED SlopeCraft_use_Eigen_vectorize)
    set(SlopeCraft_use_Eigen_vectorize OFF)
endif()

add_definitions("-D_USE_MATH_DEFINES")
add_definitions("-DSCL_FIND_GlobalEnums_BY_PATH")

# if don't vectorize, or no suitable flag found, this variable will be empty
set(SlopeCraft_vectorize_flags)

if(${SlopeCraft_use_Eigen_vectorize})
    include(CheckCXXCompilerFlag)

    check_cxx_compiler_flag("-march=native" _march_native_works)
    check_cxx_compiler_flag("-xHost" _xhost_works)

    if(${_march_native_works})
        message(STATUS "Vectorize using -march=native")
        set(SlopeCraft_vectorize_flags "-march=native")
    elseif(${_xhost_works})
        message(STATUS "Vectorize using -xHost")
        set(SlopeCraft_vectorize_flags "-xHost")
    else()
        message(STATUS "No suitable compiler flag found for vectorization")
    endif()

    list(APPEND SlopeCraft_vectorize_flags "-O3")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_compile_options(-fPIC)
    set(CMAKE_INSTALL_RPATH ".")
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
