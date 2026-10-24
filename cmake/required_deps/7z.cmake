find_program(z7_exe NAMES 7z 7za 7zr QUIET
    HINTS "C:/Program Files/7-Zip/7z.exe;/usr/bin/7z;"
    DOC "Find 7z to archive SlopeCraft block lists. 7z is required because file(ARCHIVE_CREATE in cmake too stupid to be used"
    NO_CACHE
)

if (NOT z7_exe)
    if (${SlopeCraft_no_download})
        message(FATAL_ERROR "7z command line executable not found. SlopeCraft is forbidden to download. Install it and retry")
    endif ()

    message(STATUS "7z not found, downloading...")
    if (${WIN32})
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/3rdParty")
        file(DOWNLOAD "https://github.com/ip7z/7zip/releases/download/26.05/7z2605-extra.7z"
            "${CMAKE_BINARY_DIR}/3rdParty/7z2600-extra.7z"
            EXPECTED_HASH SHA256=f27a8f103c84785026edcba1f67faf1357225df9d19ddb0b7760d55947141bc7
        )
        file(ARCHIVE_EXTRACT INPUT "${CMAKE_BINARY_DIR}/3rdParty/7z2600-extra.7z"
            DESTINATION "${CMAKE_BINARY_DIR}/3rdParty/7z"
            PATTERNS 7za.dll 7za.exe 7zxa.dll
            VERBOSE)
        set(z7_exe "${CMAKE_BINARY_DIR}/3rdParty/7z/7za.exe")
        if (NOT EXISTS ${z7_exe})
            message(FATAL_ERROR "Failed to extract 7z2600-extra.7z, ${z7_exe} doesn't exist")
        endif ()
    else ()
        message(FATAL_ERROR "Install 7z with your package manager")
    endif ()
endif ()

function(z7_create_archive)
    cmake_parse_arguments(z7ca
        "" #options
        "NAME;OUTPUT" #one value args
        "FILES;OPTIONS" # multi value args
        ${ARGN}
    )

    add_custom_target(${z7ca_NAME} ALL
        COMMAND ${z7_exe} ${z7ca_OPTIONS} a ${z7ca_OUTPUT} ${z7ca_FILES}
        COMMENT "Create archive ${z7ca_OUTPUT}"
        SOURCES ${z7ca_FILES}
        BYPRODUCTS ${z7ca_OUTPUT}
        VERBATIM
    )
endfunction()