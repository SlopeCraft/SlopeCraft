# set(temp_is_SQrd_valid OFF)

# if(DEFINED SlopeCraft_Qt_root_dir)
# if(EXISTS ${SlopeCraft_Qt_root_dir}/bin/qmake${CMAKE_EXECUTABLE_SUFFIX})
# message(STATUS "The value of SlopeCraft_Qt_root_dir is valid.")
# set(temp_is_SQrd_valid ON)
# else()
# message(WARNING "The value of SlopeCraft_Qt_root_dir may be invalid, failed to find qmake in directory " ${SlopeCraft_Qt_root_dir}/bin)
# endif()
# endif()

# if(${temp_is_SQrd_valid})
# list(PREPEND CMAKE_PREFIX_PATH ${SlopeCraft_Qt_root_dir})
# endif()

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

if (${LINUX})
    if (NOT EXISTS ${Qt6_DIR})
        message(WARNING "Qt6_DIR is not set. No way to find platform plugins")
    endif ()

    # message(STATUS "Qt6_DIR = ${Qt6_DIR}")
    file(GLOB_RECURSE SlopeCraft_Qt_plugin_platform_files "${Qt6_DIR}/../../../plugins/platforms/*")
    file(GLOB_RECURSE SlopeCraft_Qt_plugin_imageformat_files "${Qt6_DIR}/../../../plugins/imageformats/*")

    # message(STATUS "Qt platform plugin files: ${SlopeCraft_Qt_plugin_platform_files}")
endif ()
