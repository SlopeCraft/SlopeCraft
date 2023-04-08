set(AppName vccl)

configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS vccl
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES vccl-config.json
        DESTINATION ${CMAKE_INSTALL_PREFIX})

    # Run windeployqt at build time
    add_custom_target(Windeployqt-vccl ALL
        COMMAND ${SlopeCraft_Qt_windeployqt_executable} vccl.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS vccl)

    # Run windeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS vccl
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    install(FILES vccl-config.json
        DESTINATION bin)
        
    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)
    
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS vccl
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})

    # Install config json file, vccl will try to find it by ./vccl-config.json
    install(FILES vccl-config.json
    DESTINATION ${CMAKE_INSTALL_PREFIX}/vccl.app/Contents/MacOS)

    # Run macdeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

    return()
endif()