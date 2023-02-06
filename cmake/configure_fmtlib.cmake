cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 9.1.0

    # FETCHCONTENT_QUIET FALSE
    OVERRIDE_FIND_PACKAGE

    # FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/3rdParty
    # FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS

    # FIND_PACKAGE_ARGS find_fmt_args
)

FetchContent_MakeAvailable(fmt)

message(STATUS "fmt_POPULATED = " ${fmt_POPULATED})

# message(STATUS "find_fmt_args = " ${find_fmt_args})