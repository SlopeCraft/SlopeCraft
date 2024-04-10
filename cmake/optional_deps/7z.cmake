find_program(z7_exe NAMES 7z 7za 7zr REQUIRED
    HINTS "C:/Program Files/7-Zip/7z.exe;/usr/bin/7z"
    DOC "Find 7z to archive SlopeCraft block lists. 7z is required because file(ARCHIVE_CREATE in cmake too stupid to be used"
)

if (NOT z7_exe)
    message(FATAL_ERROR "7z not found, downloading...")

endif ()

list(GET z7_exe 0 z7_exe)