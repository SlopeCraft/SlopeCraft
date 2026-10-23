find_package(mc_schem 1.3.0 CONFIG)

if (NOT mc_schem_FOUND)
    set(MC_SCHEM_INSTALL OFF)
    set(MC_SCHEM_ENABLE_TESTS OFF)
    message(STATUS "Failed to find mc_schem. Downloading...")
    FetchContent_Declare(mc_schem
        GIT_REPOSITORY https://github.com/SlopeCraft/mc_schem
        GIT_TAG a97ad3138847d587b40180328195362268fae3fd #Slightly after v1.3.0 (optimized for region statistic)
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