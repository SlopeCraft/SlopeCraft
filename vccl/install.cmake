set(AppName vccl)

configure_file(${CMAKE_SOURCE_DIR}/cmake/deploy_qt.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake
    @ONLY)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS vccl
        RUNTIME DESTINATION .)
    install(FILES vccl-config.json
        DESTINATION .)

    # Run windeployqt at build time
    add_custom_target(Windeployqt-vccl ALL
        COMMAND ${SlopeCraft_Qt_windeployqt_executable} vccl.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS vccl)

    # Run windeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)
    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS vccl
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
    install(FILES vccl-config.json
        DESTINATION bin)

    # Install platforms and imageformats plugins
    include(${CMAKE_SOURCE_DIR}/cmake/install_plugins.cmake)

    return()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include(${CMAKE_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
    install(TARGETS vccl
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .)

    # Install config json file, vccl will try to find it by ./vccl-config.json
    install(FILES vccl-config.json
        DESTINATION vccl.app/Contents/MacOS)

    # Install zips. In vccl-config.json or vc-config.json, they are referred like ./Blocks_VCL/Vanilla_1_19_3.zip
    install(FILES ${VCL_app_files}
        DESTINATION vccl.app/Contents/MacOS/Blocks_VCL)

    # Run macdeployqt at install time
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

    return()
endif()