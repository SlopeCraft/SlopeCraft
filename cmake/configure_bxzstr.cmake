find_package(bxzstr)

if(${bxzstr_FOUND})
    return()
endif()

include(FetchContent)

set(BZIP2_FOUND off)
set(LIBLZMA_FOUND off)
set(ZSTD_FOUND off)

FetchContent_Declare(bxzstr
    GIT_REPOSITORY https://github.com/SlopeCraft/bxzstr
    GIT_TAG v1.2.1-fork
    OVERRIDE_FIND_PACKAGE)

message(STATUS "Configuring bxzstr ...")

FetchContent_MakeAvailable(bxzstr)