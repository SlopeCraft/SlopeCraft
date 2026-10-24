
find_package(CLI11 CONFIG QUIET)

if (CLI11_FOUND)
    return()
endif ()

message(STATUS "Failed to find CLI11 by find_package. Try manually")

if (NOT TARGET CLI11)
    add_library(CLI11 INTERFACE)
endif ()
if (NOT TARGET CLI11::CLI11)
    add_library(CLI11::CLI11 ALIAS CLI11)
endif ()

find_file(cli11_location
    NAMES CLI.hpp
    PATH_SUFFIXES CLI
    NO_CACHE
)
#message(STATUS "cli11_location = ${cli11_location}")

if (NOT cli11_location)
    if (${SlopeCraft_no_download})
        message(FATAL_ERROR "CLI.hpp is not found. SlopeCraft is forbidden to download. Please install it and try again")
    endif ()

    message(STATUS "Downloading cli11.hpp ......")
    set(cli11_location "${CMAKE_BINARY_DIR}/3rdParty/CLI/CLI.hpp")
    file(DOWNLOAD
        https://github.com/CLIUtils/CLI11/releases/download/v2.7.2/CLI11.hpp
        ${cli11_location}
        EXPECTED_HASH SHA256=ffa9a30da295c5858fb5f91f9f45771bab09471d7010a34c7c68c857a330dd76
    )
endif ()

message(STATUS "CLI.hpp found/downloaded at ${cli11_location}")
cmake_path(GET cli11_location PARENT_PATH cli11_include_dir)
cmake_path(GET cli11_include_dir PARENT_PATH cli11_include_dir)
target_include_directories(CLI11 INTERFACE ${cli11_include_dir})
