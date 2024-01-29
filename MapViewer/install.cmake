set(AppName MapViewer)
#configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
#    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
#    @ONLY)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS MapViewer
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
    )

    QD_add_deployqt(MapViewer
        BUILD_MODE
        FLAGS ${SlopeCraft_windeployqt_flags_build})
    QD_add_deployqt(MapViewer
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_windeployqt_flags_install})
    DLLD_add_deploy(MapViewer
        BUILD_MODE
        INSTALL_MODE INSTALL_DESTINATION .)

    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    # set_target_properties(MapViewer PROPERTIES INSTALL_RPATH "../lib")
    install(TARGETS MapViewer
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION lib
    )

    install(FILES others/MapViewer_64.png
        DESTINATION share/pixmaps
        RENAME com.github.SlopeCraft.MapViewer.png)
    install(FILES others/MapViewer.desktop
        DESTINATION share/applications)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)
    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS MapViewer
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Install icon for macOS
    # MapViewer.app
    file(GLOB MapViewer_Icon ${CMAKE_SOURCE_DIR}/MapViewer/others/MapViewer.icns)
    install(FILES ${MapViewer_Icon}
        DESTINATION MapViewer.app/Contents/Resources)

    QD_add_deployqt(MapViewer
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_macdeployqt_flags_install})

    DylibD_add_deploy(MapViewer
        INSTALL_DESTINATION .
        RPATH_POLICY REPLACE)
    RCS_add_codesign(MapViewer
        INSTALL_DESTINATION .)

    return()
endif ()

message(WARNING "No rule to install MapViewer, because the system is not Windows, linux or MacOS.")