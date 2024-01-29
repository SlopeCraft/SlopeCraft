set(AppName vccl)

#configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
#    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
#    @ONLY)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS vccl
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .)
    install(FILES vccl-config.json
        DESTINATION .)

    QD_add_deployqt(vccl
        BUILD_MODE
        FLAGS ${SlopeCraft_windeployqt_flags_build})
    QD_add_deployqt(vccl
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_windeployqt_flags_install})
    DLLD_add_deploy(vccl
        BUILD_MODE
        INSTALL_MODE INSTALL_DESTINATION .
        IGNORE VisualCraftL.dll libVisualCraftL.dll)

    #    # Run windeployqt at build time
    #    add_custom_target(Windeployqt-vccl
    #        COMMAND ${SlopeCraft_Qt_windeployqt_executable} vccl.exe ${SlopeCraft_windeployqt_flags_build}
    #        COMMAND_EXPAND_LISTS
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    #        DEPENDS vccl)
    #    add_dependencies(SC_deploy_all Windeployqt-vccl)
    #
    #    # Run windeployqt at install time
    #    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)
    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS vccl
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    install(FILES vccl-config.json
        DESTINATION bin)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)

    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(vccl_prefix vccl-contents)
    set(deployqt_vccl_script ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt_for_vccl_macos.cmake)
    find_program(SlopeCraft_Qt_macdeployqt_executable macdeployqt REQUIRED)
    configure_file(deploy_qt_for_vccl_macos.cmake.in
        ${deployqt_vccl_script}
        @ONLY)

    include(${CMAKE_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
    install(TARGETS vccl
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION ${vccl_prefix}
        BUNDLE DESTINATION ${vccl_prefix})

    # Install config json file, vccl will try to find it by ./vccl-config.json
    install(FILES vccl-config.json
        DESTINATION ${vccl_prefix}/vccl.app/Contents/MacOS)

    # Install zips. In vccl-config.json or vc-config.json, they are referred like ./Blocks_VCL/Vanilla_1_19_3.zip
    install(FILES ${VCL_app_files}
        DESTINATION ${vccl_prefix}/vccl.app/Contents/MacOS/Blocks_VCL)

    install(TARGETS VisualCraftL
        #EXPORT SlopeCraftTargets
        RUNTIME DESTINATION ${vccl_prefix}/vccl.app/Contents/Frameworks
        LIBRARY DESTINATION ${vccl_prefix}/vccl.app/Contents/Frameworks)

    # Do not run deploy_qt.cmake, but a specialied one
    install(SCRIPT ${deployqt_vccl_script}
        DESTINATION .)

    DylibD_add_deploy(vccl
        INSTALL_DESTINATION ${vccl_prefix})
    RCS_add_codesign(vccl
        INSTALL_DESTINATION ${vccl_prefix})

    return()
endif ()