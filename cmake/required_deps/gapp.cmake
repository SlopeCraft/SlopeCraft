find_package(gapp 1.0.1 CONFIG)

if (${gapp_FOUND})
    return()
endif ()

include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FetchContent_Declare(gapp
    GIT_REPOSITORY https://github.com/KRM7/gapp
    GIT_TAG v1.0.1
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)
message(STATUS "Downloading gapp")
FetchContent_MakeAvailable(gapp)

find_package(gapp 1.0.1 CONFIG REQUIRED)

unset(BUILD_SHARED_LIBS)