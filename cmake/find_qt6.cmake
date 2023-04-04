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
find_package(QT NAMES Qt6 COMPONENTS Widgets LinguistTools REQUIRED)

if(${WIN32})
    find_program(SlopeCraft_Qt_windeployqt_executable windeployqt PATHS ${CMAKE_PREFIX_PATH})
endif()

if(${APPLE})
    find_program(SlopeCraft_Qt_macdeployqt_executable macdeployqt PATHS ${CMAKE_PREFIX_PATH})
endif()
