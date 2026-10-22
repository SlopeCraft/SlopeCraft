find_package(xsimd QUIET)
if (xsimd_FOUND)
    return()
endif ()

include(FetchContent)
# TODO: Update to later version of xsimd
FetchContent_Declare(xsimd
    GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
    GIT_TAG 8ffcae81fc57a6cd05abddc2cdf573bda7c7d44a #11.1.0
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(xsimd)

find_package(xsimd REQUIRED)