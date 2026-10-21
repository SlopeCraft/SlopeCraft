
include(GNUInstallDirs)
set(SlopeCraft_readme_and_license_files
    LICENSE
    README.md
    README-en.md
    license-translations/LICENSE-zh.md
)

file(GLOB SlopeCraft_install_presets
    "${PROJECT_SOURCE_DIR}/SlopeCraft/others/presets/*.sc_preset_json")
# Actually these calls are only helpful for develop. just keep them here
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/SCL_block_lists)
file(COPY ${SlopeCraft_install_presets}
    DESTINATION ${PROJECT_BINARY_DIR}/SCL_block_lists/Presets)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include(${CMAKE_CURRENT_LIST_DIR}/install_rules/windows.cmake)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include(${CMAKE_CURRENT_LIST_DIR}/install_rules/linux.cmake)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include(${CMAKE_CURRENT_LIST_DIR}/install_rules/mac.cmake)
else ()
    message(WARNING "Unknown target system ${CMAKE_SYSTEM_NAME}, will try to install like linux")
    include(${CMAKE_CURRENT_LIST_DIR}/install_rules/linux.cmake)
endif ()
# Header files for slopecraft. SCL and VCL are designed to allow secondary development.
set(SlopeCraft_cpp_include_files
    ${PROJECT_SOURCE_DIR}/utilities/SC_GlobalEnums.h
    ${PROJECT_BINARY_DIR}/utilities/SC_version_buildtime.h
    ${PROJECT_SOURCE_DIR}/SlopeCraftL/SlopeCraftL.h
    ${PROJECT_BINARY_DIR}/SlopeCraftL/SlopeCraftL_export.h
    ${PROJECT_SOURCE_DIR}/VisualCraftL/VisualCraftL.h
    ${PROJECT_SOURCE_DIR}/VisualCraftL/VisualCraftL_global.h
)
install(FILES ${SlopeCraft_cpp_include_files}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SlopeCraft
)

include(${CMAKE_CURRENT_LIST_DIR}/add_compiler_path_to_prefix.cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/SlopeCraftConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/SlopeCraftConfig.cmake.in
    ${PROJECT_BINARY_DIR}/SlopeCraftConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SlopeCraft
)
install(FILES
    ${PROJECT_BINARY_DIR}/SlopeCraftConfigVersion.cmake
    ${PROJECT_BINARY_DIR}/SlopeCraftConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SlopeCraft
)

install(EXPORT SlopeCraftTargets
    FILE SlopeCraftTargets.cmake
    NAMESPACE SlopeCraft::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SlopeCraft
)
