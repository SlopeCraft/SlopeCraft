if(NOT ${LINUX})
    message(WARNING "This script should be only called on Linux.")
endif()


install(FILES ${SlopeCraft_Qt_plugin_platform_files}
    DESTINATION bin/platforms)
install(FILES ${SlopeCraft_Qt_plugin_imageformat_files}
    DESTINATION bin/imageformats)