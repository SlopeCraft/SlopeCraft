
install(TARGETS imageCutter MapViewer
    SlopeCraftL SlopeCraft sccl
    VisualCraftL VisualCraft vccl
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${SlopeCraft_readme_and_license_files}
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/SlopeCraft
)
####################### for imageCutter
install(FILES ${PROJECT_SOURCE_DIR}/imageCutter/others/imageCutterIconNew.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps
    RENAME com.github.SlopeCraft.imageCutter.png
)
install(FILES ${PROJECT_SOURCE_DIR}/imageCutter/others/imageCutter.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)
####################### for MapViewer
install(FILES ${PROJECT_SOURCE_DIR}/MapViewer/others/MapViewer_64.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps
    RENAME com.github.SlopeCraft.MapViewer.png
)
install(FILES ${PROJECT_SOURCE_DIR}/MapViewer/others/MapViewer.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

####################### for SlopeCraft
# Install all block list archives
install(FILES ${SlopeCraft_block_list_archives}
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/SlopeCraft/Blocks_SCL
)
# Install presets
install(FILES ${SlopeCraft_install_presets}
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/SlopeCraft/Blocks_SCL/Presets
)
install(FILES ${PROJECT_SOURCE_DIR}/SlopeCraft/others/SlopeCraftIconNew.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps
    RENAME com.github.SlopeCraft.SlopeCraft.png
)
install(FILES ${PROJECT_SOURCE_DIR}/SlopeCraft/others/SlopeCraft.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

####################### for visual family
set(VCL_ZIPS_PREFIX "../${CMAKE_INSTALL_DATAROOTDIR}/SlopeCraft/Blocks_VCL")
include(${PROJECT_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
install(FILES ${VCL_app_files}
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/SlopeCraft/Blocks_VCL
)

####################### for VisualCraft
# This json file tells VisualCraft where to find resources
set(vc_config_file_location ${PROJECT_BINARY_DIR}/VisualCraft/vc-config-install.json)
configure_file(${PROJECT_SOURCE_DIR}/VisualCraft/vc-config-install.json.in ${vc_config_file_location})
install(FILES ${vc_config_file_location}
    RENAME vc-config.json
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/SlopeCraft
)
install(FILES ${PROJECT_SOURCE_DIR}/VisualCraft/others/Vc_256.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps
    RENAME com.github.SlopeCraft.VisualCraft.png
)
install(FILES ${PROJECT_SOURCE_DIR}/VisualCraft/others/VisualCraft.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

####################### for vccl
# This json file tells vccl where to find resources
set(vccl_config_file_location ${PROJECT_BINARY_DIR}/vccl/vccl-config-install.json)
configure_file(${PROJECT_SOURCE_DIR}/vccl/vccl-config-install.json.in ${vccl_config_file_location})
install(FILES ${vccl_config_file_location}
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/SlopeCraft
    RENAME vccl-config.json
)