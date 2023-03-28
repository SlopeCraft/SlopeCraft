
file(GLOB SlopeCraft_install_jsons
    "${CMAKE_SOURCE_DIR}/Blocks/*.json"
    "${CMAKE_SOURCE_DIR}/Blocks/*.md")

file(GLOB SlopeCraft_install_png_fixedblocks
    "${CMAKE_SOURCE_DIR}/Blocks/FixedBlocks/*.png")

file(GLOB SlopeCraft_install_png_customblocks
    "${CMAKE_SOURCE_DIR}/Blocks/CustomBlocks/*.png")

file(GLOB SlopeCraft_install_presets
    "${CMAKE_CURRENT_SOURCE_DIR}/others/*.sc_preset_json")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Install for macOS
    # Install app
    install(TARGETS SlopeCraft
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

    # Install icons
    file(GLOB SlopeCraft_Icon
        ${CMAKE_SOURCE_DIR}/SlopeCraftMain/others/SlopeCraftIconNew.icns)
    install(FILES ${SlopeCraft_Icon}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/Resources)

    # Install FixedBlocks.json, CustomBlocks.json and README.md
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/MacOS/Blocks)
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks)

    # Install all png files of fixedblocks
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/MacOS/Blocks/FixedBlocks)
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks/FixedBlocks)

    # Install all png files of customblocks
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/SlopeCraft.app/Contents/MacOS/Blocks/CustomBlocks)
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MacOS/Blocks/CustomBlocks)
    
    # Install presets
    install(FILES ${SlopeCraft_install_presets} 
        DESTINATION ${CMAKE_INTALL_PREFIX}/SlopeCraft.app/Contents/MasOS/Blocks/Presets)
    install(FILES ${SlopeCraft_install_presets} 
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraft.app/Contents/MasOS/Blocks/Presets)
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Install for Windows.
    find_program(SlopeCraft_Qt_windeployqt_executable windeployqt PATHS ${CMAKE_PREFIX_PATH})

    if(SlopeCraft_Qt_windeployqt_executable)
        configure_file(others/deployqt_win.bat.in others/deployqt_win.bat)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/others/deployqt_win.bat
            DESTINATION ${CMAKE_INSTALL_PREFIX})
    endif()

    # SlopeCraft_Qt_windeployqt_executable

    # Install app
    install(TARGETS SlopeCraft
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

    # Install FixedBlocks.json, CustomBlocks.json and README.md
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks)
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks)

    # Install all png files of fixedblocks
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks/FixedBlocks)
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/FixedBlocks)

    # Install all png files of customblocks
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks/CustomBlocks)
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/CustomBlocks)
    
    # Install presets
    install(FILES ${SlopeCraft_install_presets} 
        DESTINATION ${CMAKE_INTALL_PREFIX}/Blocks/Presets)
    install(FILES ${SlopeCraft_install_presets} 
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/Presets)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # set_target_properties(SlopeCraft PROPERTIES INSTALL_RPATH "../lib")
    # Install for Linux
    # Install app
    install(TARGETS SlopeCraft
        RUNTIME DESTINATION bin
    )

    # Install FixedBlocks.json, CustomBlocks.json and README.md
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION bin/Blocks)
    install(FILES ${SlopeCraft_install_jsons}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks)

    # Install all png files of fixedblocks
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION bin/Blocks/FixedBlocks)
    install(FILES ${SlopeCraft_install_png_fixedblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/FixedBlocks)

    # Install all png files of customblocks
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION bin/Blocks/CustomBlocks)
    install(FILES ${SlopeCraft_install_png_customblocks}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/CustomBlocks)

    file(GLOB SlopeCraft_Icon ${CMAKE_SOURCE_DIR}/SlopeCraftMain/others/SlopeCraftIconNew.png)

    install(FILES ${SlopeCraft_Icon}
        DESTINATION bin/icons)
        
    # Install presets
    install(FILES ${SlopeCraft_install_presets} 
        DESTINATION bin/Blocks/Presets)
    install(FILES ${SlopeCraft_install_presets} 
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Blocks/Presets)

    return()
endif()

message(WARNING "No rule to install SlopeCraft and images, because the system is not Windows, linux or MacOS.")
