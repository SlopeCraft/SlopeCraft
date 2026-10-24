set(SlopeCraft_Eigen3_found OFF)
find_package(Eigen3 CONFIG)

if (${Eigen3_FOUND})
    message(STATUS "EIGEN3_INCLUDE_DIR = ${EIGEN3_INCLUDE_DIR}")
    return()
endif ()

if (${SlopeCraft_no_download})
    message(FATAL_ERROR "Eigen3 or 5.0.1 not found. SlopeCraft is forbidden to download. Install it and retry")
endif ()

include(FetchContent)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_LEAVE_TEST_IN_ALL_TARGET OFF)
FetchContent_Declare(Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG bc3b39870ecb690a623a3f49149a358b95c5781d #5.0.1
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(Eigen3)

find_package(Eigen3 5.0.1 REQUIRED)