set(VCL_zip_names
        "Vanilla_1_12_2.zip"
        "Vanilla_1_13_2.zip"
        "Vanilla_1_14_4.zip"
        "Vanilla_1_15_2.zip"
        "Vanilla_1_16_5.zip"
        "Vanilla_1_17_1.zip"
        "Vanilla_1_18_2.zip"
        "Vanilla_1_19_3.zip"
        "Vanilla_1_20_1.zip")

set(VCL_url_prefix "https://github.com/SlopeCraft/VisualCraft-binaries/releases/download/resource-packs/")

foreach (mcver RANGE 12 20)
    math(EXPR VCL_resource_idx "${mcver} - 12")

    # message(STATUS "VCL_resource_idx = ${VCL_resource_idx}")
    list(GET VCL_zip_names ${VCL_resource_idx} VCL_current_zip_name)

    set(VCL_resource_${mcver} ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name} CACHE FILEPATH "")

    if (EXISTS ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})
        # message(STATUS "Found resoruce pack for MC${mcver} (${VCL_current_zip_name}).")
        continue()
    endif ()

    message(STATUS "${VCL_current_zip_name} not found. Downloading...")

    file(DOWNLOAD
            ${VCL_url_prefix} ${VCL_current_zip_name}
            ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})

    if (EXISTS ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})
        message(STATUS "Downloaded resoruce pack for MC${mcver}.")
    else ()
        message(FATAL_ERROR "Failed to download resource pack for MC${mcver}.")
    endif ()
endforeach (mcver RANGE 12 20)