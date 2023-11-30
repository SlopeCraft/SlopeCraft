
file(GLOB SlopeCraft_install_jsons
    "${CMAKE_SOURCE_DIR}/Blocks/*.json"
    "${CMAKE_SOURCE_DIR}/Blocks/*.md")

file(GLOB SlopeCraft_install_png_fixedblocks
    "${CMAKE_SOURCE_DIR}/Blocks/FixedBlocks/*.png")

file(GLOB SlopeCraft_install_png_customblocks
    "${CMAKE_SOURCE_DIR}/Blocks/CustomBlocks/*.png")

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
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .
    )

    # Install icons
    set(SlopeCraft_Icon
        ${CMAKE_SOURCE_DIR}/SlopeCraft/others/SlopeCraftIconNew.icns)
    install(FILES ${SlopeCraft_Icon}
        DESTINATION SlopeCraft.app/Contents/Resources
        RENAME SlopeCraft.icns)

    # Install FixedBlocks.json, CustomBlocks.json and README.md
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks)
    file(COPY ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks)

    # Install all png files of fixedblocks
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks/FixedBlocks)
    file(COPY ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks/FixedBlocks)

    # Install all png files of customblocks
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks/CustomBlocks)
    file(COPY ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks/CustomBlocks)

    # Install presets
    install(FILES ${SlopeCraft_install_presets}
        DESTINATION SlopeCraft.app/Contents/MacOS/Blocks/Presets)
    file(COPY ${SlopeCraft_install_presets}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks/Presets)

    install(TARGETS SlopeCraftL
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
        RUNTIME DESTINATION .
    )

    #    # Run windeployqt at build time
    #    add_custom_target(Windeployqt-SlopeCraft
    #        COMMAND ${SlopeCraft_Qt_windeployqt_executable} SlopeCraft.exe ${SlopeCraft_windeployqt_flags_build}
    #        COMMAND_EXPAND_LISTS
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    #        DEPENDS SlopeCraft)
    #    add_dependencies(SC_deploy_all Windeployqt-SlopeCraft)
    #
    #    # Run windeployqt at install time
    #    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

    QD_add_deployqt(SlopeCraft
        BUILD_MODE
        FLAGS ${SlopeCraft_windeployqt_flags_build})
    QD_add_deployqt(SlopeCraft
        INSTALL_MODE INSTALL_DESTINATION .
        FLAGS ${SlopeCraft_windeployqt_flags_install})
    DLLD_add_deploy(SlopeCraft
        BUILD_MODE
        INSTALL_MODE INSTALL_DESTINATION .
        IGNORE SlopeCraftL.dll)

    # Install FixedBlocks.json, CustomBlocks.json and README.md
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION Blocks)
    file(COPY ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks)

    # Install all png files of fixedblocks
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION Blocks/FixedBlocks)
    file(COPY ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/FixedBlocks)

    # Install all png files of customblocks
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION Blocks/CustomBlocks)
    file(COPY ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/CustomBlocks)

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
        RUNTIME DESTINATION bin
    )

    # Install FixedBlocks.json, CustomBlocks.json and README.md
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION bin/Blocks)
    file(COPY ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks)

    # Install all png files of fixedblocks
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION bin/Blocks/FixedBlocks)
    file(COPY ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/FixedBlocks)

    # Install all png files of customblocks
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION bin/Blocks/CustomBlocks)
    file(COPY ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/CustomBlocks)


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
