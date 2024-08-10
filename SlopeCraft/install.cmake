file(GLOB SlopeCraft_block_list_archives
    "${CMAKE_BINARY_DIR}/SCL_block_lists/*.zip")

list(LENGTH SlopeCraft_block_list_archives num_archives)
if (${num_archives} LESS_EQUAL 0)
    message(FATAL_ERROR "No archive preset found")
endif ()

file(GLOB SlopeCraft_install_presets
    "${CMAKE_CURRENT_SOURCE_DIR}/others/presets/*.sc_preset_json")

set(AppName SlopeCraft)
#configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
#    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
#    @ONLY)

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
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks)
    file(COPY ${SlopeCraft_block_list_archives}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks/)

    # Install presets
    install(FILES ${SlopeCraft_install_presets}
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks/Presets)
    file(COPY ${SlopeCraft_install_presets}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks/Presets)

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

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Install app
    install(TARGETS SlopeCraft
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
    )

    QD_add_deployqt(SlopeCraft
        BUILD_MODE
        FLAGS ${SlopeCraft_windeployqt_flags_build})
    QD_add_deployqt(SlopeCraft
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_windeployqt_flags_install})
    DLLD_add_deploy(SlopeCraft
        BUILD_MODE
        INSTALL_MODE INSTALL_DESTINATION .
        OPTIONAL_DLLS "imageformats/*.dll")

    # Install all block list archives
    install(FILES ${SlopeCraft_block_list_archives}
        DESTINATION Blocks)
    file(COPY ${SlopeCraft_block_list_archives}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks)

    # Install presets
    install(FILES ${SlopeCraft_install_presets}
        DESTINATION Blocks/Presets)
    file(COPY ${SlopeCraft_install_presets}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/Presets)

    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    # set_target_properties(SlopeCraft PROPERTIES INSTALL_RPATH "../lib")
    # Install for Linux
    # Install app
    install(TARGETS SlopeCraft
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
    )

    # Install all block list archives
    install(FILES ${SlopeCraft_block_list_archives}
        DESTINATION bin/Blocks)
    file(COPY ${SlopeCraft_block_list_archives}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks)


    install(FILES others/SlopeCraftIconNew.png
        DESTINATION share/pixmaps
        RENAME com.github.SlopeCraft.SlopeCraft.png)
    install(FILES others/SlopeCraft.desktop
        DESTINATION share/applications)

    # Install presets
    install(FILES ${SlopeCraft_install_presets}
        DESTINATION bin/Blocks/Presets)
    file(COPY ${SlopeCraft_install_presets}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/Presets)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)

    return()
endif ()

message(WARNING "No rule to install SlopeCraft and images, because the system is not Windows, linux or MacOS.")
