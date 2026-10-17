
install(FILES ${SlopeCraft_readme_and_license_files}
    DESTINATION .
)

####################### for imageCutter
install(TARGETS imageCutter
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .
)
file(GLOB imageCutter_Icon
    ${PROJECT_SOURCE_DIR}/imageCutter/others/imageCutterIconNew.icns)
install(FILES ${imageCutter_Icon}
    DESTINATION imageCutter.app/Contents/Resources
)
qt6_generate_deploy_app_script(
    TARGET imageCutter
    OUTPUT_SCRIPT imageCutter_deploy_script
    DEPLOY_TOOL_OPTIONS ${SlopeCraft_macdeployqt_flags_install}
)
install(SCRIPT ${imageCutter_deploy_script})
#QD_add_deployqt(imageCutter
#    INSTALL_MODE INSTALL_DESTINATION .
#    FLAGS ${SlopeCraft_macdeployqt_flags_install}
#)
#DylibD_add_deploy(imageCutter
#    INSTALL_DESTINATION .
#    RPATH_POLICY REPLACE
#)
RCS_add_codesign(imageCutter
    INSTALL_DESTINATION .
)

####################### for MapViewer
install(TARGETS MapViewer
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .
)
file(GLOB MapViewer_Icon ${PROJECT_SOURCE_DIR}/MapViewer/others/MapViewer.icns)
install(FILES ${MapViewer_Icon}
    DESTINATION MapViewer.app/Contents/Resources
)
qt6_generate_deploy_app_script(
    TARGET MapViewer
    OUTPUT_SCRIPT MapViewer_deploy_script
    DEPLOY_TOOL_OPTIONS ${SlopeCraft_macdeployqt_flags_install}
)
install(SCRIPT ${MapViewer_deploy_script})
#QD_add_deployqt(MapViewer
#    INSTALL_MODE INSTALL_DESTINATION .
#    FLAGS ${SlopeCraft_macdeployqt_flags_install}
#)
#DylibD_add_deploy(MapViewer
#    INSTALL_DESTINATION .
#    RPATH_POLICY REPLACE)
RCS_add_codesign(MapViewer
    INSTALL_DESTINATION .
)

####################### for SlopeCraft, SlopeCraftL and sccl
install(TARGETS SlopeCraft
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .
)
# Put sccl(command line exe) inside SlopeCraft bundle. They share SCL and resources.
install(TARGETS sccl
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION SlopeCraft.app/Contents/MacOS
)
install(TARGETS SlopeCraftL
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION SlopeCraft.app/Contents/Frameworks
    LIBRARY DESTINATION SlopeCraft.app/Contents/Frameworks
)
set(SlopeCraft_Icon ${PROJECT_SOURCE_DIR}/SlopeCraft/others/SlopeCraftIconNew.icns)
install(FILES ${SlopeCraft_Icon}
    DESTINATION SlopeCraft.app/Contents/Resources
    RENAME SlopeCraft.icns
)
# Install resources. SlopeCraft and sccl should obtain resources from ../Resources
install(FILES ${SlopeCraft_block_list_archives}
    DESTINATION SlopeCraft.app/Contents/Resources/Blocks_SCL
)
# Install presets
install(FILES ${SlopeCraft_install_presets}
    DESTINATION SlopeCraft.app/Contents/Resources/Blocks_SCL/Presets
)
qt6_generate_deploy_script(
    TARGET sccl
    OUTPUT_SCRIPT sccl_deploy_script
    DEPLOY_TOOL_OPTIONS ${SlopeCraft_macdeployqt_flags_install}
)
qt6_generate_deploy_app_script(
    TARGET SlopeCraft
    OUTPUT_SCRIPT SlopeCraft_deploy_script
    DEPLOY_TOOL_OPTIONS ${SlopeCraft_macdeployqt_flags_install}
)
install(SCRIPT ${sccl_deploy_script})
install(SCRIPT ${SlopeCraft_deploy_script})
#QD_add_deployqt(SlopeCraft
#    INSTALL_MODE INSTALL_DESTINATION .
#    FLAGS ${SlopeCraft_macdeployqt_flags_install}
#)
#DylibD_add_deploy(SlopeCraft
#    INSTALL_DESTINATION .
#    RPATH_POLICY REPLACE)
RCS_add_codesign(SlopeCraft
    INSTALL_DESTINATION .
)

####################### for VisualCraft, VisualCraftL and vccl
install(TARGETS VisualCraft
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .
)
# Put vccl inside VC, like sccl. Also share lib and resources
install(TARGETS vccl
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION VisualCraft.app/Contents/MacOS
)
install(TARGETS VisualCraftL
    EXPORT SlopeCraftTargets
    RUNTIME DESTINATION VisualCraft.app/Contents/Frameworks
    LIBRARY DESTINATION VisualCraft.app/Contents/Frameworks
)
install(FILES ${PROJECT_SOURCE_DIR}/VisualCraft/others/VisualCraft.icns
    DESTINATION VisualCraft.app/Contents/Resources
)
# Install VCL block list json and zips
include(${PROJECT_SOURCE_DIR}/VisualCraftL/setup_zip_names.cmake)
install(FILES ${VCL_app_files}
    DESTINATION VisualCraft.app/Contents/Resources/Blocks_VCL
)
# Install vc and vccl config json. config json tells where to find resource (by relative path)
set(VCL_ZIPS_PREFIX "../Resources/Blocks_VCL")
set(vc_config_file_location ${PROJECT_BINARY_DIR}/VisualCraft/vc-config-install.json)
set(vccl_config_file_location ${PROJECT_BINARY_DIR}/vccl/vccl-config-install.json)
configure_file(${PROJECT_SOURCE_DIR}/VisualCraft/vc-config-install.json.in ${vc_config_file_location})
configure_file(${PROJECT_SOURCE_DIR}/vccl/vccl-config-install.json.in ${vccl_config_file_location})
install(FILES ${vc_config_file_location} ${vccl_config_file_location}
    DESTINATION VisualCraft.app/Contents/Resources
)
qt6_generate_deploy_script(
    TARGET vccl
    OUTPUT_SCRIPT vccl_deploy_script
    DEPLOY_TOOL_OPTIONS ${SlopeCraft_macdeployqt_flags_install}
)
qt6_generate_deploy_app_script(
    TARGET VisualCraft
    OUTPUT_SCRIPT VisualCraft_deploy_script
    DEPLOY_TOOL_OPTIONS ${SlopeCraft_macdeployqt_flags_install}
)
install(SCRIPT ${vccl_deploy_script})
install(SCRIPT ${VisualCraft_deploy_script})
#QD_add_deployqt(VisualCraft
#    INSTALL_MODE INSTALL_DESTINATION .
#    FLAGS ${SlopeCraft_macdeployqt_flags_install}
#)
RCS_add_codesign(VisualCraft
    INSTALL_DESTINATION .
)