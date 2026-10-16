if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include(${CMAKE_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
    install(TARGETS VisualCraft
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Install icons
    file(GLOB SlopeCraft_Icon
        ${CMAKE_SOURCE_DIR}/VisualCraft/others/VisualCraft.icns
    )
    install(FILES ${SlopeCraft_Icon}
        DESTINATION VisualCraft.app/Contents/Resources
    )

    # Install config json file, VisualCraft will try to find it by ./vc-config.json
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vc-config-install.json
        RENAME vc-config.json
        DESTINATION VisualCraft.app/Contents/MacOS
    )

    # Install zips. In vccl-config.json or vc-config.json, they are referred like ./Blocks_VCL/Vanilla_1_19_3.zip
    install(FILES ${VCL_app_files}
        DESTINATION VisualCraft.app/Contents/MacOS/Blocks_VCL
    )

    install(TARGETS VisualCraftL
        # EXPORT SlopeCraftTargets
        RUNTIME DESTINATION VisualCraft.app/Contents/Frameworks
        LIBRARY DESTINATION VisualCraft.app/Contents/Frameworks
    )

    QD_add_deployqt(VisualCraft
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_macdeployqt_flags_install}
    )

    DylibD_add_deploy(VisualCraft
        INSTALL_DESTINATION .
        RPATH_POLICY REPLACE
    )
    RCS_add_codesign(VisualCraft
        INSTALL_DESTINATION .
    )

    return()
endif ()