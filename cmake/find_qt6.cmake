
set(temp_is_SQrd_valid OFF)

if(DEFINED SlopeCraft_Qt_root_dir)
    if(EXISTS ${SlopeCraft_Qt_root_dir}/bin/qmake${CMAKE_EXECUTABLE_SUFFIX})
        message(STATUS "The value of SlopeCraft_Qt_root_dir is valid.")
        set(temp_is_SQrd_valid ON)
    else()
        message(WARNING "The value of SlopeCraft_Qt_root_dir may be invalid, failed to find qmake in directory " ${SlopeCraft_Qt_root_dir}/bin)
    endif()
endif()

if(${temp_is_SQrd_valid})
    list(PREPEND CMAKE_PREFIX_PATH ${SlopeCraft_Qt_root_dir})
endif()

message(STATUS "Finding Qt6. CMAKE_PREIFX_PATH = ${CMAKE_PREIFX_PATH}")
find_package(QT NAMES Qt6 COMPONENTS Widgets LinguistTools REQUIRED)

if(NOT ${QT_FOUND})
    message(FATAL_ERROR "Failed to found Qt6.")
endif()

unset(temp_is_SQrd_valid)
