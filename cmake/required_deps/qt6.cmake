
message(STATUS "Searching for Qt6. CMAKE_PREFIX_PATH = " ${CMAKE_PREFIX_PATH})
find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)

# find lupdate executable
if (${SlopeCraft_update_ts_files} AND (NOT DEFINED SlopeCraft_Qt_lupdate_executable))
    find_program(SlopeCraft_Qt_lupdate_executable name lupdate PATHS ${CMAKE_PREFIX_PATH} REQUIRED)
    message(STATUS "Found lupdate at : " ${SlopeCraft_Qt_lupdate_executable})

    if (${SlopeCraft_update_ts_no_obsolete})
        set(SlopeCraft_ts_flags)
    else ()
        set(SlopeCraft_ts_flags "-no-obsolete")
    endif ()
endif ()

