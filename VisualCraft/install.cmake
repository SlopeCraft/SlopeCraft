set(AppName VisualCraft)

configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
    @ONLY)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS VisualCraft
        RUNTIME DESTINATION .
    )

    install(FILES vc-config.json
        DESTINATION .)

    # Run windeployqt at build time
    add_custom_target(Windeployqt-VisualCraft ALL
        COMMAND ${SlopeCraft_Qt_windeployqt_executable} VisualCraft.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS VisualCraft)

    # Run windeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS VisualCraft
        RUNTIME DESTINATION bin
    )

    install(FILES vc-config.json
        DESTINATION bin)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS VisualCraft
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})

    # Install icons
    file(GLOB SlopeCraft_Icon
        ${CMAKE_SOURCE_DIR}/VisualCraft/others/VisualCraft.icns)
    install(FILES ${SlopeCraft_Icon}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/VisualCraft.app/Contents/Resources)

    # Install config json file, VisualCraft will try to find it by ./vc-config.json
    install(FILES vc-config.json
        DESTINATION ${CMAKE_INSTALL_PREFIX}/VisualCraft.app/Contents/MacOS)

    # Run macdeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

    return()
endif()