if(CMAKE_SYSTEM_NAME MATCHES "Windows")

    install(TARGETS imageCutter
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
#et_target_properties(imageCutter PROPERTIES INSTALL_RPATH "../lib")
    install(TARGETS imageCutter
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION lib
    )

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