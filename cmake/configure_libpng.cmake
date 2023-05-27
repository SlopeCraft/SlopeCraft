
find_package(PNG 1.6)

if(${PNG_FOUND})
    return()
endif()

include(FetchContent)

FetchContent_Declare(libpng
    GIT_REPOSITORY https://github.com/glennrp/libpng.git
    GIT_TAG v1.6.39
    OVERRIDE_FIND_PACKAGE)

FetchContent_MakeAvailable(libpng)