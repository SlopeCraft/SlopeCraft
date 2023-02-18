if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(FILES
        ${CMAKE_BINARY_DIR}/utilities/SC_version_buildtime.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/include)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(FILES
        ${CMAKE_BINARY_DIR}/utilities/SC_version_buildtime.h
        DESTINATION include)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    return()
endif()