find_package(gapp 1.0.1 CONFIG)

if (${gapp_FOUND})
    # Usually if gapp is found externally, gapp.hpp is located at <gapp/gapp.hpp>.
    # But fetched version should include <gapp.hpp>. To fix this fucking stupid error, we always include <gapp.h>
    find_file(gapp_header_loc
        NAMES gapp.hpp
        HINTS ${gapp_DIR}/../../include/gapp
        /usr/include/gapp
        /usr/local/include/gapp
    )
    if (NOT gapp_header_loc)
        message(WARNING "Package gapp is found, but failed to find gapp.hpp. May encounter build error.")
        return()
    endif ()
    cmake_path(GET gapp_header_loc PARENT_PATH gapp_header_dir)
    target_include_directories(gapp::gapp INTERFACE ${gapp_header_dir})

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