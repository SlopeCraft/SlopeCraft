set(AppName MapViewer)
configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
    @ONLY)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS MapViewer
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Run windeployqt at build time
    add_custom_target(Windeployqt-MapViewer ALL
        COMMAND ${SlopeCraft_Qt_windeployqt_executable} MapViewer.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS MapViewer)

    # Run windeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

    SlopeCraft_install_if_is_shared(ZLIB::ZLIB .)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # set_target_properties(MapViewer PROPERTIES INSTALL_RPATH "../lib")
    install(TARGETS MapViewer
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION lib
    )

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS MapViewer
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Install icon for macOS
    # MapViewer.app
    file(GLOB MapViewer_Icon ${CMAKE_SOURCE_DIR}/MapViewer/others/MapViewer.icns)
    install(FILES ${MapViewer_Icon}
        DESTINATION MapViewer.app/Contents/Resources)
    return()
endif()

message(WARNING "No rule to install MapViewer, because the system is not Windows, linux or MacOS.")