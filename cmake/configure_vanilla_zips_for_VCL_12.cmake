unset(VCL_resource_12)

if(EXISTS ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_12_2.zip)
    set(VCL_resource_12 ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_12_2.zip)
    message(STATUS "Found resoruce pack for MC12.")
    return()
endif()

message(STATUS "Resource pack for MC12 not found. Downloading...")

file(DOWNLOAD
    https://github.com/SlopeCraft/VisualCraft-binaries/releases/download/resource-packs/Vanilla_1_12_2.zip
    ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_12_2.zip SHOW_PROGRESS)

if(EXISTS ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_12_2.zip)
    set(VCL_resource_12 ${CMAKE_SOURCE_DIR}/binaries/Vanilla_1_12_2.zip)
    message(STATUS "Downloaded resoruce pack for MC12.")
else()
    message(FATAL_ERROR "Failed to download Vanilla_1_12_2.zip.")
endif()