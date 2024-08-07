set(SlopeCraft_Eigen3_found OFF)
find_package(Eigen3 3.4.0 QUIET)

if (${Eigen3_FOUND})
    message(STATUS "EIGEN3_INCLUDE_DIR = ${EIGEN3_INCLUDE_DIR}")
    return()
endif ()

include(FetchContent)

FetchContent_Declare(Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(Eigen3)

find_package(Eigen3 3.4.0 REQUIRED)