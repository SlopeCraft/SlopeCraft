
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