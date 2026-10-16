
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(vccl_prefix vccl-contents)
    set(deployqt_vccl_script ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt_for_vccl_macos.cmake)
    find_program(SlopeCraft_Qt_macdeployqt_executable macdeployqt REQUIRED)
    configure_file(deploy_qt_for_vccl_macos.cmake.in
        ${deployqt_vccl_script}
        @ONLY
    )

    include(${CMAKE_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
    install(TARGETS vccl
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION ${vccl_prefix}
        BUNDLE DESTINATION ${vccl_prefix}
    )

    # Install config json file, vccl will try to find it by ./vccl-config.json
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vccl-config-install.json
        DESTINATION ${vccl_prefix}/vccl.app/Contents/MacOS
        RENAME vccl-config.json
    )

    # Install zips. In vccl-config.json or vc-config.json, they are referred like ./Blocks_VCL/Vanilla_1_19_3.zip
    install(FILES ${VCL_app_files}
        DESTINATION ${vccl_prefix}/vccl.app/Contents/MacOS/Blocks_VCL
    )

    install(TARGETS VisualCraftL
        #EXPORT SlopeCraftTargets
        RUNTIME DESTINATION ${vccl_prefix}/vccl.app/Contents/Frameworks
        LIBRARY DESTINATION ${vccl_prefix}/vccl.app/Contents/Frameworks
    )

    # Do not run deploy_qt.cmake, but a specialied one
    install(SCRIPT ${deployqt_vccl_script}
        DESTINATION .
    )

    DylibD_add_deploy(vccl
        INSTALL_DESTINATION ${vccl_prefix}
    )
    RCS_add_codesign(vccl
        INSTALL_DESTINATION ${vccl_prefix}
    )

    return()
endif ()