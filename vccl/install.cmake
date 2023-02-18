if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS vccl
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS vccl
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    return()
endif()