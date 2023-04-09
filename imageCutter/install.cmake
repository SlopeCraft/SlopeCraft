set(AppName imageCutter)
configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS imageCutter
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Run windeployqt at build time
    add_custom_target(Windeployqt-imageCutter ALL
        COMMAND ${SlopeCraft_Qt_windeployqt_executable} imageCutter.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS imageCutter)

    # Run windeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # et_target_properties(imageCutter PROPERTIES INSTALL_RPATH "../lib")
    install(TARGETS imageCutter
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION lib
    )

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS imageCutter
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

    # Install icon for macOS
    file(GLOB imageCutter_Icon
        ${CMAKE_SOURCE_DIR}/imageCutter/others/imageCutterIconNew.icns)
    install(FILES ${imageCutter_Icon}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/imageCutter.app/Contents/Resources)

    return()
endif()

message(WARNING "No rule to install imageCutter, because the system is not Windows, linux or MacOS.")