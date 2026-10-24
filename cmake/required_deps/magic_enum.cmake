# https://github.com/Neargye/magic_enum/releases/download/v0.8.2/magic_enum.hpp

cmake_minimum_required(VERSION 3.28)

find_package(magic_enum QUIET)

if (NOT ${magic_enum_FOUND})
    
    if (${SlopeCraft_no_download})
        message(FATAL_ERROR "magic_enum not found. SlopeCraft is forbidden to download. Install it and retry")
    endif ()

    message(STATUS "Downloading magic_enum......")
    include(FetchContent)
    FetchContent_Declare(magic_enum

        # URL https://github.com/Neargye/magic_enum/releases/download/v0.8.2/magic_enum.hpp
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG 1384769c66bd16ec9bb1353f45fe8ec8ccc12dbd #v0.9.8
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )

    FetchContent_MakeAvailable(magic_enum)
endif ()

get_target_property(enum_target_real_name magic_enum::magic_enum ALIASED_TARGET)
if (enum_target_real_name)
    target_compile_definitions(${enum_target_real_name} INTERFACE
        MAGIC_ENUM_RANGE_MAX=65536
    )
else ()
    target_compile_definitions(magic_enum::magic_enum INTERFACE
        MAGIC_ENUM_RANGE_MAX=65536
    )
endif ()
