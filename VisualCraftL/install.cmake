include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_12.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/configure_vanilla_zips_for_VCL_latest.cmake)

set(VCL_app_files
    ${VCL_resource_12}
    ${VCL_resource_latest}
    ${CMAKE_CURRENT_SOURCE_DIR}/VCL_blocks_fixed.json)

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
    install(FILES VisualCraftL.h VisualCraftL_global.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/../install_SlopeCraftL/Cpp/include)
    return()
endif()