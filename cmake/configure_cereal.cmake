find_package(cereal)

if(${cereal_FOUND})
    return()
endif()

include(FetchContent)

FetchContent_Declare(cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    GIT_TAG v1.3.2
    OVERRIDE_FIND_PACKAGE)

message(STATUS "Configuring cereal ...")

FetchContent_MakeAvailable(cereal)