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

if (EXISTS ${CMAKE_SOURCE_DIR}/3rdParty/cli11/CLI11.hpp)
    message(STATUS "cli11 found at " ${CMAKE_SOURCE_DIR}/3rdParty/cli11/CLI11.hpp)
    set(cli11_include_dir ${CMAKE_SOURCE_DIR}/3rdParty/cli11)
    return()
endif ()

message(STATUS "Downloading cli11.hpp ......")

file(DOWNLOAD
    https://github.com/CLIUtils/CLI11/releases/download/v2.4.1/CLI11.hpp
    ${CMAKE_SOURCE_DIR}/3rdParty/cli11/CLI11.hpp
    EXPECTED_HASH SHA512=7DB1F5B879DF99639ADA29AD313D5E344E1B0FD30A44116DA00A6CA53BEF4BA840684CBC64290CF365569095B0915D181039777FCE780BC60E20F624623B98B9
    SHOW_PROGRESS)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdParty/cli11/CLI11.hpp)
    message(ERROR "Failed to download cli11.")
    return()
endif ()

message(STATUS "cli11 downloaded successfully.")
set(cli11_include_dir ${CMAKE_SOURCE_DIR}/3rdParty/cli11)
