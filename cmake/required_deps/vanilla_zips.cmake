set(VCL_zip_names
    "Vanilla_1_12_2.zip"
    "Vanilla_1_13_2.zip"
    "Vanilla_1_14_4.zip"
    "Vanilla_1_15_2.zip"
    "Vanilla_1_16_5.zip"
    "Vanilla_1_17_1.zip"
    "Vanilla_1_18_2.zip"
    "Vanilla_1_19_3.zip"
    "Vanilla_1_20_6.zip"
    "Vanilla_1_21_11.zip"
    "Vanilla_26_1_2.zip"
    "Vanilla_26_2.zip"
    "Vanilla_26_3.zip"
)
set(VCL_zip_hashes
    SHA256=357ab131461581927caf47093d56d75d115c7426ad4466e5e448c40f5620fb42 # Vanilla_1_12_2.zip
    SHA256=1bec61b5f56239c116359c3c04843c08d6fda0dd181eb21bb0f44e556b5aa182 # Vanilla_1_13_2.zip
    SHA256=c762bb384a9fbf937f81d5a892b16b28d0d943a8da69d4e11cbfdb44767e5090 # Vanilla_1_14_4.zip
    SHA256=3895246b308378b66ee37db700e1f27811e1b371fea52123f007a5674e89078b # Vanilla_1_15_2.zip
    SHA256=66c71ef98c3c4a74ea60e3dbd465164d34ec4678389c7e0a0491fe3366307625 # Vanilla_1_16_5.zip
    SHA256=e6d3f19ec13d0a001c4f5a8e95dd6f4352d80e2dc5af34be4161899f99a6aa8e # Vanilla_1_17_1.zip
    SHA256=5eaddb32a79e020120004bbcdafa620f03128159fc53e37992f659425351f730 # Vanilla_1_18_2.zip
    SHA256=2a30be0768cde7bc6e3dc657f13e3b0f41b71f7253ad53bbe4c91dba4b82bcb7 # Vanilla_1_19_3.zip
    SHA256=3840824c59ebb36c19c8605786b99cc30e5541cd31cddec2c887f1370cef061e # Vanilla_1_20_6.zip
    SHA256=f2039093f08e53d2bc6f3a1770ae6966a5ab3618e899dd0bdc7b69816ced71d2 # Vanilla_1_21_11.zip
    SHA256=3a80b38f00461f4b9f546c3f702fee72b09c674a0bfc477e2c627576161ca2d8 # Vanilla_26_1_2.zip
    SHA256=6acaba4c13199f9d32d21d5a82e09909a9adf3755cc1bd03d310d75c67fe30ca # Vanilla_26_2.zip
    SHA256=b0d46fa98b75d104e53db979b9f31922fab34bd3c106d78ed4389ca6f604a9f4 # Vanilla_26_3.zip
)
set(VCL_version_names
    12
    13
    14
    15
    16
    17
    18
    19
    20
    21
    26_1
    26_2
    26_3
)

list(LENGTH VCL_zip_names vcl_zip_num)
list(LENGTH VCL_version_names vcl_version_num)

if (NOT ${vcl_zip_num} EQUAL ${vcl_version_num})
    message(FATAL_ERROR "VCL zip filenames and versions not match: ${VCL_zip_names} and ${VCL_version_names}")
endif ()

#https://github.com/SlopeCraft/VisualCraft-binaries/releases/download/resource-packs/Vanilla_1_20_1.zip
set(VCL_url_prefix "https://github.com/SlopeCraft/VisualCraft-binaries/releases/download/resource-packs/")

math(EXPR VCL_resource_idx_max "${vcl_zip_num}-1")
foreach (VCL_resource_idx RANGE 0 ${VCL_resource_idx_max})
    # message(STATUS "VCL_resource_idx = ${VCL_resource_idx}")
    list(GET VCL_zip_names ${VCL_resource_idx} VCL_current_zip_name)
    list(GET VCL_version_names ${VCL_resource_idx} mcver)
    list(GET VCL_zip_hashes ${VCL_resource_idx} VCL_zip_hash)

    set(VCL_resource_${mcver} ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name} CACHE FILEPATH "")

    if (EXISTS ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})
        message(STATUS "Found resource pack for MC${mcver} (${VCL_current_zip_name}), skip downloading.")
        continue()
    endif ()

    message(STATUS "${VCL_current_zip_name} not found. Downloading...")

    string(CONCAT VCL_current_download_url ${VCL_url_prefix} ${VCL_current_zip_name})
    file(DOWNLOAD
        ${VCL_current_download_url}
        ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name}
        EXPECTED_HASH ${VCL_zip_hash}
    )

    if (EXISTS ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})
        message(STATUS "Downloaded resource pack for MC${mcver}.")
    else ()
        message(FATAL_ERROR "Failed to download resource pack for MC${mcver} from ${VCL_current_download_url}.")
    endif ()
endforeach ()

#foreach (mcver RANGE 12 21)
#    math(EXPR VCL_resource_idx "${mcver} - 12")
#
#    # message(STATUS "VCL_resource_idx = ${VCL_resource_idx}")
#    list(GET VCL_zip_names ${VCL_resource_idx} VCL_current_zip_name)
#
#    set(VCL_resource_${mcver} ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name} CACHE FILEPATH "")
#
#    if (EXISTS ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})
#        # message(STATUS "Found resoruce pack for MC${mcver} (${VCL_current_zip_name}).")
#        continue()
#    endif ()
#
#    message(STATUS "${VCL_current_zip_name} not found. Downloading...")
#
#    string(CONCAT VCL_current_download_url ${VCL_url_prefix} ${VCL_current_zip_name})
#    file(DOWNLOAD
#        ${VCL_current_download_url}
#        ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})
#
#    if (EXISTS ${CMAKE_SOURCE_DIR}/binaries/${VCL_current_zip_name})
#        message(STATUS "Downloaded resoruce pack for MC${mcver}.")
#    else ()
#        message(FATAL_ERROR "Failed to download resource pack for MC${mcver} from ${VCL_current_download_url}.")
#    endif ()
#endforeach ()