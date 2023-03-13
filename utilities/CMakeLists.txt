include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file(SC_version_buildtime.h.in
    SC_version_buildtime.h)

include(install.cmake)

add_subdirectory(MCDataVersion)
add_subdirectory(GPUWrapper)
add_subdirectory(ColorManip)
add_subdirectory(BlockListManager)
add_subdirectory(ExternalConverters)
add_subdirectory(Schem)
add_subdirectory(NBTWriter)
add_subdirectory(uiPack)
add_subdirectory(MapImageCvter)
add_subdirectory(VersionDialog)

# add_subdirectory(libAbstractGUI)
# add_subdirectory(libSCGUI)