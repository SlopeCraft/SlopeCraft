

set(VCL_app_files
    ${CMAKE_CURRENT_SOURCE_DIR}/VCL_blocks_fixed.json)

include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips.cmake)

foreach(mcver RANGE 12 19)
    set(VCL_current_var_name VCL_resource_${mcver})

    if(NOT DEFINED ${VCL_current_var_name})
        message(WARNING "${VCL_current_var_name} is not defined")
    endif()

    list(APPEND VCL_app_files ${${VCL_current_var_name}})
endforeach(mcver RANGE 12 19)

unset(mcver)

set(VCL_include_headers
    VisualCraftL.h
    VisualCraftL_global.h)

if(${WIN32})
    # install for app
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}

        # LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    install(FILES ${VCL_app_files}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks_VCL)

    # install for lib
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/lib)
    install(TARGETS VisualCraftL_static
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/lib)
    install(FILES ${VCL_include_headers}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/include)
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
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})

    # Install zips. In vccl-config.json or vc-config.json, they are refered like ./Blocks_VCL/Vanilla_1_19_3.zip
    install(FILES ${VCL_app_files}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/Blocks_VCL)

    install(TARGETS VisualCraftL
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/lib)
    return()
endif()

message(WARNING "No rule to install VisualCraftL")