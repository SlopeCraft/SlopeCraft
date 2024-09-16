include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(MCDataVersion)
add_subdirectory(version_set)
add_subdirectory(GPUWrapper)
add_subdirectory(ColorManip)
add_subdirectory(ProcessBlockId)
add_subdirectory(AdaptiveLabel)
add_subdirectory(BlockListManager)
add_subdirectory(ExternalConverters)
add_subdirectory(Schem)
add_subdirectory(NBTWriter)
add_subdirectory(uiPack)
add_subdirectory(MapImageCvter)
add_subdirectory(VersionDialog)
add_subdirectory(VCLConfigLoader)
add_subdirectory(FlatDiagram)
add_subdirectory(libpngReader)
add_subdirectory(StatMemory)
add_subdirectory(sNBT_formatter)

function(SC_process_boolean value_name)
    if (${${value_name}})
        set(${value_name} "true" PARENT_SCOPE)
    else ()
        set(${value_name} "false" PARENT_SCOPE)
    endif ()
endfunction(SC_process_boolean value_name)

SC_process_boolean(SlopeCraft_vectorize)
SC_process_boolean(SlopeCraft_gprof)

configure_file(SC_version_buildtime.h.in
    SC_version_buildtime.h)

include(install.cmake)

# add_subdirectory(libAbstractGUI)
# add_subdirectory(libSCGUI)