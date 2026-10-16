if (${APPLE})
    install(TARGETS VisualCraftL
        EXPORT SlopeCraftTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
    return()
endif ()