find_program(z7_exe NAMES 7z 7za 7zr QUIET
    HINTS "C:/Program Files/7-Zip/7z.exe;/usr/bin/7z;"
    DOC "Find 7z to archive SlopeCraft block lists. 7z is required because file(ARCHIVE_CREATE in cmake too stupid to be used"
)

if (z7_exe)
    return()
endif ()

message(STATUS "7z not found, downloading...")
if (${WIN32})
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/3rdParty")
    file(DOWNLOAD "https://7-zip.org/a/7z2404-extra.7z"
        "${CMAKE_BINARY_DIR}/3rdParty/7z2404-extra.7z"
        EXPECTED_HASH SHA512=459C9CEFB6587BD0836CAABC106265CFBABA57D1C00F9444F0FD9A6773E199080DDF02ABB7D8E2FD461C484F61D07E0363E1448ADEDADF6E274DAED96CA4A5E6)
    file(ARCHIVE_EXTRACT INPUT "${CMAKE_BINARY_DIR}/3rdParty/7z2404-extra.7z"
        DESTINATION "${CMAKE_BINARY_DIR}/3rdParty/7z"
        PATTERNS 7za.dll 7za.exe 7zxa.dll
        VERBOSE)
    set(z7_exe "${CMAKE_BINARY_DIR}/3rdParty/7z/7za.exe")
    if (NOT EXISTS ${z7_exe})
        message(FATAL_ERROR "Failed to extract 7z2404-extra.7z, ${z7_exe} doesn't exist")
    endif ()

    return()
endif ()

message(FATAL_ERROR "Install 7z with your package manager")

#list(GET z7_exe 0 z7_exe)