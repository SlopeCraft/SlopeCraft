find_package(mc_schem 1.3.2 CONFIG QUIET)

if (NOT mc_schem_FOUND)
    if (${SlopeCraft_no_download})
        message(FATAL_ERROR "mc_schem not found. SlopeCraft is forbidden to download. Install it from https://github.com/SlopeCraft/mc_schem and retry")
    endif ()

    set(MC_SCHEM_INSTALL OFF)
    set(MC_SCHEM_ENABLE_TESTS OFF)
    message(STATUS "Failed to find mc_schem. Downloading...")
    FetchContent_Declare(mc_schem
        GIT_REPOSITORY https://github.com/SlopeCraft/mc_schem
        GIT_TAG 95c6f7190f1b9d526906a65c3c3127ad0c1c1daa # v1.3.2
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(mc_schem)
    find_package(mc_schem CONFIG REQUIRED)
endif ()

if (NOT mc_schem_SHARED_LIB_LOCATION)
    message(WARNING "mc_schem_SHARED_LIB_LOCATION is not defined or error. This is required for installation, install may fail")
endif ()

message(STATUS "Found mc_schem shared lib at ${mc_schem_SHARED_LIB_LOCATION}")

if (APPLE)
    # Corrosion v0.6.1 hardcodes the CommandLineTools SDK usr/lib into
    # INTERFACE_LINK_DIRECTORIES; it beats -isysroot in the linker search order.
    foreach (t mc_schem-shared mc_schem-static schemtool mc_schem::mc_schem-shared)
        if (TARGET ${t})
            set_property(TARGET ${t} PROPERTY INTERFACE_LINK_DIRECTORIES "")
        endif ()
    endforeach ()
endif ()