include(FetchContent)

FetchContent_Declare(xsimd
    GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
    GIT_TAG 11.1.0
    OVERRIDE_FIND_PACKAGE)

FetchContent_MakeAvailable(xsimd)

find_package(xsimd REQUIRED)