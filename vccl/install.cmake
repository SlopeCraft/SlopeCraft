if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS vccl
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES vccl-config.json
        DESTINATION ${CMAKE_INSTALL_PREFIX})
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS vccl
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    install(FILES vccl-config.json
        DESTINATION bin)
    return()
endif()