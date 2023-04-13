include(setup_zip_names.cmake)

set(VCL_include_headers
    VisualCraftL.h
    VisualCraftL_global.h)

if(${WIN32})
    # install for app
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION .

        # LIBRARY DESTINATION .
    )
    install(FILES ${VCL_app_files}
        DESTINATION Blocks_VCL)

    # install for lib
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION ../install_SlopeCraftL/Cpp/bin
        LIBRARY DESTINATION ../install_SlopeCraftL/Cpp/lib)

    # install(TARGETS VisualCraftL_static
    # RUNTIME DESTINATION ../install_SlopeCraftL/Cpp/bin
    # LIBRARY DESTINATION ../install_SlopeCraftL/Cpp/lib)
    install(FILES ${VCL_include_headers}
        DESTINATION ../install_SlopeCraftL/Cpp/include)
    SlopeCraft_install_if_is_shared(ZLIB::ZLIB .)
    SlopeCraft_install_if_is_shared(libzip::zip .)
    SlopeCraft_install_if_is_shared(PNG::PNG .)
    return()
endif()

if(${LINUX})
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

    install(TARGETS VisualCraftL_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

    install(FILES ${VCL_app_files}
        DESTINATION bin/Blocks_VCL)
    install(FILES ${VCL_include_headers}
        DESTINATION include)

    # install(TARGETS libzip::zip
    # RUNTIME DESTINATION bin
    # LIBRARY DESTINATION lib)
    return()
endif()

if(${APPLE})
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .)

    # zips and blocklist jsons are installed inside each macos bundle
    # Install zips. In vccl-config.json or vc-config.json, they are refered like ./Blocks_VCL/Vanilla_1_19_3.zip
    # install(FILES ${VCL_app_files}
    # DESTINATION Blocks_VCL)
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION ../install_SlopeCraftL/Cpp/bin
        LIBRARY DESTINATION ../install_SlopeCraftL/Cpp/lib)
    return()
endif()

message(WARNING "No rule to install VisualCraftL")