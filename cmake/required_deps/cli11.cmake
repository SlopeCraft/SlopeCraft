cmake_minimum_required(VERSION 3.19)

if (DEFINED cli11_include_dir)
    if (EXISTS ${cli11_include_dir}/CLI11.hpp)
        message(STATUS "cli11 found at " ${cli11_include_dir}/CLI11.hpp)
        return()
    else ()
        message(WARNING "Assigned cli11_include_dir to be " ${cli11_include_dir}
            " but failed to find CLI11.hpp")
        unset(cli11_include_dir)
    endif ()
endif ()

if (EXISTS ${CMAKE_BINARY_DIR}/3rdParty/cli11/CLI11.hpp)
    message(STATUS "cli11 found at " ${CMAKE_BINARY_DIR}/3rdParty/cli11/CLI11.hpp)
    set(cli11_include_dir ${CMAKE_BINARY_DIR}/3rdParty/cli11)
    return()
endif ()

message(STATUS "Downloading cli11.hpp ......")

file(DOWNLOAD
    https://github.com/CLIUtils/CLI11/releases/download/v2.7.2/CLI11.hpp
    ${CMAKE_BINARY_DIR}/3rdParty/cli11/CLI11.hpp
    EXPECTED_HASH SHA256=ffa9a30da295c5858fb5f91f9f45771bab09471d7010a34c7c68c857a330dd76
    SHOW_PROGRESS)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/3rdParty/cli11/CLI11.hpp)
    message(ERROR "Failed to download cli11.")
    return()
endif ()

message(STATUS "cli11 downloaded successfully.")
set(cli11_include_dir ${CMAKE_BINARY_DIR}/3rdParty/cli11)
