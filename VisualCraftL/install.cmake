include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_12.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_latest.cmake)

set(VCL_app_files
    ${VCL_resource_12}
    ${VCL_resource_latest}
    ${CMAKE_CURRENT_SOURCE_DIR}/VCL_blocks_fixed.json)

set(VCL_include_headers
    VisualCraftL.h
    VisualCraftL_global.h)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # install for app
    install(TARGETS VisualCraftL
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
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

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
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

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    return()
endif()