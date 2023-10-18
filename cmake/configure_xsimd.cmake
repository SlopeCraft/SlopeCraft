find_package(xsimd QUIET)
if (NOT ${xsimd_FOUND})
    message(FATAL_ERROR "xsimd is not installed. You may install it with vcpkg")
endif ()

#include(FetchContent)
#
#FetchContent_Declare(xsimd
#    GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
#    GIT_TAG 11.1.0
#    OVERRIDE_FIND_PACKAGE)
#
#FetchContent_MakeAvailable(xsimd)
#
#find_package(xsimd REQUIRED)