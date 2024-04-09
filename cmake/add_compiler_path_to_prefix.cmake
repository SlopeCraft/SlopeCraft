if (NOT DEFINED CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "Cpp compiler not found : CMAKE_CXX_COMPILER is not defined.")
endif ()

# message("CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH temp)
cmake_path(GET temp PARENT_PATH temp)

list(FIND CMAKE_PREFIX_PATH temp out_temp)

if (NOT ${temp} IN_LIST CMAKE_PREFIX_PATH)
    message(STATUS "Added the installation prefix of compiler to CMAKE_PREFIX_PATH")
    list(APPEND CMAKE_PREFIX_PATH ${temp})

    # set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${temp})
    message("CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
endif ()

unset(temp)
unset(out_temp)
