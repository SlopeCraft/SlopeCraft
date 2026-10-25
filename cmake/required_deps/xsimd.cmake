find_package(xsimd QUIET)
if (xsimd_FOUND)
    return()
endif ()

if (${SlopeCraft_no_download})
    message(FATAL_ERROR "xsimd not found. SlopeCraft is forbidden to download. Install it and retry")
endif ()

include(FetchContent)
# TODO: Update to later version of xsimd
FetchContent_Declare(xsimd
    GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
    GIT_TAG e88a72831858123924f7118f345dfe5d70d95991 #14.3.0
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(xsimd)
find_package(xsimd REQUIRED)