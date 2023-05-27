
if(${APPLE})
    set(CMAKE_FIND_FRAMEWORK LAST)
endif()

find_package(PNG 1.6 REQUIRED)

if(${PNG_FOUND})
    return()
endif()

include(FetchContent)

FetchContent_Declare(PNG
    GIT_REPOSITORY https://github.com/glennrp/libpng
    GIT_TAG v1.6.39
    OVERRIDE_FIND_PACKAGE)

FetchContent_MakeAvailable(PNG)

find_package(PNG 1.6 REQUIRED CONFIG)