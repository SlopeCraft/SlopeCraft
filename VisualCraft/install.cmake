

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS VisualCraft
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS VisualCraft
        RUNTIME DESTINATION bin
    )
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    return()
endif()