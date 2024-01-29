set(AppName VisualCraft)


if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS VisualCraft
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
    )

    install(FILES vc-config.json
        DESTINATION .)

    QD_add_deployqt(VisualCraft
        BUILD_MODE
        FLAGS ${SlopeCraft_windeployqt_flags_build})
    QD_add_deployqt(VisualCraft
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_windeployqt_flags_install})
    DLLD_add_deploy(VisualCraft
        BUILD_MODE
        INSTALL_MODE INSTALL_DESTINATION .
        IGNORE VisualCraftL.dll libVisualCraftL.dll)

    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS VisualCraft
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
    )

    install(FILES vc-config.json
        DESTINATION bin)

    install(FILES others/Vc_256.png
        DESTINATION share/pixmaps
        RENAME com.github.SlopeCraft.VisualCraft.png)
    install(FILES others/VisualCraft.desktop
        DESTINATION share/applications)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)

    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include(${CMAKE_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
    install(TARGETS VisualCraft
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .)

    # Install icons
    file(GLOB SlopeCraft_Icon
        ${CMAKE_SOURCE_DIR}/VisualCraft/others/VisualCraft.icns)
    install(FILES ${SlopeCraft_Icon}
        DESTINATION VisualCraft.app/Contents/Resources)

    # Install config json file, VisualCraft will try to find it by ./vc-config.json
    install(FILES vc-config.json
        DESTINATION VisualCraft.app/Contents/MacOS)

    # Install zips. In vccl-config.json or vc-config.json, they are referred like ./Blocks_VCL/Vanilla_1_19_3.zip
    install(FILES ${VCL_app_files}
        DESTINATION VisualCraft.app/Contents/MacOS/Blocks_VCL
    )

    install(TARGETS VisualCraftL
        # EXPORT SlopeCraftTargets
        RUNTIME DESTINATION VisualCraft.app/Contents/Frameworks
        LIBRARY DESTINATION VisualCraft.app/Contents/Frameworks)

    QD_add_deployqt(VisualCraft
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_macdeployqt_flags_install})

    DylibD_add_deploy(VisualCraft
        INSTALL_DESTINATION .
        RPATH_POLICY REPLACE)
    RCS_add_codesign(VisualCraft
        INSTALL_DESTINATION .)

    return()
endif ()