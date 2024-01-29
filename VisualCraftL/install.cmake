include(setup_zip_names.cmake)

set(VCL_include_headers
    VisualCraftL.h
    VisualCraftL_global.h)
install(FILES ${VCL_include_headers}
    DESTINATION include/SlopeCraft)

if (${WIN32})
    # install for app
    install(TARGETS VisualCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .

        # LIBRARY DESTINATION .
    )
    install(FILES ${VCL_app_files}
        DESTINATION Blocks_VCL)

    DLLD_add_deploy(VisualCraftL
        INSTALL_MODE INSTALL_DESTINATION .)

    return()
endif ()

if (${LINUX})
    install(TARGETS VisualCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

    #    install(TARGETS VisualCraftL_static
    #        EXPORT SlopeCraftTargets
    #        RUNTIME DESTINATION bin
    #        LIBRARY DESTINATION lib)

    install(FILES ${VCL_app_files}
        DESTINATION bin/Blocks_VCL)
    return()
endif ()

if (${APPLE})
    install(TARGETS VisualCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    return()
endif ()

message(WARNING "No rule to install VisualCraftL")