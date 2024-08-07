cmake_minimum_required(VERSION 3.14)

set(SC_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

find_package(fmt 9.1.0 QUIET)

if (NOT ${fmt_FOUND})
    include(FetchContent)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG "10.1.1"
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL

        # QUIET false
        # FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/3rdParty
        # FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS FIND_PACKAGE_ARGS find_fmt_args
    )

    message(STATUS "Downaloding......")

    FetchContent_MakeAvailable(fmt)
    find_package(fmt REQUIRED)
endif ()

set(CMAKE_CXX_FLAGS ${SC_CMAKE_CXX_FLAGS})
unset(SC_CMAKE_CXX_FLAGS)

if (${MSVC})
    # enable c++ exceptions
    add_compile_options("/EHsc")

    # target_compile_options(fmt::fmt INTERFACE "/EHsc")
endif ()

# message(STATUS "fmt_POPULATED = " )

# message(STATUS "find_fmt_args = " ${find_fmt_args})
