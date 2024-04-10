
if(${APPLE})
    set(CMAKE_FIND_FRAMEWORK LAST)
endif()

find_package(PNG 1.6 REQUIRED)