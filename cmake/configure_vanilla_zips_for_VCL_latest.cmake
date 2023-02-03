unset(VCL_resource_latest)

if(EXISTS ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_19_2.zip)
    set(VCL_resource_latest ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_19_2.zip)
    message(STATUS "Found resoruce pack for MC19.")
    return()
endif()

message(STATUS "Resource pack for MC19 not found. Downloading...")

file(DOWNLOAD
    https://github.com/SlopeCraft/VisualCraft-binaries/releases/download/resource-packs/Vanilla_1_19_2.zip
    ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_19_2.zip SHOW_PROGRESS)

if(EXISTS ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_19_2.zip)
    set(VCL_resource_latest ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_19_2.zip)
    message(STATUS "Downloaded resoruce pack for latest MC.")
else()
    message(FATAL_ERROR "Failed to download latest resource pack.")
endif()