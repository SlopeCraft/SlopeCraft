install(TARGETS imageCutter MapViewer
    SlopeCraftL SlopeCraft sccl
    VisualCraftL VisualCraft vccl
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
)

install(FILES ${SlopeCraft_readme_and_license_files}
    DESTINATION .
)

####################### for imageCutter
QD_add_deployqt(imageCutter
    BUILD_MODE
    FLAGS ${SlopeCraft_windeployqt_flags_build}
)
QD_add_deployqt(imageCutter
    INSTALL_MODE INSTALL_DESTINATION .
    FLAGS ${SlopeCraft_windeployqt_flags_install}
)
DLLD_add_deploy(imageCutter
    BUILD_MODE
    INSTALL_MODE INSTALL_DESTINATION .
)

####################### for MapViewer
QD_add_deployqt(MapViewer
    BUILD_MODE
    FLAGS ${SlopeCraft_windeployqt_flags_build}
)
QD_add_deployqt(MapViewer
    INSTALL_MODE INSTALL_DESTINATION .
    FLAGS ${SlopeCraft_windeployqt_flags_install}
)
DLLD_add_deploy(MapViewer
    BUILD_MODE
    INSTALL_MODE INSTALL_DESTINATION .
)

####################### mc_schem
install(FILES ${mc_schem_SHARED_LIB_LOCATION}
    DESTINATION .)

####################### for SlopeCraftL
DLLD_add_deploy(SlopeCraftL
    INSTALL_MODE INSTALL_DESTINATION .
)

####################### for SlopeCraft
QD_add_deployqt(SlopeCraft
    BUILD_MODE
    FLAGS ${SlopeCraft_windeployqt_flags_build}
)
QD_add_deployqt(SlopeCraft
    INSTALL_MODE INSTALL_DESTINATION .
    FLAGS ${SlopeCraft_windeployqt_flags_install}
)
DLLD_add_deploy(SlopeCraft
    BUILD_MODE
    INSTALL_MODE INSTALL_DESTINATION .
    OPTIONAL_DLLS "imageformats/*.dll"
)
# Install all block list archives
install(FILES ${SlopeCraft_block_list_archives}
    DESTINATION Blocks_SCL)
# Install presets
install(FILES ${SlopeCraft_install_presets}
    DESTINATION Blocks_SCL/Presets)
# Run windeployqt at build time
#    add_custom_target(Windeployqt-imageCutter
#        COMMAND ${SlopeCraft_Qt_windeployqt_executable} imageCutter.exe ${SlopeCraft_windeployqt_flags_build}
#        COMMAND_EXPAND_LISTS
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        DEPENDS imageCutter)
#    add_dependencies(SC_deploy_all Windeployqt-imageCutter)
#
#    # Run windeployqt at install time
#    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)

####################### for sccl
QD_add_deployqt(sccl
    BUILD_MODE
    FLAGS ${SlopeCraft_windeployqt_flags_build}
)
QD_add_deployqt(sccl
    INSTALL_MODE INSTALL_DESTINATION .
    FLAGS ${SlopeCraft_windeployqt_flags_install}
)
DLLD_add_deploy(sccl
    BUILD_MODE
    INSTALL_MODE INSTALL_DESTINATION .
    IGNORE SlopeCraftL.dll libSlopeCraftL.dll
)

####################### for visual family
set(VCL_ZIPS_PREFIX "./Blocks_VCL")
include(${PROJECT_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
install(FILES ${VCL_app_files}
    DESTINATION Blocks_VCL
)
####################### for VCL
DLLD_add_deploy(VisualCraftL
    INSTALL_MODE INSTALL_DESTINATION .
)

####################### for VisualCraft
# This json file tells VisualCraft where to find resources
set(vc_config_file_location ${PROJECT_BINARY_DIR}/VisualCraft/vc-config-install.json)
configure_file(${PROJECT_SOURCE_DIR}/VisualCraft/vc-config-install.json.in ${vc_config_file_location})
install(FILES ${vc_config_file_location}
    RENAME vc-config.json
    DESTINATION .
)
QD_add_deployqt(VisualCraft
    BUILD_MODE
    FLAGS ${SlopeCraft_windeployqt_flags_build}
)
QD_add_deployqt(VisualCraft
    INSTALL_MODE INSTALL_DESTINATION .
    FLAGS ${SlopeCraft_windeployqt_flags_install}
)
DLLD_add_deploy(VisualCraft
    BUILD_MODE
    INSTALL_MODE INSTALL_DESTINATION .
    IGNORE VisualCraftL.dll libVisualCraftL.dll
)

####################### for vccl
# This json file tells vccl where to find resources
set(vccl_config_file_location ${PROJECT_BINARY_DIR}/vccl/vccl-config-install.json)
configure_file(${PROJECT_SOURCE_DIR}/vccl/vccl-config-install.json.in ${vccl_config_file_location})

install(FILES ${vccl_config_file_location}
    RENAME vccl-config.json
    DESTINATION .
)
QD_add_deployqt(vccl
    BUILD_MODE
    FLAGS ${SlopeCraft_windeployqt_flags_build}
)
QD_add_deployqt(vccl
    INSTALL_MODE INSTALL_DESTINATION .
    FLAGS ${SlopeCraft_windeployqt_flags_install}
)
DLLD_add_deploy(vccl
    BUILD_MODE
    INSTALL_MODE INSTALL_DESTINATION .
    IGNORE VisualCraftL.dll libVisualCraftL.dll
)
#    # Run windeployqt at build time
#    add_custom_target(Windeployqt-vccl
#        COMMAND ${SlopeCraft_Qt_windeployqt_executable} vccl.exe ${SlopeCraft_windeployqt_flags_build}
#        COMMAND_EXPAND_LISTS
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        DEPENDS vccl)
#    add_dependencies(SC_deploy_all Windeployqt-vccl)
#
#    # Run windeployqt at install time
#    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/deploy_qt.cmake)