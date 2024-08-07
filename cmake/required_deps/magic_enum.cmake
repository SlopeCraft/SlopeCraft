# https://github.com/Neargye/magic_enum/releases/download/v0.8.2/magic_enum.hpp

cmake_minimum_required(VERSION 3.14)

find_package(magic_enum 0.8.2 QUIET)

if (${magic_enum_FOUND})
    return()
endif ()

include(FetchContent)

FetchContent_Declare(magic_enum

    # URL https://github.com/Neargye/magic_enum/releases/download/v0.8.2/magic_enum.hpp
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG "v0.9.6"
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)

message(STATUS "Downaloding magic_enum......")

FetchContent_MakeAvailable(magic_enum)