
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS MapViewer
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # set_target_properties(MapViewer PROPERTIES INSTALL_RPATH "../lib")
    install(TARGETS MapViewer
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION lib
    )
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS MapViewer
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

    # Install icon for macOS
    # MapViewer.app
    file(GLOB MapViewer_Icon ${CMAKE_SOURCE_DIR}/MapViewer/others/MapViewer.icns)
    install(FILES ${MapViewer_Icon}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/MapViewer.app/Contents/Resources)
endif()

message(WARNING "No rule to install MapViewer, because the system is not Windows, linux or MacOS.")