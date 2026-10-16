
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS SlopeCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    return()
endif ()

# add_subdirectory(SCL_C)
