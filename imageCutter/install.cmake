set(AppName imageCutter)
#configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
#    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
#    @ONLY)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS imageCutter
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
    )

    QD_add_deployqt(imageCutter
        BUILD_MODE
        FLAGS ${SlopeCraft_windeployqt_flags_build})
    QD_add_deployqt(imageCutter
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_windeployqt_flags_install})
    DLLD_add_deploy(imageCutter
        BUILD_MODE
        INSTALL_MODE INSTALL_DESTINATION .)
    # Run windeployqt at build time
    #    add_custom_target(Windeployqt-imageCutter
    #        COMMAND ${SlopeCraft_Qt_windeployqt_executable} imageCutter.exe ${SlopeCraft_windeployqt_flags_build}
    #        COMMAND_EXPAND_LISTS
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    #        DEPENDS imageCutter)
    #    add_dependencies(SC_deploy_all Windeployqt-imageCutter)
    #
    #    # Run windeployqt at install time
    #    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)


    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    # et_target_properties(imageCutter PROPERTIES INSTALL_RPATH "../lib")
    install(TARGETS imageCutter
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION lib
    )

    install(FILES others/imageCutterIconNew.png
        DESTINATION share/pixmaps
        RENAME com.github.SlopeCraft.imageCutter.png)
    install(FILES others/imageCutter.desktop
        DESTINATION share/applications)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)
    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS imageCutter
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Install icon for macOS
    file(GLOB imageCutter_Icon
        ${CMAKE_SOURCE_DIR}/imageCutter/others/imageCutterIconNew.icns)
    install(FILES ${imageCutter_Icon}
        DESTINATION imageCutter.app/Contents/Resources)

    QD_add_deployqt(imageCutter
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_macdeployqt_flags_install})

    DylibD_add_deploy(imageCutter
        INSTALL_DESTINATION .
        RPATH_POLICY REPLACE)

    RCS_add_codesign(imageCutter
        INSTALL_DESTINATION .)

    return()
endif ()

message(WARNING "No rule to install imageCutter, because the system is not Windows, linux or MacOS.")