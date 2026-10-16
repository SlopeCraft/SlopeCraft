
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Install for macOS
    # Install app
    install(TARGETS SlopeCraft
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Install icons
    set(SlopeCraft_Icon
        ${CMAKE_SOURCE_DIR}/SlopeCraft/others/SlopeCraftIconNew.icns)
    install(FILES ${SlopeCraft_Icon}
        DESTINATION SlopeCraft.app/Contents/Resources
        RENAME SlopeCraft.icns)

    # Install all block list archives
    install(FILES ${SlopeCraft_install_presets}
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks_SCL)

    # Install presets
    install(FILES ${SlopeCraft_install_presets}
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks_SCL/Presets)

    install(TARGETS SlopeCraftL
        #EXPORT SlopeCraftTargets
        RUNTIME DESTINATION SlopeCraft.app/Contents/Frameworks
        LIBRARY DESTINATION SlopeCraft.app/Contents/Frameworks)

    QD_add_deployqt(SlopeCraft
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_macdeployqt_flags_install})

    DylibD_add_deploy(SlopeCraft
        INSTALL_DESTINATION .
        RPATH_POLICY REPLACE)
    RCS_add_codesign(SlopeCraft
        INSTALL_DESTINATION .)

    return()
endif ()
