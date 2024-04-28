# install C shared lib
# set(SlopeCraft_SCL_C_include_files
#
# ${CMAKE_SOURCE_DIR}/utilities/SC_GlobalEnums.h
# SlopeCraftL.h
# SCL_C/SlopeCraftL_global.h # a copy from ../SlopeCraft_global.h, an extra line `#define SCL_CAPI` is added in this file
# )

# install Cpp sharerd lib
set(SlopeCraft_SCL_Cpp_include_files

    ${CMAKE_SOURCE_DIR}/utilities/SC_GlobalEnums.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SlopeCraftL.h
    ${CMAKE_CURRENT_BINARY_DIR}/SlopeCraftL_export.h
)
install(FILES ${SlopeCraft_SCL_Cpp_include_files}
    DESTINATION include/SlopeCraft)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    # install for applications
    install(TARGETS SlopeCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .)

    # install(TARGETS SlopeCraftL_C
    # RUNTIME DESTINATION ./../install_SlopeCraftL/C/bin
    # LIBRARY DESTINATION ./../install_SlopeCraftL/C/lib

    # LIBRARY DESTINATION ./../install_SlopeCraftL/C/lib
    # )

    DLLD_add_deploy(SlopeCraftL
        INSTALL_MODE INSTALL_DESTINATION .)

    # install(FILES ${SlopeCraft_SCL_C_include_files}
    # DESTINATION ./../install_SlopeCraftL/C/include)
    #    SlopeCraft_install_if_is_shared(ZLIB::ZLIB .)
    #    SlopeCraft_install_if_is_shared(PNG::PNG .)

    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Install SlopeCraft for linux")

    # install for applications
    install(TARGETS SlopeCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

    return()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS SlopeCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    return()
endif ()

# add_subdirectory(SCL_C)
