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