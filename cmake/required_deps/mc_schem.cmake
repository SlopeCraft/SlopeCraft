find_package(mc_schem 1.3.2 CONFIG QUIET)

if (NOT mc_schem_FOUND)
    if (${SlopeCraft_no_download})
        message(FATAL_ERROR "mc_schem not found. SlopeCraft is forbidden to download. Install it from https://github.com/SlopeCraft/mc_schem and retry")
    endif ()

    set(MC_SCHEM_INSTALL OFF)
    set(MC_SCHEM_ENABLE_TESTS OFF)
    message(STATUS "Failed to find mc_schem. Downloading...")
    FetchContent_Declare(mc_schem
        URL https://github.com/SlopeCraft/mc_schem/archive/refs/tags/v1.4.0.tar.gz
        URL_HASH SHA256=32597f032335be157f6cba5f7a2dff0ada046492998d796807b66460f55d30c3
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
    foreach (t mc_schem mc_schem-shared mc_schem-static)
        if (NOT TARGET ${t})
            continue()
        endif ()
        set_property(TARGET ${t} PROPERTY INTERFACE_LINK_DIRECTORIES "")
    endforeach ()
endif ()