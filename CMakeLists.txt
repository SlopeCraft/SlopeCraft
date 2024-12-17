cmake_minimum_required(VERSION 3.20)

# set version -----------------------------------------------------------------
set(SlopeCraft_version 5.3.0)

# set basic project attributes ------------------------------------------------
project(SlopeCraft VERSION ${SlopeCraft_version} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

if (${LINUX})
    message(STATUS "setting runpath for linux")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

    # set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    # set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif ()

enable_testing()

# configure options -----------------------------------------------------------
if (${APPLE})
    set(SlopeCraft_GPU_API "None" CACHE STRING "API used to compute. Valid values : OpenCL, None. Metal may be supported.")
    option(SlopeCraft_vectorize "Compile with vectorization" OFF)
    message(STATUS "GPU boosting and vectorization have been disabled on mac by default")

else ()
    set(SlopeCraft_GPU_API "OpenCL" CACHE STRING "API used to compute. Valid values : OpenCL, None. Metal may be supported.")
    option(SlopeCraft_vectorize "Compile with vectorization" ON)
endif ()


option(SlopeCraft_update_ts_files "Update language files before build. If it is set to ON, everytime CMake reconfigure the project, all .ts files will be updated." OFF)

option(SlopeCraft_update_ts_no_obsolete "Remove obsolete translations from ts files." OFF)

option(SlopeCraft_gprof "Profile with gprof" OFF)

option(SlopeCraft_sanitize "Build with sanitizer" OFF)

set(SlopeCraft_vccl_test_gpu_platform_idx 0 CACHE STRING "The opencl platform index used to test vccl")
set(SlopeCraft_vccl_test_gpu_device_idx 0 CACHE STRING "The opencl device index used to test vccl")

if (${WIN32})
    set(SlopeCraft_windeployqt_flags_install "--no-translations" CACHE STRING "Flags to run windeployqt during installation")
    set(SlopeCraft_windeployqt_flags_build "${SlopeCraft_windeployqt_flags_install};--force" CACHE STRING "Flags to run windeployqt in build dir")
endif ()

if (${APPLE})
    set(SlopeCraft_macdeployqt_flags_install "" CACHE STRING "Flags to run macdeployqt during installation")
    # set(SlopeCraft_macdeployqt_flags_build "${SlopeCraft_windeployqt_flags_install};--force" CACHE STRING "Flags to run macdeployqt in build dir")
endif ()
# manage the install prefix --------------------------------------------------
if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
endif ()

# convert relative path to absolute ------------------------------------------
cmake_path(ABSOLUTE_PATH CMAKE_INSTALL_PREFIX OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX)
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

# Remove old compilation database --------------------------------------------
if (EXISTS ${CMAKE_SOURCE_DIR}/compile_commands.json)
    # delete files here
    file(REMOVE ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif ()

# configure global dependencies ----------------------------------------------

include(cmake/add_compiler_path_to_prefix.cmake)

file(GLOB required_dep_cmake_files "${CMAKE_SOURCE_DIR}/cmake/required_deps/*.cmake")
foreach (file ${required_dep_cmake_files})
    include(${file})
endforeach ()


# Add compile options --------------------------------------------------------
# if don't vectorize, or no suitable flag found, this variable will be empty
set(SlopeCraft_vectorize_flags)

if (NOT ${SlopeCraft_vectorize})
    if (NOT ${SlopeCraft_GPU_API} STREQUAL "None")
        message(STATUS "You enabled GPU boosting, but disabled vectorization")
    endif ()
endif ()

if (${SlopeCraft_vectorize})
    include(cmake/select_vectorize_flag.cmake)
endif ()

if (${WIN32})
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif ()

# Add global compile options -------------------------------------------------
# profile with gprof
if (${SlopeCraft_gprof})
    add_compile_options("-pg")
    add_link_options("-pg")
endif ()

if (${SlopeCraft_sanitize})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fsanitize=address")
endif ()

add_compile_definitions(_USE_MATH_DEFINES)
add_compile_definitions(SCL_FIND_GlobalEnums_BY_PATH)
add_compile_definitions(SLOPECRAFTL_NOT_INSTALLED)
add_compile_definitions(VISUALCRAFTL_NOT_INSTALLED)

# Add compile options for different compilers
if (${MSVC})
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/wd4819")
    add_compile_options("/utf-8")
    add_compile_options("/EHsc")
else ()
    add_compile_options(-Wall -Wreturn-type -Wsign-compare -Wextra)
    add_link_options(-flto)
endif ()

# Add global include dirs ----------------------------------------------------
include_directories(${CMAKE_BINARY_DIR}/utilities)

# Add global custom targets on windows ---------------------------------------
if (${WIN32})
    add_custom_target(SC_create_all_symlinks
        COMMENT "A interface target to create all symlinks on windows")

    add_custom_target(SC_deploy_all
        COMMENT "A interface target to run windeployqt for all targets")
endif ()

# # Language related variables
# set(SC_lupdate_flags -no-ui-lines)
set(SC_lupdate_flags)

if (${SlopeCraft_update_ts_no_obsolete})
    list(APPEND SC_lupdate_flags "-no-obsolete")
endif ()

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
